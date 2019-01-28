#include "main.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "chunk.cpp"
#include "models.cpp"
#include "opengl.cpp"
#include "shaders.cpp"
#include "textures.cpp"
#include "utils.cpp"

void initChunk(Chunk* c) {
  for (int x = 0; x < CHUNK_SIZE; x++) {
    for (int y = 0; y < CHUNK_SIZE; y++) {
      for (int z = 0; z < CHUNK_SIZE; z++) {
        c->blocks[x][y][z].block_shape = AIR;
      }
    }
  }
  c->blocks[1][2][1].block_shape = CUBE;
  c->blocks[0][2][1].block_shape = NEG_X_NEG_Y_SLOPE;
  c->blocks[2][2][1].block_shape = POS_X_NEG_Y_SLOPE;
  c->blocks[1][2][0].block_shape = NEG_Z_NEG_Y_SLOPE;
  c->blocks[1][2][2].block_shape = POS_Z_NEG_Y_SLOPE;
  c->blocks[2][2][2].block_shape = NEG_NEG_NEG_CORNER;
  c->blocks[0][2][2].block_shape = POS_NEG_NEG_CORNER;
  c->blocks[0][2][0].block_shape = POS_NEG_POS_CORNER;
  c->blocks[2][2][0].block_shape = NEG_NEG_POS_CORNER;

  c->blocks[0][1][1].block_shape = CUBE;
  c->blocks[2][1][1].block_shape = CUBE;
  c->blocks[1][1][0].block_shape = CUBE;
  c->blocks[1][1][2].block_shape = CUBE;
  c->blocks[1][1][1].block_shape = CUBE;
  c->blocks[0][1][0].block_shape = POS_POS_DIAGONAL;
  c->blocks[2][1][0].block_shape = NEG_POS_DIAGONAL;
  c->blocks[2][1][2].block_shape = NEG_NEG_DIAGONAL;
  c->blocks[0][1][2].block_shape = POS_NEG_DIAGONAL;

  c->blocks[1][0][1].block_shape = CUBE;
  c->blocks[0][0][1].block_shape = NEG_X_POS_Y_SLOPE;
  c->blocks[2][0][1].block_shape = POS_X_POS_Y_SLOPE;
  c->blocks[1][0][0].block_shape = NEG_Z_POS_Y_SLOPE;
  c->blocks[1][0][2].block_shape = POS_Z_POS_Y_SLOPE;
  c->blocks[2][0][2].block_shape = NEG_POS_NEG_CORNER;
  c->blocks[0][0][2].block_shape = POS_POS_NEG_CORNER;
  c->blocks[0][0][0].block_shape = POS_POS_POS_CORNER;
  c->blocks[2][0][0].block_shape = NEG_POS_POS_CORNER;

  fillChunkRenderData(c);
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

  Sprite sprites[] = { { { 0.5, CHUNK_SIZE/2.0, -0.5 }, 0 },
                       { { 4.5, CHUNK_SIZE/2.0, 7.5 }, 0 },
                       { { -6.5, CHUNK_SIZE/2.0, -2.5 }, 0 } };


  GameData* game_data = (GameData*) malloc(sizeof(GameData));
  initChunk(&game_data->chunk);

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
    if (acc >= 2.0) {
      printf("fps: %2.2f\n", frames/acc);
      acc = 0;
      frames = 0;
    }

    t += delta;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float y_rot = M_PI / 8 * t;

    // Chunk
    Matrix4x4 model = identity();
    model *= translate(v3(-(CHUNK_SIZE)/2.0, -(CHUNK_SIZE)/2.0, -(CHUNK_SIZE)/2.0));
    Matrix4x4 view = identity();
    view *= translate(v3(0.0, 0.0, -CHUNK_SIZE - 5.0));
    view *= rotate(v3(1.0, 0.0, 0.0) * M_PI / 4);
    view *= rotate(v3(0.0, 1.0, 0.0) * y_rot);
    float ratio = float(SCREEN_WIDTH) / float(SCREEN_HEIGHT);
    Matrix4x4 projection = perspective_projection(0.1, 100.0, 45.0, ratio);

    glUseProgram(shader_program_no_texture);
    GLuint transformLocation = glGetUniformLocation(shader_program, "view");
    glUniformMatrix4fv(transformLocation, 1, GL_TRUE, (float*)&view);
    transformLocation = glGetUniformLocation(shader_program, "projection");
    glUniformMatrix4fv(transformLocation, 1, GL_TRUE, (float*)&projection);
    transformLocation = glGetUniformLocation(shader_program, "model");
    glUniformMatrix4fv(transformLocation, 1, GL_TRUE, (float*)&model);

    ChunkRenderData* render_data = &(game_data->chunk.render_data);
    glBindVertexArray(render_data->vao);
    glDrawArrays(GL_TRIANGLES, 0, render_data->num_vertices);

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
      model = identity();
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
