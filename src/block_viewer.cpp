int rotationFromOcclusion(bool* occlusion) {
  if(occlusion[0]) return 5;
  for (int i = 0; i < 4; i++) {
    if (occlusion[i+1]) return i;
  }
  return -1;
}

float DISTANCE = 1.5;
void sideOcclusionBuffer(BlockShape block_shape, ChunkRenderData* render_data) {
  V3 rotations[6] = {v3(0, M_PI, 0), v3(0, M_PI/2, 0), v3(0, 0, 0), v3(0, -M_PI/2, 0), v3(M_PI/2, 0, 0), v3(-M_PI/2, 0, 0)};
  V3 colours[6] = {v3(0, 0, 1), v3(1, 0, 0), v3(1, 1, 0), v3(0, 1, 1), v3(0, 1, 0), v3(1, 0, 1)};
  ChunkVertex vertices[72];
  ChunkVertex* vertex_cursor = vertices;
  V3 vs[4] = { v3(1, 0, 0), v3(1, 1, 0), v3(0, 1, 0), v3(0, 0, 0) };
  for(int i = 0; i < 6; i++) { 
    bool* occlusion = getBlockSideOcclusion(block_shape, (Direction)i);
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
    for (int k = 0; k < 3; k++) {
      *vertex_cursor = { rotated_vs[k], colours[i] };
      vertex_cursor++;
    }
    for (int k = 0; k < 3; k++) {
      *vertex_cursor = { rotated_vs[2-k], colours[i] };
      vertex_cursor++;
    }
    if (rotation == 5) {
      *vertex_cursor = { rotated_vs[3], colours[i] };
      vertex_cursor++;
      *vertex_cursor = { rotated_vs[0], colours[i] };
      vertex_cursor++;
      *vertex_cursor = { rotated_vs[2], colours[i] };
      vertex_cursor++;
      *vertex_cursor = { rotated_vs[3], colours[i] };
      vertex_cursor++;
      *vertex_cursor = { rotated_vs[2], colours[i] };
      vertex_cursor++;
      *vertex_cursor = { rotated_vs[0], colours[i] };
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

  render_data->vao = vao;
  render_data->num_vertices = vertex_count;
}


void updateForNewShape(BlockViewerData* block_viewer_data, BlockShape block_shape) {
  block_viewer_data->block_shape = block_shape;
  Chunk* c = block_viewer_data->c;
  c->blocks[1][1][1].block_shape = block_shape;
  fillChunkRenderData(c, &block_viewer_data->empty_world);
  fillChunkVao(c);
  sideOcclusionBuffer(block_shape, &block_viewer_data->occlusion_render_data);
}


void updateBlockViewer(BlockViewerData* block_viewer_data, bool* down, bool* block_viewer_mode) {
  bool left = glfwGetKey(WINDOW, GLFW_KEY_LEFT);
  bool right = glfwGetKey(WINDOW, GLFW_KEY_RIGHT);
  bool mode_swap = glfwGetKey(WINDOW, GLFW_KEY_B);
  if (!*down) {
    if (left || right) {
      int diff;
      if (left) diff = -1;
      if (right) diff = 1;
      BlockShape shape = (BlockShape)(((int) block_viewer_data->block_shape + diff + BLOCK_SHAPE_COUNT) % BLOCK_SHAPE_COUNT);
      updateForNewShape(block_viewer_data, shape);
    } else if (mode_swap) {
      *block_viewer_mode = false;
    }
  }
  *down = left || right || mode_swap;
}

void renderBlockViewer(BlockViewerData* block_viewer_data, float t, GLuint terrain_shader_program) {
  float y_rot = M_PI / 8 * t;
  Matrix4x4 model = identity();
  model *= translate(v3(-1.0, -1.0, -1.0));
  Matrix4x4 view = identity();
  view *= translate(v3(0.0, 0.0, -6.0));
  view *= rotate(v3(1.0, 0.0, 0.0) * M_PI / 4);
  view *= rotate(v3(0.0, 1.0, 0.0) * y_rot);
  float ratio = float(SCREEN_WIDTH) / float(SCREEN_HEIGHT);
  Matrix4x4 projection = perspective_projection(0.1, 100.0, 45.0, ratio);

  glUseProgram(terrain_shader_program);
  GLuint transformLocation = glGetUniformLocation(terrain_shader_program, "view");
  glUniformMatrix4fv(transformLocation, 1, GL_TRUE, (float*)&view);
  transformLocation = glGetUniformLocation(terrain_shader_program, "projection");
  glUniformMatrix4fv(transformLocation, 1, GL_TRUE, (float*)&projection);
  transformLocation = glGetUniformLocation(terrain_shader_program, "model");
  glUniformMatrix4fv(transformLocation, 1, GL_TRUE, (float*)&model);

  ChunkRenderData* render_data = &(block_viewer_data->c->render_data);
  glBindVertexArray(render_data->vao);
  glDrawArrays(GL_TRIANGLES, 0, render_data->num_vertices);

  model = identity();
  transformLocation = glGetUniformLocation(terrain_shader_program, "model");
  glUniformMatrix4fv(transformLocation, 1, GL_TRUE, (float*)&model);

  if (!glfwGetKey(WINDOW, GLFW_KEY_SPACE)) {
    ChunkRenderData* rd = &(block_viewer_data->occlusion_render_data);
    glBindVertexArray(rd->vao);
    glDrawArrays(GL_TRIANGLES, 0, rd->num_vertices);
  }
}


void initBlockViewer(BlockViewerData* block_viewer_data) {
  LoadedWorld* empty_world = &block_viewer_data->empty_world;
  initWorld(empty_world);
  for (int x = 0; x < LOADED_WORLD_SIZE; x++) {
    for (int y = 0; y < LOADED_WORLD_SIZE; y++) {
      for (int z = 0; z < LOADED_WORLD_SIZE; z++) { 
        clearChunk(&empty_world->chunks[x][y][z]);
      }
    }
  }
  block_viewer_data->c = &empty_world->chunks[0][0][0];
  updateForNewShape(block_viewer_data, CUBE);
}
