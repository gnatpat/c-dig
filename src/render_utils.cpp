void initBasicRenderObject(BasicRenderObject* basic_render_object, int size) {
  glGenVertexArrays(1, &basic_render_object->vao);
  glGenBuffers(1, &basic_render_object->vbo);  
  basic_render_object->triangles = (Triangle*) malloc(sizeof(Triangle) * size);
  basic_render_object->vertices = (V3*) malloc(sizeof(V3) * size * 3);
  basic_render_object->triangle_count = 0;
}

void markDirty(BasicRenderObject* basic_render_object) {
  basic_render_object->dirty = true;
}

void blowOutRenderObject(BasicRenderObject* basic_render_object, float distance) {
  for(int i = 0; i < basic_render_object->triangle_count; i++) {
    Triangle* triangle = basic_render_object->triangles + i;
    V3 normal_offset = triangle->normal * distance;
    *triangle = shiftTriangle(*triangle, normal_offset);
  }
}

void fillRenderData(BasicRenderObject* basic_render_object) {
  V3* cursor = basic_render_object->vertices;
  for(int i = 0; i < basic_render_object->triangle_count; i++) {
    Triangle* triangle = basic_render_object->triangles + i;
    *cursor = triangle->vertices[0];
    *(cursor+1) = triangle->vertices[1];
    *(cursor+2) = triangle->vertices[2];
    cursor += 3;
  }
  glBindVertexArray(basic_render_object->vao);
  glBindBuffer(GL_ARRAY_BUFFER, basic_render_object->vbo);  
  glBufferData(GL_ARRAY_BUFFER, basic_render_object->triangle_count*3*sizeof(V3), basic_render_object->vertices, GL_DYNAMIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
  glEnableVertexAttribArray(0);
  glBindVertexArray(0);
}

void render(BasicRenderObject* basic_render_object, GLuint shader, Matrix4x4 view, Matrix4x4 projection, V3 col) {
  if (basic_render_object->dirty) {
    fillRenderData(basic_render_object);
    basic_render_object->dirty = false;
  }
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

  glBindVertexArray(basic_render_object->vao);
  glDrawArrays(GL_TRIANGLES, 0, basic_render_object->triangle_count*3);
}

