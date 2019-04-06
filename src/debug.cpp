void initDebugTriangles(DebugTriangles* debug_triangles, int size) {
  glGenVertexArrays(1, &debug_triangles->vao);
  glGenBuffers(1, &debug_triangles->vbo);  
  debug_triangles->triangles = (Triangle*) malloc(sizeof(Triangle) * size);
  debug_triangles->vertices = (V3*) malloc(sizeof(V3) * size * 3);
  debug_triangles->triangle_count = 0;
}

void initDebugData(DebugData* debug_data) {
  initDebugTriangles(&debug_data->mesh_around_player, 1000);
}

void debugMeshAroundPlayer(DebugTriangles* debug_triangles, GameData* game_data) {
  debug_triangles->triangle_count = getMeshAroundPosition(debug_triangles->triangles,
                                                          &game_data->loaded_world,
                                                          toV3i(game_data->player.position)-v3i(1, 1, 1),
                                                          toV3i(game_data->player.position)+v3i(1, 2, 1));
  fillDebugTriangleRenderData(debug_triangles);
}

void fillDebugTriangleRenderData(DebugTriangles* debug_triangles) {
  V3* cursor = debug_triangles->vertices;
  for(int i = 0; i < debug_triangles->triangle_count; i++) {
    Triangle* triangle = debug_triangles->triangles + i;
    V3 normal_offset = triangle->normal * 0.001;
    *cursor = triangle->vertices[0] + normal_offset;
    *(cursor+1) = triangle->vertices[1] + normal_offset;
    *(cursor+2) = triangle->vertices[2] + normal_offset;
    cursor += 3;
  }
  glBindVertexArray(debug_triangles->vao);
  glBindBuffer(GL_ARRAY_BUFFER, debug_triangles->vbo);  
  glBufferData(GL_ARRAY_BUFFER, debug_triangles->triangle_count*3*sizeof(V3), debug_triangles->vertices, GL_DYNAMIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
  glEnableVertexAttribArray(0);
  glBindVertexArray(0);
}

void renderDebug(DebugData* debug_data, GLuint shader, Matrix4x4 view, Matrix4x4 projection) {
  renderDebugTriangles(&debug_data->mesh_around_player, shader, view, projection, WHITE);
}

void renderDebugTriangles(DebugTriangles* debug_triangles, GLuint shader, Matrix4x4 view, Matrix4x4 projection, V3 col) {
  glUseProgram(shader);
  Matrix4x4 model = identity();
  GLuint location = glGetUniformLocation(shader, "model");
  glUniformMatrix4fv(location, 1, GL_TRUE, (float*)&model);
  location = glGetUniformLocation(shader, "view");
  glUniformMatrix4fv(location, 1, GL_TRUE, (float*)&view);
  location = glGetUniformLocation(shader, "projection");
  glUniformMatrix4fv(location, 1, GL_TRUE, (float*)&projection);
  location = glGetUniformLocation(shader, "inCol");
  glUniform3fv(location, 1, (float*)&col);

  glBindVertexArray(debug_triangles->vao);
  glDrawArrays(GL_TRIANGLES, 0, debug_triangles->triangle_count*3);
}
