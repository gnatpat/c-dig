
void texture_indices_to_texture_coords(int *textures, Rectangle* texture_coords, int num_textures,
                                       int unit_size, int textures_per_row) {
  // NOTE(nathan): Texture map size is hardcoded for now, will need to change.
  float map_width = unit_size * textures_per_row;
  for(int i = 0; i < num_textures; i++) {
    int index = textures[i];
    float y = ((index / textures_per_row) * unit_size) / map_width;
    float x = ((index % textures_per_row) * unit_size) / map_width;
    texture_coords[i].left = x;
    texture_coords[i].bottom = y + (unit_size / map_width);
    texture_coords[i].top = y;
    texture_coords[i].right = x + (unit_size / map_width);
  }
}


// Slopes faces near Z (+Z) and up (+Y). Textures go slope, far, left, right, bottom.
// NOTE(nathan): The slope is actually rectangular so square texures are a no go. FIXME
GLuint genTexturedSlopeBuffer(int textures[5]) {
  Rectangle texture_coords[5];
  texture_indices_to_texture_coords(textures, texture_coords, 5, 16, 16);

  GLuint vao, vbo, ebo;
  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);
  glGenBuffers(1, &ebo);

  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

  float vertices[] = {
    // SLOPE
    -0.5f, -0.5f,  0.5f, texture_coords[0].left, texture_coords[0].bottom,
     0.5f, -0.5f,  0.5f, texture_coords[0].right, texture_coords[0].bottom,
     0.5f,  0.5f, -0.5f, texture_coords[0].right, texture_coords[0].top,
    -0.5f,  0.5f, -0.5f, texture_coords[0].left, texture_coords[0].top,    

    // FAR
     0.5f, -0.5f, -0.5f, texture_coords[1].right, texture_coords[1].bottom,
    -0.5f, -0.5f, -0.5f, texture_coords[1].left, texture_coords[1].bottom,
    -0.5f,  0.5f, -0.5f, texture_coords[1].left, texture_coords[1].top,
     0.5f,  0.5f, -0.5f, texture_coords[1].right, texture_coords[1].top,

    // LEFT
    -0.5f, -0.5f,  0.5f, texture_coords[2].left, texture_coords[2].bottom,
    -0.5f,  0.5f, -0.5f, texture_coords[2].right, texture_coords[2].top,    
    -0.5f, -0.5f, -0.5f, texture_coords[2].right, texture_coords[2].bottom,

    // RIGHT
     0.5f,  0.5f, -0.5f, texture_coords[3].right, texture_coords[3].top,
     0.5f, -0.5f,  0.5f, texture_coords[3].left, texture_coords[3].bottom,
     0.5f, -0.5f, -0.5f, texture_coords[3].right, texture_coords[3].bottom,    

    // BOTTOM
     0.5f, -0.5f, -0.5f, texture_coords[4].left, texture_coords[4].bottom,
     0.5f, -0.5f,  0.5f, texture_coords[4].right, texture_coords[4].bottom,
    -0.5f, -0.5f,  0.5f, texture_coords[4].right, texture_coords[4].top,
    -0.5f, -0.5f, -0.5f, texture_coords[4].left, texture_coords[4].top,    
  };

  unsigned int indices[] = {
    0, 1, 2,
    0, 2, 3,
    4, 5, 6,
    4, 6, 7,
    8, 9, 10,
    11, 12, 13,
    14, 15, 16,
    14, 16, 17,
  };

  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  return vao;
}

// Slopes faces near Z (+Z), right (+X) and up (+Y). Textures go slope, far, left, bottom.
// NOTE(nathan): Triangular texures? Hmm. FIXME
GLuint genTexturedSlopeCornerBuffer(int textures[4]) {
  Rectangle texture_coords[4];
  texture_indices_to_texture_coords(textures, texture_coords, 4, 16, 16);

  GLuint vao, vbo, ebo;
  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);
  glGenBuffers(1, &ebo);

  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

  float vertices[] = {
    // SLOPE
    -0.5f, -0.5f,  0.5f, texture_coords[0].left, texture_coords[0].bottom,
     0.5f, -0.5f, -0.5f, texture_coords[0].right, texture_coords[0].bottom,
    -0.5f,  0.5f, -0.5f, (texture_coords[0].left + texture_coords[0].right) / 2, texture_coords[0].top,    

    // FAR
    -0.5f, -0.5f, -0.5f, texture_coords[1].left, texture_coords[1].bottom,
    -0.5f,  0.5f, -0.5f, texture_coords[1].left, texture_coords[1].top,
     0.5f, -0.5f, -0.5f, texture_coords[1].right, texture_coords[1].bottom,

    // LEFT
    -0.5f, -0.5f,  0.5f, texture_coords[2].left, texture_coords[2].bottom,
    -0.5f,  0.5f, -0.5f, texture_coords[2].right, texture_coords[2].top,    
    -0.5f, -0.5f, -0.5f, texture_coords[2].right, texture_coords[2].bottom,

    // BOTTOM
    -0.5f, -0.5f, -0.5f, texture_coords[3].right, texture_coords[3].bottom,
     0.5f, -0.5f, -0.5f, texture_coords[3].left, texture_coords[3].bottom,
    -0.5f, -0.5f,  0.5f, texture_coords[3].right, texture_coords[3].top,    
  };

  unsigned int indices[] = {
    0, 1, 2,
    3, 4, 5,
    6, 7, 8,
    9, 10, 11
  };

  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  return vao;
}

GLuint genTexturedGroundedSprite(int index) {
  Rectangle texture_coords[1];
  texture_indices_to_texture_coords(&index, texture_coords, 1, 32, 16);

  GLuint vao, vbo, ebo;
  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);
  glGenBuffers(1, &ebo);

  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

  float vertices[] = {
    -0.5f,  0.0f, 0.0f, texture_coords[0].left, texture_coords[0].bottom,
     0.5f,  0.0f, 0.0f, texture_coords[0].right, texture_coords[0].bottom,
     0.5f,  1.0f, 0.0f, texture_coords[0].right, texture_coords[0].top,
    -0.5f,  1.0f, 0.0f, texture_coords[0].left, texture_coords[0].top,
  };

  unsigned int indices[] = {
    0, 1, 2,
    0, 2, 3,
  };

  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  return vao;
}
