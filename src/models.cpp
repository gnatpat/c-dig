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

void fillTriangleFace(Face* faces, Direction direction, V3 top_right_vertices_as_v3[3], V3 normal) {
  Quaternion q = from_vector_and_angle(normal, M_PI/2);
  V3* vs = top_right_vertices_as_v3;
  faces[TOP_RIGHT].type = TRIANGLE;
  V3 top_right_vertices[] = { vs[0], vs[1], vs[2] };
  memcpy(&faces[TOP_RIGHT].vertices[direction], top_right_vertices, sizeof(V3) * 3);
  vs[0] = rotate(q, vs[0]);
  vs[1] = rotate(q, vs[1]);
  vs[2] = rotate(q, vs[2]);
  faces[TOP_LEFT].type = TRIANGLE;
  V3 top_left_vertices[] = { vs[0], vs[1], vs[2] };
  memcpy(&faces[TOP_LEFT].vertices[direction], top_left_vertices, sizeof(V3) * 3);

  vs[0] = rotate(q, vs[0]);
  vs[1] = rotate(q, vs[1]);
  vs[2] = rotate(q, vs[2]);
  faces[BOTTOM_LEFT].type = TRIANGLE;
  V3 bottom_left_vertices[] = { vs[0], vs[1], vs[2] };
  memcpy(&faces[BOTTOM_LEFT].vertices[direction], bottom_left_vertices, sizeof(V3) * 3);

  vs[0] = rotate(q, vs[0]);
  vs[1] = rotate(q, vs[1]);
  vs[2] = rotate(q, vs[2]);
  faces[BOTTOM_RIGHT].type = TRIANGLE;
  V3 bottom_right_vertices[] = { vs[0], vs[1], vs[2] };
  memcpy(&faces[BOTTOM_RIGHT].vertices[direction], bottom_right_vertices, sizeof(V3) * 3);
}

void initFaces(Faces* faces) {
  faces->empty_face.type = EMPTY;

  // NOTE(nathan): CBA to refactor this - faces now store V3 not float, but memcpy will still work
  faces->square_face.type = SQUARE;
  float neg_z_vertices[] = {
    -0.5f,  0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
  };
  memcpy(&faces->square_face.vertices[NEG_Z], neg_z_vertices, sizeof(float) * 12);

  float pos_z_vertices[] = {
    -0.5f, -0.5f,  0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,
  };
  memcpy(&faces->square_face.vertices[POS_Z], pos_z_vertices, sizeof(float) * 12);

  float neg_x_vertices[] = {
    -0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f,  0.5f,
  };
  memcpy(&faces->square_face.vertices[NEG_X], neg_x_vertices, sizeof(float) * 12);

  float pos_x_vertices[] = {
     0.5f,  0.5f, -0.5f,
     0.5f,  0.5f,  0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f, -0.5f, -0.5f,
  };
  memcpy(&faces->square_face.vertices[POS_X], pos_x_vertices, sizeof(float) * 12);

  float neg_y_vertices[] = {
     0.5f, -0.5f, -0.5f,
     0.5f, -0.5f,  0.5f,
    -0.5f, -0.5f,  0.5f,
    -0.5f, -0.5f, -0.5f,
  };
  memcpy(&faces->square_face.vertices[NEG_Y], neg_y_vertices, sizeof(float) * 12);

  float pos_y_vertices[] = {
     0.5f,  0.5f,  0.5f,
     0.5f,  0.5f, -0.5f,
    -0.5f,  0.5f, -0.5f,
    -0.5f,  0.5f,  0.5f,
  };
  memcpy(&faces->square_face.vertices[POS_Y], pos_y_vertices, sizeof(float) * 12);

  V3 neg_z_traingle[3] = { v3(-0.5f,  0.5f, -0.5f), v3( 0.5f,  0.5f, -0.5f), v3(-0.5f, -0.5f, -0.5f) };
  fillTriangleFace(faces->triangle_faces, NEG_Z, neg_z_traingle, v3(0.0, 0.0, -1.0));
  V3 pos_z_traingle[3] = { v3( 0.5f,  0.5f,  0.5f), v3(-0.5f,  0.5f,  0.5f), v3( 0.5f, -0.5f,  0.5f) };
  fillTriangleFace(faces->triangle_faces, POS_Z, pos_z_traingle, v3(0.0, 0.0, 1.0));

  V3 neg_x_traingle[3] = { v3(-0.5f, 0.5f,  0.5f), v3(-0.5f,  0.5f, -0.5f), v3(-0.5f, -0.5f,  0.5f) };
  fillTriangleFace(faces->triangle_faces, NEG_X, neg_x_traingle, v3(-1.0, 0.0, 0.0));
  V3 pos_x_traingle[3] = { v3( 0.5f, 0.5f, -0.5f), v3( 0.5f,  0.5f,  0.5f), v3( 0.5f, -0.5f, -0.5f) };
  fillTriangleFace(faces->triangle_faces, POS_X, pos_x_traingle, v3(1.0, 0.0, 0.0));

  V3 neg_y_traingle[3] = { v3(-0.5f,  0.5f,  0.5f), v3( 0.5f,  0.5f, -0.5f), v3( 0.5f,  0.5f,  0.5f) };
  fillTriangleFace(faces->triangle_faces, NEG_Y, neg_y_traingle, v3(0.0, -1.0, 0.0));
  V3 pos_y_traingle[3] = { v3(-0.5f, -0.5f, -0.5f), v3( 0.5f, -0.5f,  0.5f), v3( 0.5f, -0.5f, -0.5f) };
  fillTriangleFace(faces->triangle_faces, POS_Y, pos_y_traingle, v3(0.0, 1.0, 0.0));
}

void fillBlockFlyweights(BlockShapeFlyweights* block_shape_flyweights, Faces* faces) {
  BlockShapeFlyweight empty;
  empty.block_type = AIR;
  for (int d = 0; d < DIRECTION_COUNT; d++) {
    empty.side_faces[d] = &faces->empty_face;
  }
  block_shape_flyweights->empty = empty;

  BlockShapeFlyweight cube;
  cube.block_type = CUBE;
  for (int d = 0; d < DIRECTION_COUNT; d++) {
    cube.side_faces[d] = &faces->square_face;
  }
  block_shape_flyweights->cube = cube;

  // Because I'm lazy we'll reuse this slope object and then copy it into the flyweights struct
  BlockShapeFlyweight slope;
  slope.block_type = SLOPE;
  slope.side_faces[NEG_Y] = &faces->square_face;
  slope.side_faces[POS_Y] = &faces->empty_face;

  Direction xz_sides[] = {POS_X, NEG_Z, NEG_X, POS_Z, POS_X, NEG_Z, NEG_X, POS_Z};
  XZDirection xz_sides_as_xz[] = {XZ_POS_X, XZ_NEG_Z, XZ_NEG_X, XZ_POS_Z};

  for (int i = 0; i < 4; i++) {
    slope.side_faces[xz_sides[i]] = &faces->empty_face;
    slope.side_faces[xz_sides[i+1]] = &faces->triangle_faces[BOTTOM_RIGHT];
    slope.side_faces[xz_sides[i+2]] = &faces->square_face;
    slope.side_faces[xz_sides[i+3]] = &faces->triangle_faces[BOTTOM_LEFT];

    // A little hack here. We use the Up facing sqaure face, and use the order of the vertices to bring the two
    // at the bottom down to create the slope. Horrible.
    memcpy(&slope.slope_vertices, faces->square_face.vertices[POS_Y], 4 * sizeof(V3));
    slope.slope_vertices[((i  )%4)].y = -0.5;
    slope.slope_vertices[((i+1)%4)].y = -0.5;

    slope.block_direction.slope_direction.facing = xz_sides_as_xz[i];
    slope.block_direction.slope_direction.sloping_down = true;

    block_shape_flyweights->slopes[xz_sides_as_xz[i]][0] = slope;
  }

  slope.side_faces[NEG_Y] = &faces->empty_face;
  slope.side_faces[POS_Y] = &faces->square_face;
  for (int i = 0; i < 4; i++) {
    slope.side_faces[xz_sides[i]] = &faces->empty_face;
    slope.side_faces[xz_sides[i+1]] = &faces->triangle_faces[TOP_LEFT];
    slope.side_faces[xz_sides[i+2]] = &faces->square_face;
    slope.side_faces[xz_sides[i+3]] = &faces->triangle_faces[TOP_RIGHT];

    slope.block_direction.slope_direction.facing = xz_sides_as_xz[i];
    slope.block_direction.slope_direction.sloping_down = false;

    // A little hack here. We use the down facing sqaure face, and use the order of the vertices to bring the two
    // at the top up to create the slope. Horrible.
    memcpy(&slope.slope_vertices, faces->square_face.vertices[NEG_Y], 4 * sizeof(V3));
    slope.slope_vertices[(i+2)%4].y = -0.5;
    slope.slope_vertices[(i+3)%4].y = -0.5;

    block_shape_flyweights->slopes[xz_sides_as_xz[i]][1] = slope;
  }
}

GLuint generateChunkVertexBuffer() {
  int block_count = CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE;
  // NOTE(nathan): Using V3 here is not great in the long run - colour will probably have to go here too, unless
  // it's stored in a separate buffer.
  int vertices_per_block = 6 * 4;
  size_t size_per_block = vertices_per_block * sizeof(V3);
  size_t vertex_buffer_size = block_count * size_per_block;
  // NOTE(nathan): Do we need to malloc? Could this go on the stack?
  V3* vertex_buffer = malloc(vertex_buffer_size);


  float base_verticies[] = {
    // NEG Z
    -0.5f,  0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
    // POS Z
    -0.5f, -0.5f,  0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,
    // NEG X
    -0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f,  0.5f,
    // POS X
     0.5f,  0.5f, -0.5f,
     0.5f,  0.5f,  0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f, -0.5f, -0.5f,
     // NEG Y
     0.5f, -0.5f, -0.5f,
     0.5f, -0.5f,  0.5f,
    -0.5f, -0.5f,  0.5f,
    -0.5f, -0.5f, -0.5f,
    // POS Y
     0.5f,  0.5f,  0.5f,
     0.5f,  0.5f, -0.5f,
    -0.5f,  0.5f, -0.5f,
    -0.5f,  0.5f,  0.5f,
  };
  for (int x = 0; x < CHUNK_SIZE; x++) {
    for (int y = 0; y < CHUNK_SIZE; y++) {
      for (int z = 0; z < CHUNK_SIZE; z++) {
        int block_index = z * CHUNK_SIZE * CHUNK_SIZE + y * CHUNK_SIZE + x;
        V3* cursor = vertex_buffer + block_index * size_per_block;
        memcpy(cursor, base_verticies, size_per_block);
        V3 offset = v3(x, y, z);
        for (int v = 0; v < vertices_per_block; v++) {
          *(cursor + v) += offset;
        }
      }
    }
  }

  GLuint vbo;
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, vertex_buffer_size, vertex_buffer, GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  free(vertex_buffer)
  return vbo;
}

static void copyVertices(float* vertex_cursor, float* vertices, int x, int y, int z, int num_vertices) {
  for (int v = 0; v < num_vertices; v++) {
    *(vertex_cursor + v * 3 + 0) = vertices[v*3 + 0] + x;
    *(vertex_cursor + v * 3 + 1) = vertices[v*3 + 1] + y;
    *(vertex_cursor + v * 3 + 2) = vertices[v*3 + 2] + z;
  }
}

void fillChunkRenderData(Chunk* chunk) {
  struct timespec start_time, end_time;
  float delta;
  clock_gettime(CLOCK_MONOTONIC_RAW, &start_time);
  printf("Allocating space for %d vertices.\n", CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE * 6 * 4);
  V3* vertices = (V3*)malloc(CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE * 6 * 4 * sizeof(V3));
  printf("Allocating space for %d indices.\n", CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE * 6 * 6);
  int* indices = (int*)malloc(CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE * 6 * 6 * sizeof(int));

  clock_gettime(CLOCK_MONOTONIC_RAW, &end_time);
  delta = (end_time.tv_sec - start_time.tv_sec) + (end_time.tv_nsec - start_time.tv_nsec) / 1000000000.0f;
  printf("Chunk Render data generation took %.4f seconds.\n", delta);

  for(int x = 0; x < CHUNK_SIZE; x++) {
    for(int y = 0; y < CHUNK_SIZE; y++) {
      for(int z = 0; z < CHUNK_SIZE; z++) {
        Block* block = &chunk->blocks[x][y][z];
        V3 offset = v3(x, y, z);
        BlockShapeFlyweight* block_shape = block->block_shape;
        if (block_shape->block_type == AIR) {
          continue;
        }
        for(int d = 0; d < DIRECTION_COUNT; d++) {
          Face* face = block_shape->side_faces[d];
          if (face->type == EMPTY) {
            continue;
          } else if (face->type == TRIANGLE) {
            copyVertices(vertex_cursor, face->vertices[d], x, y, z, 3);

            for (int p = 0; p < 3; p++) {
              *(index_cursor + p) = vertex_count + p;
            }

            vertex_count += 3;
            vertex_cursor += 9;
            index_cursor += 3;
          } else if (face->type == SQUARE) {
            copyVertices(vertex_cursor, face->vertices[d], x, y, z, 4);
            memcpy(index_cursor, square_indices, 6 * sizeof(unsigned int));
            for (unsigned int p = 0; p < 6; p++) {
              *(index_cursor + p) += vertex_count;
            }

            vertex_count += 4;
            vertex_cursor += 12;
            index_cursor += 6;
          }
        }
        if (block_shape->block_type == SLOPE) {
          copyVertices(vertex_cursor, block_shape->slope_vertices, x, y, z, 4);

          memcpy(index_cursor, square_indices, 6 * sizeof(unsigned int));
          for (unsigned int p = 0; p < 6; p++) {
            *(index_cursor + p) += vertex_count;
          }

          vertex_count += 4;
          vertex_cursor += 12;
          index_cursor += 6;
        }
      }
    }
  }

  ptrdiff_t index_count = index_cursor - indices;
  printf("Vertex count: %d, Index Count: %td\n", vertex_count, index_count);

  clock_gettime(CLOCK_MONOTONIC_RAW, &end_time);
  delta = (end_time.tv_sec - start_time.tv_sec) + (end_time.tv_nsec - start_time.tv_nsec) / 1000000000.0f;
  printf("Chunk Render data generation took %.4f seconds.\n", delta);

  GLuint vao, vbo, ebo;
  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);
  glGenBuffers(1, &ebo);
  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ARRAY_BUFFER, (vertex_cursor - vertices) * sizeof(float), vertices, GL_STATIC_DRAW);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, (index_cursor - indices) * sizeof(unsigned int), indices, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  chunk->render_data.vao = vao;
  chunk->render_data.num_indices = (index_cursor - indices);

  clock_gettime(CLOCK_MONOTONIC_RAW, &end_time);
  delta = (end_time.tv_sec - start_time.tv_sec) + (end_time.tv_nsec - start_time.tv_nsec) / 1000000000.0f;
  printf("Chunk Render data generation took %.4f seconds.\n", delta);

  free(vertices);
  free(indices);

  clock_gettime(CLOCK_MONOTONIC_RAW, &end_time);
  delta = (end_time.tv_sec - start_time.tv_sec) + (end_time.tv_nsec - start_time.tv_nsec) / 1000000000.0f;
  printf("Chunk Render data generation took %.4f seconds.\n", delta);
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
