#include "main.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "block_viewer.cpp"
#include "chunk.cpp"
#include "input.cpp"
#include "models.cpp"
#include "noise.cpp"
#include "opengl.cpp"
#include "player.cpp"
#include "shaders.cpp"
#include "textures.cpp"
#include "utils.cpp"
#include "world.cpp"
#include "generated/block_render.cpp"

void* newChunkRenderMain(void* game_data_as_void_pointer) {
  GameData* game_data = (GameData*) game_data_as_void_pointer;
  WorldRenderState* render_state = &game_data->loaded_world.render_state;

  // The Mutex lock/unlock logic in here is a bit hairy. In some ways it's nice - it shows that we only unlock
  // the mutex whilst rendering the chunk (and implicitly when waiting for the condition), but having the first lock
  // outside the loop and only unlocking inside two inner loops doesn't seem right, and it takes some thinking to
  // convince yourself the logic is correct.
  Mutex* new_chunk_lock = &render_state->new_chunk_lock;
  lockMutex(new_chunk_lock);
  while (true) {
    Chunk* chunk = (Chunk*) removefromLinkedList(&render_state->new_chunks);
    while (chunk != NULL) {
      unlockMutex(new_chunk_lock);
      fillChunkRenderData(chunk, &game_data->loaded_world);
      lockMutex(new_chunk_lock);
      chunk = (Chunk*) removefromLinkedList(&render_state->new_chunks);
    }
    waitForCondition(&render_state->new_chunk_condition, new_chunk_lock);
  }
}

void renderWorld(LoadedWorld* loaded_world, GLuint terrain_shader, Matrix4x4* view, Matrix4x4* projection) {
  glUseProgram(terrain_shader);
  GLuint transformLocation = glGetUniformLocation(terrain_shader, "view");
  glUniformMatrix4fv(transformLocation, 1, GL_TRUE, (float*)view);
  transformLocation = glGetUniformLocation(terrain_shader, "projection");
  glUniformMatrix4fv(transformLocation, 1, GL_TRUE, (float*)projection);

  for(int x = 0; x < LOADED_WORLD_SIZE; x++) {
    for(int y = 0; y < LOADED_WORLD_SIZE; y++) {
      for(int z = 0; z < LOADED_WORLD_SIZE; z++) {
        Chunk* c = getChunkAt(loaded_world, v3i(x, y, z));
        switch (c->render_data.state) {
          case NO_RENDER_DATA:
          case CHUNK_RENDER_DATA_STATE_COUNT:
            break;

          case NOT_PASSED_TO_OPENGL:
            fillChunkVao(c);
            // The break here is missing on purpose, so that a Chunk with a new VAO is rendered.
          case OKAY:
            Matrix4x4 model = translate(v3((x - LOADED_WORLD_SIZE/2) * CHUNK_SIZE,
                                           (y - LOADED_WORLD_SIZE/2) * CHUNK_SIZE,
                                           (z - LOADED_WORLD_SIZE/2) * CHUNK_SIZE));
            transformLocation = glGetUniformLocation(terrain_shader, "model");
            glUniformMatrix4fv(transformLocation, 1, GL_TRUE, (float*)&model);
            ChunkRenderData* render_data = &c->render_data;
            glBindVertexArray(render_data->vao);
            glDrawArrays(GL_TRIANGLES, 0, render_data->num_vertices);
            break;
        }
      }
    }
  }
}


int main(void) {
  setbuf(stdout, NULL);
  GLFWwindow* window;
  bool success = initOpenGLAndCreateWindow(&window);
  if (!success) {
    return -1;
  }

  GLuint terrain_shader = compileShaderOrDie("resources/shaders/shader_no_texture.vs",
                                                        "resources/shaders/shader_no_texture.fs");

  GameData* game_data = (GameData*) malloc(sizeof(GameData));
  initWorld(&game_data->loaded_world);

  pthread_t chunk_render_thread;
  pthread_create(&chunk_render_thread, NULL, newChunkRenderMain, game_data);

  initInput();
  initPlayer(&game_data->player);

  bool block_viewer_mode = false;
  BlockViewerData* block_viewer_data = (BlockViewerData*) malloc(sizeof(BlockViewerData));
  initBlockViewer(block_viewer_data);

  float t = 0.0;

  int frames = 0;
  float acc = 0;

  struct timespec old_time, new_time;
  clock_gettime(CLOCK_MONOTONIC_RAW, &old_time);
  while(!glfwWindowShouldClose(window) && !isKeyDown(QUIT_KEY))
  {
    clock_gettime(CLOCK_MONOTONIC_RAW, &new_time);
    float delta = (new_time.tv_sec - old_time.tv_sec) + (new_time.tv_nsec - old_time.tv_nsec) / 1000000000.0f;
    old_time = new_time;


    frames += 1;
    acc += delta;
    if (acc >= 0.5) {
      printf("fps: %2.2f\n", frames/acc);
      acc = 0;
      frames = 0;
    }

    t += delta;

    // UPDATE

    if (isKeyPressed(SWITCH_TO_BLOCK_VIEWER_KEY)) {
      block_viewer_mode = !block_viewer_mode;
    }

    if (block_viewer_mode) {
      updateBlockViewer(block_viewer_data);
    } else {
      updatePlayer(&game_data->player, delta);
    }

    // RENDER
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (block_viewer_mode) {
      renderBlockViewer(block_viewer_data, t, terrain_shader);
    } else {
      Player* player = &game_data->player;
      Matrix4x4 yaw_rotation = rotate(v3(0, 1, 0) * player->yaw);
      Matrix4x4 pitch_rotation = rotate(v3(1, 0, 0) * player->pitch);

      Matrix4x4 view = identity();
      view *= pitch_rotation;
      view *= yaw_rotation;
      view *= translate(game_data->player.position);

      float ratio = float(SCREEN_WIDTH) / float(SCREEN_HEIGHT);
      Matrix4x4 projection = perspective_projection(0.1, 1000.0, 45.0, ratio);

      renderWorld(&game_data->loaded_world, terrain_shader, &view, &projection);
    }

    glfwSwapBuffers(window);

    updateInput();
    glfwPollEvents();    
  }

  free(game_data);
  free(block_viewer_data);
  glfwTerminate();
  return 0;
}

