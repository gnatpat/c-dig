struct TextVertex {
  V2 position;
  V2 uv;
};

struct CharacterQuad {
  TextVertex vertices[4];
};

Text createText(const char* string, Font* font) {
  int length = strlen(string);
  CharacterQuad* vertices = (CharacterQuad*)malloc(sizeof(CharacterQuad) * length);
  unsigned int* indices = (unsigned int*) malloc(sizeof(unsigned int) * length * 6);

  FontData* data = &font->data;

  float cursor_x = 0.0;
  const char* current_char = string;
  CharacterQuad* current_quad = vertices;
  int i = 0;
  float ratio = data->line_height;
  while(*current_char) {
    FontCharData char_data = data->chars[(int)*current_char];
    const char* next_char = current_char + 1;
    int kerning = data->kerning[(int)*current_char][(int)*next_char];
    V2 cursor_pos = v2(cursor_x + char_data.x_offset, -char_data.y_offset);
    V2 top_left_uv = v2(char_data.x, char_data.y);
    V2 uv_ratio = v2(data->texture_width, data->texture_height);

    current_quad->vertices[0].position = (cursor_pos + v2(0, 0)) / ratio;
    current_quad->vertices[1].position = (cursor_pos + v2(char_data.width, 0))/ratio;
    current_quad->vertices[2].position = (cursor_pos + v2(0, -char_data.height))/ratio;
    current_quad->vertices[3].position = (cursor_pos + v2(char_data.width, -char_data.height))/ratio;

    current_quad->vertices[0].uv = (top_left_uv)/uv_ratio;
    current_quad->vertices[1].uv = (top_left_uv + v2(char_data.width, 0))/uv_ratio;
    current_quad->vertices[2].uv = (top_left_uv + v2(0, char_data.height))/uv_ratio;
    current_quad->vertices[3].uv = (top_left_uv + v2(char_data.width, char_data.height))/uv_ratio;

    indices[i*6 + 0] = i * 4;
    indices[i*6 + 1] = i * 4 + 2;
    indices[i*6 + 2] = i * 4 + 1;
    indices[i*6 + 3] = i * 4 + 2;
    indices[i*6 + 4] = i * 4 + 3;
    indices[i*6 + 5] = i * 4 + 1;

    printf("char: %c int: %d x: %d, x_advance: %d height: %d pos: (%.2f, %.2f)\n", *current_char, (int)*current_char, char_data.x, char_data.x_advance, char_data.height, current_quad->vertices[1].position.x, current_quad->vertices[1].position.y);
    current_char++;
    cursor_x += char_data.x_advance + kerning - kerning;
    current_quad++;
    i++;
  }

  GLuint vao, vbo, ebo;
  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);
  glGenBuffers(1, &ebo);
  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, length * sizeof(CharacterQuad), vertices, GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, length * sizeof(unsigned int) * 6, indices, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(TextVertex), (void*)0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(TextVertex), (void*)(2 * sizeof(float)));
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);

  glBindVertexArray(0);
  glDeleteBuffers(1, &vbo);
  glDeleteBuffers(1, &ebo);

  Text text;
  text.vao = vao;
  text.length = length;
  text.font = font;
  return text;
}

void renderText(const Text text, const V2 pos, const float height, const GLuint shader) {
  float aspect_ratio = ((float)SCREEN_HEIGHT)/SCREEN_WIDTH;
  float font_scale = height;
  Matrix4x4 model = identity();
  model *= translate(v3(pos.x, pos.y, 0.0));
  model *= scale(v3(font_scale, font_scale / aspect_ratio, 1.0));
  
  glUseProgram(shader);
  GLuint model_location = glGetUniformLocation(shader, "model");
  glUniformMatrix4fv(model_location, 1, GL_TRUE, (float*)&model);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, text.font->texture);
  glBindVertexArray(text.vao);

  glDisable(GL_DEPTH_TEST);
  glDrawElements(
      GL_TRIANGLES,
      text.length * 6,
      GL_UNSIGNED_INT,
      (void*)0
  );
  glEnable(GL_DEPTH_TEST);
  glBindVertexArray(0);
}

