#include "main.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "models.cpp"
#include "opengl.cpp"
#include "shaders.cpp"
#include "textures.cpp"
#include "utils.cpp"

int rotationFromOcclusion(bool* occlusion) {
  if(occlusion[0]) return 5;
  for (int i = 0; i < 4; i++) {
    if (occlusion[i]) return i + 1;
  }
  return -1;
}

float DISTANCE = 1.5;
V3 BLACK = v3(0, 0, 0);
ChunkRenderData sideOcclusionBuffer(BlockShape block_shape) {
  V3 rotations[6] = {v3(0, 0, 0), v3(0, M_PI/2, 0), v3(0, M_PI, 0), v3(0, -M_PI/2, 0), v3(M_PI/2, 0, 0), v3(-M_PI/2, 0, 0)};
  ChunkVertex vertices[36];
  ChunkVertex* vertex_cursor = vertices;
  V3 vs[4] = { v3(1, 0, 0), v3(0, 0, 0), v3(1, 1, 0), v3(0, 1, 0) };
  for(int i = 0; i < 6; i++) { 
    bool* occlusion = BLOCK_SIDE_OCCLUSION_BITFIELD + (block_shape * 30 + i * 5);
    int rotation = rotationFromOcclusion(occlusion);
    if (rotation == -1) {
      continue;
    }
    Matrix4x4 m = identity();
    m *= translate(v3(0.5, 0.5, 0.5));
    m *= rotate(rotations[i]);
    m *= translate(v3(0, 0, -DISTANCE));
    m *= rotate(v3(0, 0, 1) * M_PI/2.0 * rotation);
    m *= translate(v3(-0.5, -0.5, 0));
    V3 rotated_vs[4];
    memcpy(rotated_vs, vs, sizeof(V3) * 4);
    for (int j = 0; j < 4; j++) { 
      rotated_vs[j] = m * rotated_vs[j];
    }
    if (rotation == 5) {
      *vertex_cursor = { rotated_vs[0], BLACK };
      vertex_cursor++;
      *vertex_cursor = { rotated_vs[1], BLACK };
      vertex_cursor++;
      *vertex_cursor = { rotated_vs[2], BLACK };
      vertex_cursor++;
      *vertex_cursor = { rotated_vs[2], BLACK };
      vertex_cursor++;
      *vertex_cursor = { rotated_vs[1], BLACK };
      vertex_cursor++;
      *vertex_cursor = { rotated_vs[3], BLACK };
      vertex_cursor++;
      continue;
    }
    for (int k = 0; k < 3; k++) {
      *vertex_cursor = { rotated_vs[k], BLACK };
      vertex_cursor++;
    }
  }

  ptrdiff_t vertex_count = vertex_cursor - vertices;
  GLuint vao, vbo;
  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);
  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, vertex_count * sizeof(ChunkVertex), vertices, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ChunkVertex), (void*)0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(ChunkVertex), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);

  ChunkRenderData render_data;
  render_data.vao = vao;
  render_data.num_vertices = vertex_count;

  return render_data;
}

int main(void) {
  GLFWwindow* window;
  bool success = initOpenGLAndCreateWindow(&window);
  if (!success) {
    return -1;
  }

  GLuint shader_program_no_texture = compileShaderOrDie("resources/shaders/shader_no_texture.vs",
                                                        "resources/shaders/shader_no_texture.fs");

  Chunk c;
  BlockShape shape = CUBE;
  c.blocks[0][0][0].block_shape = shape;
  fillChunkRenderData(&c);
  ChunkRenderData rd = sideOcclusionBuffer(shape);
  bool down = false;

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

    bool left = glfwGetKey(window, GLFW_KEY_LEFT);
    bool right = glfwGetKey(window, GLFW_KEY_RIGHT);
    if ((left || right) && !down) {
      int diff;
      if (left) diff = -1;
      if (right) diff = 1;
      shape = (BlockShape)(((int) shape + diff + BLOCK_SHAPE_COUNT) % BLOCK_SHAPE_COUNT);
      c.blocks[0][0][0].block_shape = shape;
      fillChunkRenderData(&c);
      rd = sideOcclusionBuffer(shape);
    }
    down = left || right;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float y_rot = M_PI / 8 * t;

    // Chunk
    Matrix4x4 model = identity();
    model *= translate(v3(-(CHUNK_SIZE)/2.0, -(CHUNK_SIZE)/2.0, -(CHUNK_SIZE)/2.0));
    Matrix4x4 view = identity();
    view *= translate(v3(0.0, 0.0, -CHUNK_SIZE - 5.0));
    view *= rotate(v3(1.0, 0.0, 0.0) * M_PI / 4);
    view *= rotate(v3(1.0, 1.0, 0.0) * y_rot);
    float ratio = float(SCREEN_WIDTH) / float(SCREEN_HEIGHT);
    Matrix4x4 projection = perspective_projection(0.1, 100.0, 45.0, ratio);

    glUseProgram(shader_program_no_texture);
    GLuint transformLocation = glGetUniformLocation(shader_program_no_texture, "view");
    glUniformMatrix4fv(transformLocation, 1, GL_TRUE, (float*)&view);
    transformLocation = glGetUniformLocation(shader_program_no_texture, "projection");
    glUniformMatrix4fv(transformLocation, 1, GL_TRUE, (float*)&projection);
    transformLocation = glGetUniformLocation(shader_program_no_texture, "model");
    glUniformMatrix4fv(transformLocation, 1, GL_TRUE, (float*)&model);

    ChunkRenderData* render_data = &(c.render_data);
    glBindVertexArray(render_data->vao);
    glDrawArrays(GL_TRIANGLES, 0, render_data->num_vertices);

    if (!glfwGetKey(window, GLFW_KEY_SPACE)) {
      glBindVertexArray(rd.vao);
      glDrawArrays(GL_TRIANGLES, 0, rd.num_vertices);
    }

    glfwSwapBuffers(window);
    glfwPollEvents();    
  }

  glfwTerminate();
  return 0;
}
