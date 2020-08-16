void initBasicRenderObject(BasicRenderObject* basic_render_object, int max_triangle_count) {
  glGenVertexArrays(1, &basic_render_object->vao);
  glGenBuffers(1, &basic_render_object->vbo);  
  basic_render_object->vertices = (ObjectVertex*) malloc(sizeof(ObjectVertex) * max_triangle_count * 3);
  basic_render_object->triangles = (ObjectTriangle*) malloc(sizeof(ObjectTriangle) * max_triangle_count);
  basic_render_object->triangle_count = 0;
}

void markDirty(BasicRenderObject* basic_render_object) {
  basic_render_object->dirty = true;
}

void blowOutRenderObject(BasicRenderObject* basic_render_object, float distance) {
  for(int i = 0; i < basic_render_object->triangle_count; i++) {
    ObjectTriangle* t = basic_render_object->triangles + i;
    for (int j = 0; j < 3; j++) {
      t->vertices[j] += t->normal * distance;
    }
  }
}

void fillRenderData(BasicRenderObject* basic_render_object) {
  ObjectVertex* cursor = basic_render_object->vertices;
  for (int i = 0; i < basic_render_object->triangle_count; i++) {
    ObjectTriangle t = basic_render_object->triangles[i];
    for (int j = 0; j < 3; j++) {
      cursor->position = t.vertices[j];
      cursor->colour = t.colour;
      cursor->normal = t.normal;
      cursor->alpha = t.alpha;
      cursor++;
    }
  }

  glBindVertexArray(basic_render_object->vao);
  glBindBuffer(GL_ARRAY_BUFFER, basic_render_object->vbo);  
  glBufferData(GL_ARRAY_BUFFER, basic_render_object->triangle_count*3*sizeof(ObjectVertex), basic_render_object->vertices, GL_DYNAMIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ObjectVertex), (void*)0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(ObjectVertex), (void*)(3 * sizeof(float)));
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(ObjectVertex), (void*)(6 * sizeof(float)));
  glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(ObjectVertex), (void*)(9 * sizeof(float)));
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glEnableVertexAttribArray(2);
  glEnableVertexAttribArray(3);
  glBindVertexArray(0);
}

void render(BasicRenderObject* basic_render_object, GLuint shader, Matrix4x4 view, Matrix4x4 projection) {
  render(basic_render_object, shader, identity(), view, projection);
}

void render(BasicRenderObject* basic_render_object, GLuint shader, Matrix4x4 model, Matrix4x4 view, Matrix4x4 projection) {
  if (basic_render_object->dirty) {
    fillRenderData(basic_render_object);
    basic_render_object->dirty = false;
  }
  glUseProgram(shader);
  GLuint location = glGetUniformLocation(shader, "model");
  glUniformMatrix4fv(location, 1, GL_TRUE, (float*)&model);
  location = glGetUniformLocation(shader, "view");
  glUniformMatrix4fv(location, 1, GL_TRUE, (float*)&view);
  location = glGetUniformLocation(shader, "projection");
  glUniformMatrix4fv(location, 1, GL_TRUE, (float*)&projection);

  glBindVertexArray(basic_render_object->vao);
  glDrawArrays(GL_TRIANGLES, 0, basic_render_object->triangle_count*3);
}

void make3dTriangle(ObjectTriangle* buffer, V3 direction, V3 perp_dir, V3 colour, float offset_fraction) {
  V3 offset = direction * offset_fraction;
  V3 tip = direction + offset;
  V3 base_a = perp_dir;
  V3 base_b = cross(direction, perp_dir);

  buffer->vertices[0] = tip;
  buffer->vertices[1] = base_a + offset;
  buffer->vertices[2] = -base_a + offset;
  buffer->colour = colour;
  buffer->alpha = 1.0f;
  buffer++;

  buffer->vertices[0] = tip;
  buffer->vertices[1] = -base_a + offset;
  buffer->vertices[2] = base_a + offset;
  buffer->colour = colour;
  buffer->alpha = 1.0f;
  buffer++;

  buffer->vertices[0] = tip;
  buffer->vertices[1] = -base_b + offset;
  buffer->vertices[2] = base_b + offset;
  buffer->colour = colour;
  buffer->alpha = 1.0f;
  buffer++;

  buffer->vertices[0] = tip;
  buffer->vertices[1] = base_b + offset;
  buffer->vertices[2] = -base_b + offset;
  buffer->colour = colour;
  buffer->alpha = 1.0f;
  buffer++;
}

