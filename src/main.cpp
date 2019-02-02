#include "main.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "chunk.cpp"
#include "models.cpp"
#include "opengl.cpp"
#include "shaders.cpp"
#include "textures.cpp"
#include "utils.cpp"
#include "world.cpp"

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

int main(void) {
  GLFWwindow* window;
  bool success = initOpenGLAndCreateWindow(&window);
  if (!success) {
    return -1;
  }

  GLuint shader_program = compileShaderOrDie("resources/shaders/shader.vs", "resources/shaders/shader.fs");
  GLuint shader_program_no_texture = compileShaderOrDie("resources/shaders/shader_no_texture.vs",
                                                        "resources/shaders/shader_no_texture.fs");

  GLuint sprite_textures = loadTexture("resources/sprites.png");
  GLuint sprite_vaos[] = { genTexturedGroundedSprite(0) };

  Sprite sprites[] = { { { 1.0, 3.0, 1.0 }, 0 } };

  GameData* game_data = (GameData*) malloc(sizeof(GameData));
  initWorld(&game_data->loaded_world);

  pthread_t chunk_render_thread;
  pthread_create(&chunk_render_thread, NULL, newChunkRenderMain, game_data);

  float t = 0.0;

  int frames = 0;
  float acc = 0;

  struct timespec old_time, new_time;
  clock_gettime(CLOCK_MONOTONIC_RAW, &old_time);
  while(!glfwWindowShouldClose(window))
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

    // RENDER
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float y_rot = M_PI / 8 * t;

    // Chunk
    Matrix4x4 view = identity();
    view *= translate(v3(0.0, 0.0, -CHUNK_SIZE - 5.0));
    view *= rotate(v3(1.0, 0.0, 0.0) * M_PI / 4);
    view *= rotate(v3(0.0, 1.0, 0.0) * y_rot);
    view *= translate(v3(0, -2 * CHUNK_SIZE, 0));
    float ratio = float(SCREEN_WIDTH) / float(SCREEN_HEIGHT);
    Matrix4x4 projection = perspective_projection(0.1, 100.0, 45.0, ratio);

    glUseProgram(shader_program_no_texture);
    GLuint transformLocation = glGetUniformLocation(shader_program, "view");
    glUniformMatrix4fv(transformLocation, 1, GL_TRUE, (float*)&view);
    transformLocation = glGetUniformLocation(shader_program, "projection");
    glUniformMatrix4fv(transformLocation, 1, GL_TRUE, (float*)&projection);

    for(int x = 0; x < LOADED_WORLD_SIZE; x++) {
      for(int y = 0; y < LOADED_WORLD_SIZE; y++) {
        for(int z = 0; z < LOADED_WORLD_SIZE; z++) {
          Chunk* c = getChunkAt(&game_data->loaded_world, v3i(x, y, z));
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
              transformLocation = glGetUniformLocation(shader_program, "model");
              glUniformMatrix4fv(transformLocation, 1, GL_TRUE, (float*)&model);
              ChunkRenderData* render_data = &c->render_data;
              glBindVertexArray(render_data->vao);
              glDrawArrays(GL_TRIANGLES, 0, render_data->num_vertices);
              break;
          }
        }
      }
    }

    // Sprites
    glUseProgram(shader_program);
    transformLocation = glGetUniformLocation(shader_program, "view");
    glUniformMatrix4fv(transformLocation, 1, GL_TRUE, (float*)&view);
    transformLocation = glGetUniformLocation(shader_program, "projection");
    glUniformMatrix4fv(transformLocation, 1, GL_TRUE, (float*)&projection);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, sprite_textures);
    for (unsigned int i = 0; i < (sizeof(sprites) / sizeof(Sprite)); i++) {
      Sprite s = sprites[i];
      Matrix4x4 model = identity();
      model *= translate(s.position);
      model *= rotate(v3(0, 1, 0) * -y_rot);
      transformLocation = glGetUniformLocation(shader_program, "model");
      glUniformMatrix4fv(transformLocation, 1, GL_TRUE, (float*)&model);
      glBindVertexArray(sprite_vaos[s.index]);
      glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }

    glfwSwapBuffers(window);
    glfwPollEvents();    
  }

  free(game_data);
  glfwTerminate();
  return 0;
}
