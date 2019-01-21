// TODO(nathan): I'm not sure how I feel about macros. These feel like they should be functions.
#define PUSH_VERTEX(vertex_cursor, vertex, colour) *(vertex_cursor) = { vertex, colour }; vertex_cursor++;
#define PUSH_SQUARE(cursor, v1, v2, v3, v4, colour) \
        PUSH_VERTEX(cursor, v1, colour); PUSH_VERTEX(cursor, v2, colour); PUSH_VERTEX(cursor, v3, colour); \
        PUSH_VERTEX(cursor, v1, colour); PUSH_VERTEX(cursor, v3, colour); PUSH_VERTEX(cursor, v4, colour); 
#define PUSH_TRIANGLE(cursor, v1, v2, v3, colour) \
        PUSH_VERTEX(cursor, v1, colour); PUSH_VERTEX(cursor, v2, colour); PUSH_VERTEX(cursor, v3, colour);

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

//void fillTriangleFace(Face* faces, Direction direction, V3 top_right_vertices_as_v3[3], V3 normal) {
//  Quaternion q = from_vector_and_angle(normal, M_PI/2);
//  V3* vs = top_right_vertices_as_v3;
//  faces[TOP_RIGHT].type = TRIANGLE;
//  V3 top_right_vertices[] = { vs[0], vs[1], vs[2] };
//  memcpy(&faces[TOP_RIGHT].vertices[direction], top_right_vertices, sizeof(V3) * 3);
//  vs[0] = rotate(q, vs[0]);
//  vs[1] = rotate(q, vs[1]);
//  vs[2] = rotate(q, vs[2]);
//  faces[TOP_LEFT].type = TRIANGLE;
//  V3 top_left_vertices[] = { vs[0], vs[1], vs[2] };
//  memcpy(&faces[TOP_LEFT].vertices[direction], top_left_vertices, sizeof(V3) * 3);
//
//  vs[0] = rotate(q, vs[0]);
//  vs[1] = rotate(q, vs[1]);
//  vs[2] = rotate(q, vs[2]);
//  faces[BOTTOM_LEFT].type = TRIANGLE;
//  V3 bottom_left_vertices[] = { vs[0], vs[1], vs[2] };
//  memcpy(&faces[BOTTOM_LEFT].vertices[direction], bottom_left_vertices, sizeof(V3) * 3);
//
//  vs[0] = rotate(q, vs[0]);
//  vs[1] = rotate(q, vs[1]);
//  vs[2] = rotate(q, vs[2]);
//  faces[BOTTOM_RIGHT].type = TRIANGLE;
//  V3 bottom_right_vertices[] = { vs[0], vs[1], vs[2] };
//  memcpy(&faces[BOTTOM_RIGHT].vertices[direction], bottom_right_vertices, sizeof(V3) * 3);
//}
//
//void initFaces(Faces* faces) {
//  faces->empty_face.type = EMPTY;
//
//  // NOTE(nathan): CBA to refactor this - faces now store V3 not float, but memcpy will still work
//  faces->square_face.type = SQUARE;
//  float neg_z_vertices[] = {
//    -0.5f,  0.5f, -0.5f,
//     0.5f,  0.5f, -0.5f,
//     0.5f, -0.5f, -0.5f,
//    -0.5f, -0.5f, -0.5f,
//  };
//  memcpy(&faces->square_face.vertices[NEG_Z], neg_z_vertices, sizeof(float) * 12);
//
//  float pos_z_vertices[] = {
//    -0.5f, -0.5f,  0.5f,
//     0.5f, -0.5f,  0.5f,
//     0.5f,  0.5f,  0.5f,
//    -0.5f,  0.5f,  0.5f,
//  };
//  memcpy(&faces->square_face.vertices[POS_Z], pos_z_vertices, sizeof(float) * 12);
//
//  float neg_x_vertices[] = {
//    -0.5f,  0.5f,  0.5f,
//    -0.5f,  0.5f, -0.5f,
//    -0.5f, -0.5f, -0.5f,
//    -0.5f, -0.5f,  0.5f,
//  };
//  memcpy(&faces->square_face.vertices[NEG_X], neg_x_vertices, sizeof(float) * 12);
//
//  float pos_x_vertices[] = {
//     0.5f,  0.5f, -0.5f,
//     0.5f,  0.5f,  0.5f,
//     0.5f, -0.5f,  0.5f,
//     0.5f, -0.5f, -0.5f,
//  };
//  memcpy(&faces->square_face.vertices[POS_X], pos_x_vertices, sizeof(float) * 12);
//
//  float neg_y_vertices[] = {
//     0.5f, -0.5f, -0.5f,
//     0.5f, -0.5f,  0.5f,
//    -0.5f, -0.5f,  0.5f,
//    -0.5f, -0.5f, -0.5f,
//  };
//  memcpy(&faces->square_face.vertices[NEG_Y], neg_y_vertices, sizeof(float) * 12);
//
//  float pos_y_vertices[] = {
//     0.5f,  0.5f,  0.5f,
//     0.5f,  0.5f, -0.5f,
//    -0.5f,  0.5f, -0.5f,
//    -0.5f,  0.5f,  0.5f,
//  };
//  memcpy(&faces->square_face.vertices[POS_Y], pos_y_vertices, sizeof(float) * 12);
//
//  V3 neg_z_traingle[3] = { v3(-0.5f,  0.5f, -0.5f), v3( 0.5f,  0.5f, -0.5f), v3(-0.5f, -0.5f, -0.5f) };
//  fillTriangleFace(faces->triangle_faces, NEG_Z, neg_z_traingle, v3(0.0, 0.0, -1.0));
//  V3 pos_z_traingle[3] = { v3( 0.5f,  0.5f,  0.5f), v3(-0.5f,  0.5f,  0.5f), v3( 0.5f, -0.5f,  0.5f) };
//  fillTriangleFace(faces->triangle_faces, POS_Z, pos_z_traingle, v3(0.0, 0.0, 1.0));
//
//  V3 neg_x_traingle[3] = { v3(-0.5f, 0.5f,  0.5f), v3(-0.5f,  0.5f, -0.5f), v3(-0.5f, -0.5f,  0.5f) };
//  fillTriangleFace(faces->triangle_faces, NEG_X, neg_x_traingle, v3(-1.0, 0.0, 0.0));
//  V3 pos_x_traingle[3] = { v3( 0.5f, 0.5f, -0.5f), v3( 0.5f,  0.5f,  0.5f), v3( 0.5f, -0.5f, -0.5f) };
//  fillTriangleFace(faces->triangle_faces, POS_X, pos_x_traingle, v3(1.0, 0.0, 0.0));
//
//  V3 neg_y_traingle[3] = { v3(-0.5f,  0.5f,  0.5f), v3( 0.5f,  0.5f, -0.5f), v3( 0.5f,  0.5f,  0.5f) };
//  fillTriangleFace(faces->triangle_faces, NEG_Y, neg_y_traingle, v3(0.0, -1.0, 0.0));
//  V3 pos_y_traingle[3] = { v3(-0.5f, -0.5f, -0.5f), v3( 0.5f, -0.5f,  0.5f), v3( 0.5f, -0.5f, -0.5f) };
//  fillTriangleFace(faces->triangle_faces, POS_Y, pos_y_traingle, v3(0.0, 1.0, 0.0));
//}
//
//void fillBlockFlyweights(BlockShapeFlyweights* block_shape_flyweights, Faces* faces) {
//  BlockShapeFlyweight empty;
//  empty.block_type = AIR;
//  for (int d = 0; d < DIRECTION_COUNT; d++) {
//    empty.side_faces[d] = &faces->empty_face;
//  }
//  block_shape_flyweights->empty = empty;
//
//  BlockShapeFlyweight cube;
//  cube.block_type = CUBE;
//  for (int d = 0; d < DIRECTION_COUNT; d++) {
//    cube.side_faces[d] = &faces->square_face;
//  }
//  block_shape_flyweights->cube = cube;
//
//  // Because I'm lazy we'll reuse this slope object and then copy it into the flyweights struct
//  BlockShapeFlyweight slope;
//  slope.block_type = SLOPE;
//  slope.side_faces[NEG_Y] = &faces->square_face;
//  slope.side_faces[POS_Y] = &faces->empty_face;
//
//  Direction xz_sides[] = {POS_X, NEG_Z, NEG_X, POS_Z, POS_X, NEG_Z, NEG_X, POS_Z};
//  XZDirection xz_sides_as_xz[] = {XZ_POS_X, XZ_NEG_Z, XZ_NEG_X, XZ_POS_Z};
//
//  for (int i = 0; i < 4; i++) {
//    slope.side_faces[xz_sides[i]] = &faces->empty_face;
//    slope.side_faces[xz_sides[i+1]] = &faces->triangle_faces[BOTTOM_RIGHT];
//    slope.side_faces[xz_sides[i+2]] = &faces->square_face;
//    slope.side_faces[xz_sides[i+3]] = &faces->triangle_faces[BOTTOM_LEFT];
//
//    // A little hack here. We use the Up facing sqaure face, and use the order of the vertices to bring the two
//    // at the bottom down to create the slope. Horrible.
//    memcpy(&slope.slope_vertices, faces->square_face.vertices[POS_Y], 4 * sizeof(V3));
//    slope.slope_vertices[((i  )%4)].y = -0.5;
//    slope.slope_vertices[((i+1)%4)].y = -0.5;
//
//    slope.block_direction.slope_direction.facing = xz_sides_as_xz[i];
//    slope.block_direction.slope_direction.sloping_down = true;
//
//    block_shape_flyweights->slopes[xz_sides_as_xz[i]][0] = slope;
//  }
//
//  slope.side_faces[NEG_Y] = &faces->empty_face;
//  slope.side_faces[POS_Y] = &faces->square_face;
//  for (int i = 0; i < 4; i++) {
//    slope.side_faces[xz_sides[i]] = &faces->empty_face;
//    slope.side_faces[xz_sides[i+1]] = &faces->triangle_faces[TOP_LEFT];
//    slope.side_faces[xz_sides[i+2]] = &faces->square_face;
//    slope.side_faces[xz_sides[i+3]] = &faces->triangle_faces[TOP_RIGHT];
//
//    slope.block_direction.slope_direction.facing = xz_sides_as_xz[i];
//    slope.block_direction.slope_direction.sloping_down = false;
//
//    // A little hack here. We use the down facing sqaure face, and use the order of the vertices to bring the two
//    // at the top up to create the slope. Horrible.
//    memcpy(&slope.slope_vertices, faces->square_face.vertices[NEG_Y], 4 * sizeof(V3));
//    slope.slope_vertices[(i+2)%4].y = -0.5;
//    slope.slope_vertices[(i+3)%4].y = -0.5;
//
//    block_shape_flyweights->slopes[xz_sides_as_xz[i]][1] = slope;
//  }
//}

inline BlockShape getBlockShapeAt(Chunk* c, int x, int y, int z) {
  if (x < 0 || y < 0 || z < 0 || x >= CHUNK_SIZE || y >= CHUNK_SIZE || z >= CHUNK_SIZE) {
    return AIR;
  }
  return c->blocks[x][y][z].block_shape;
}

V3 RED = v3(1, 0, 0);
V3 GREEN = v3(0, 1, 0);
V3 BLUE = v3(0, 0, 1);
V3 YELLOW = v3(0.8, 0.8, 0);

void fillChunkRenderData(Chunk* chunk) {
  struct timespec start_time, end_time;
  float delta;
  clock_gettime(CLOCK_MONOTONIC_RAW, &start_time);
  printf("Allocating space for %d vertices.\n", CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE * 6 * 6);
  ChunkVertex* vertices = (ChunkVertex*)malloc(CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE * 6 * 6 * sizeof(ChunkVertex));
  ChunkVertex* vertex_cursor = vertices;

  //clock_gettime(CLOCK_MONOTONIC_RAW, &end_time);
  //delta = (end_time.tv_sec - start_time.tv_sec) + (end_time.tv_nsec - start_time.tv_nsec) / 1000000000.0f;
  //printf("Chunk Render data generation took %.4f seconds.\n", delta);

  for(int x = 0; x < CHUNK_SIZE; x++) {
    for(int y = 0; y < CHUNK_SIZE; y++) {
      for(int z = 0; z < CHUNK_SIZE; z++) {
        Direction opposite_side;
        bool* block_side_occlusion;
        BlockShape block_shape = chunk->blocks[x][y][z].block_shape;
        if (block_shape == AIR) {
          continue;
        }

        float fx = x;
        float fy = y;
        float fz = z;
        V3 xyz = { fx    , fy    , fz    };
        V3 Xyz = { fx + 1, fy    , fz    };
        V3 xYz = { fx    , fy + 1, fz    };
        V3 xyZ = { fx    , fy    , fz + 1};
        V3 xYZ = { fx    , fy + 1, fz + 1};
        V3 XyZ = { fx + 1, fy    , fz + 1};
        V3 XYz = { fx + 1, fy + 1, fz    };
        V3 XYZ = { fx + 1, fy + 1, fz + 1};

        // POS Z
        opposite_side = NEG_Z;
        block_side_occlusion = BLOCK_SIDE_OCCLUSION_BITFIELD + (getBlockShapeAt(chunk, x, y, z+1) * 30 + opposite_side * 5);
        switch(block_shape) {
          case CUBE:
          case NEG_Z_NEG_Y_SLOPE:
          case NEG_Z_POS_Y_SLOPE:
            if (block_side_occlusion[FULL]) {
              break;
            }
            PUSH_SQUARE(vertex_cursor, xyZ, XyZ, XYZ, xYZ, BLUE)
            break;

          case POS_X_NEG_Y_SLOPE:
            if (block_side_occlusion[BOTTOM_LEFT]) {
              break;
            }
            PUSH_TRIANGLE(vertex_cursor, xyZ, XyZ, xYZ, BLUE);
            break;

          case NEG_X_NEG_Y_SLOPE:
            if (block_side_occlusion[BOTTOM_RIGHT]) {
              break;
            }
            PUSH_TRIANGLE(vertex_cursor, xyZ, XyZ, XYZ, BLUE);
            break;

          case POS_X_POS_Y_SLOPE:
            if (block_side_occlusion[TOP_LEFT]) {
              break;
            }
            PUSH_TRIANGLE(vertex_cursor, xyZ, XYZ, xYZ, BLUE);
            break;

          case NEG_X_POS_Y_SLOPE:
            if (block_side_occlusion[TOP_RIGHT]) {
              break;
            }
            PUSH_TRIANGLE(vertex_cursor, XyZ, XYZ, xYZ, BLUE);
            break;
      
          case POS_Z_POS_Y_SLOPE:
          case POS_Z_NEG_Y_SLOPE:
          case AIR:
          case BLOCK_SHAPE_COUNT:
            break;
        }


        // POS X
        opposite_side = NEG_X;
        block_side_occlusion = BLOCK_SIDE_OCCLUSION_BITFIELD + (getBlockShapeAt(chunk, x+1, y, z) * 30 + opposite_side * 5);
        switch(block_shape) {
          case CUBE:
          case NEG_X_NEG_Y_SLOPE:
          case NEG_X_POS_Y_SLOPE:
            if (block_side_occlusion[FULL]) {
              break;
            }
            PUSH_SQUARE(vertex_cursor, XyZ, Xyz, XYz, XYZ, RED);
            break;

          case NEG_Z_NEG_Y_SLOPE:
            if (block_side_occlusion[BOTTOM_LEFT]) {
              break;
            }
            PUSH_TRIANGLE(vertex_cursor, XyZ, Xyz, XYZ, RED);
            break;

          case POS_Z_NEG_Y_SLOPE:
            if (block_side_occlusion[BOTTOM_RIGHT]) {
              break;
            }
            PUSH_TRIANGLE(vertex_cursor, XyZ, Xyz, XYz, RED);
            break;

          case NEG_Z_POS_Y_SLOPE:
            if (block_side_occlusion[TOP_LEFT]) {
              break;
            }
            PUSH_TRIANGLE(vertex_cursor, XyZ, XYz, XYZ, RED);
            break;

          case POS_Z_POS_Y_SLOPE:
            if (block_side_occlusion[TOP_RIGHT]) {
              break;
            }
            PUSH_TRIANGLE(vertex_cursor, Xyz, XYz, XYZ, RED);
            break;
      
          case POS_X_POS_Y_SLOPE:
          case POS_X_NEG_Y_SLOPE:
          case AIR:
          case BLOCK_SHAPE_COUNT:
            break;
        }

        // NEG Z
        opposite_side = POS_Z;
        block_side_occlusion = BLOCK_SIDE_OCCLUSION_BITFIELD + (getBlockShapeAt(chunk, x, y, z-1) * 30 + opposite_side * 5);
        switch(block_shape) {
          case CUBE:
          case POS_Z_NEG_Y_SLOPE:
          case POS_Z_POS_Y_SLOPE:
            if (block_side_occlusion[FULL]) {
              break;
            }
            PUSH_SQUARE(vertex_cursor, xyz, xYz, XYz, Xyz, BLUE);
            break;

          case NEG_X_NEG_Y_SLOPE:
            if (block_side_occlusion[BOTTOM_LEFT]) {
              break;
            }
            PUSH_TRIANGLE(vertex_cursor, Xyz, xyz, XYz, BLUE);
            break;

          case POS_X_NEG_Y_SLOPE:
            if (block_side_occlusion[BOTTOM_RIGHT]) {
              break;
            }
            PUSH_TRIANGLE(vertex_cursor, Xyz, xyz, xYz, BLUE);
            break;

          case NEG_X_POS_Y_SLOPE:
            if (block_side_occlusion[TOP_LEFT]) {
              break;
            }
            PUSH_TRIANGLE(vertex_cursor, Xyz, xYz, XYz, BLUE);
            break;

          case POS_X_POS_Y_SLOPE:
            if (block_side_occlusion[TOP_RIGHT]) {
              break;
            }
            PUSH_TRIANGLE(vertex_cursor, xyz, xYz, XYz, BLUE);
            break;
      
          case NEG_Z_POS_Y_SLOPE:
          case NEG_Z_NEG_Y_SLOPE:
          case AIR:
          case BLOCK_SHAPE_COUNT:
            break;
        }
        
        // NEG X
        opposite_side = POS_X;
        block_side_occlusion = BLOCK_SIDE_OCCLUSION_BITFIELD + (getBlockShapeAt(chunk, x-1, y, z) * 30 + opposite_side * 5);
        switch(block_shape) {
          case CUBE:
          case POS_X_NEG_Y_SLOPE:
          case POS_X_POS_Y_SLOPE:
            if (block_side_occlusion[FULL]) {
              break;
            }
            PUSH_SQUARE(vertex_cursor, xyz, xyZ, xYZ, xYz, RED);
            break;

          case POS_Z_NEG_Y_SLOPE:
            if (block_side_occlusion[BOTTOM_LEFT]) {
              break;
            }
            PUSH_TRIANGLE(vertex_cursor, xyz, xyZ, xYz, RED);
            break;

          case NEG_Z_NEG_Y_SLOPE:
            if (block_side_occlusion[BOTTOM_RIGHT]) {
              break;
            }
            PUSH_TRIANGLE(vertex_cursor, xyz, xyZ, xYZ, RED);
            break;

          case POS_Z_POS_Y_SLOPE:
            if (block_side_occlusion[TOP_LEFT]) {
              break;
            }
            PUSH_TRIANGLE(vertex_cursor, xyz, xYZ, xYz, RED);
            break;

          case NEG_Z_POS_Y_SLOPE:
            if (block_side_occlusion[TOP_RIGHT]) {
              break;
            }
            PUSH_TRIANGLE(vertex_cursor, xyZ, xYZ, xYz, RED);
            break;
      
          case NEG_X_POS_Y_SLOPE:
          case NEG_X_NEG_Y_SLOPE:
          case AIR:
          case BLOCK_SHAPE_COUNT:
            break;
        }
        
        // POS Y
        opposite_side = NEG_Y;
        block_side_occlusion = BLOCK_SIDE_OCCLUSION_BITFIELD + (getBlockShapeAt(chunk, x, y+1, z) * 30 + opposite_side * 5);
        switch(block_shape) {
          case CUBE:
          case POS_X_POS_Y_SLOPE:
          case POS_Z_POS_Y_SLOPE:
          case NEG_Z_POS_Y_SLOPE:
          case NEG_X_POS_Y_SLOPE:
            if (block_side_occlusion[FULL]) {
              break;
            }
            PUSH_SQUARE(vertex_cursor, xYZ, XYZ, XYz, xYz, GREEN);
            break;

          case POS_Z_NEG_Y_SLOPE:
          case POS_X_NEG_Y_SLOPE:
          case NEG_Z_NEG_Y_SLOPE:
          case NEG_X_NEG_Y_SLOPE:
          case AIR:
          case BLOCK_SHAPE_COUNT:
            break;
        }
        

        // NEG Y
        opposite_side = POS_Y;
        block_side_occlusion = BLOCK_SIDE_OCCLUSION_BITFIELD + (getBlockShapeAt(chunk, x, y-1, z) * 30 + opposite_side * 5);
        switch(block_shape) {
          case CUBE:
          case POS_Z_NEG_Y_SLOPE:
          case POS_X_NEG_Y_SLOPE:
          case NEG_Z_NEG_Y_SLOPE:
          case NEG_X_NEG_Y_SLOPE:
            if (block_side_occlusion[FULL]) {
              break;
            }
            PUSH_SQUARE(vertex_cursor, xyz, Xyz, XyZ, xyZ, GREEN);
            break;

          case POS_X_POS_Y_SLOPE:
          case POS_Z_POS_Y_SLOPE:
          case NEG_Z_POS_Y_SLOPE:
          case NEG_X_POS_Y_SLOPE:
          case AIR:
          case BLOCK_SHAPE_COUNT:
            break;
        }

        // SLOPE
        switch(block_shape) {
          case POS_Z_NEG_Y_SLOPE:
            PUSH_SQUARE(vertex_cursor, xyZ, XyZ, XYz, xYz, YELLOW);
            break;
          case POS_X_NEG_Y_SLOPE:
            PUSH_SQUARE(vertex_cursor, XyZ, Xyz, xYz, xYZ, YELLOW);
            break;
          case NEG_Z_NEG_Y_SLOPE:
            PUSH_SQUARE(vertex_cursor, Xyz, xyz, xYZ, XYZ, YELLOW);
            break;
          case NEG_X_NEG_Y_SLOPE:
            PUSH_SQUARE(vertex_cursor, xyz, xyZ, XYZ, XYz, YELLOW);
            break;
          case POS_Z_POS_Y_SLOPE:
            PUSH_SQUARE(vertex_cursor, Xyz, XYZ, xYZ, xyz, YELLOW);
            break;
          case POS_X_POS_Y_SLOPE:
            PUSH_SQUARE(vertex_cursor, xyz, XYz, XYZ, xyZ, YELLOW);
            break;
          case NEG_Z_POS_Y_SLOPE:
            PUSH_SQUARE(vertex_cursor, xyZ, xYz, XYz, XyZ, YELLOW);
            break;
          case NEG_X_POS_Y_SLOPE:
            PUSH_SQUARE(vertex_cursor, XyZ, xYZ, xYz, Xyz, YELLOW);
            break;

          case CUBE:
          case AIR:
          case BLOCK_SHAPE_COUNT:
            break;
        }
      }
    }
  }
  
  ptrdiff_t vertex_count = vertex_cursor - vertices;
  printf("Vertex count: %ld\n", vertex_count);

  //clock_gettime(CLOCK_MONOTONIC_RAW, &end_time);
  //delta = (end_time.tv_sec - start_time.tv_sec) + (end_time.tv_nsec - start_time.tv_nsec) / 1000000000.0f;
  //printf("Chunk Render data generation took %.4f seconds.\n", delta);

  GLuint vao, vbo;
  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);
  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, vertex_count * sizeof(ChunkVertex), vertices, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);

  chunk->render_data.vao = vao;
  chunk->render_data.num_vertices = vertex_count;

  //clock_gettime(CLOCK_MONOTONIC_RAW, &end_time);
  //delta = (end_time.tv_sec - start_time.tv_sec) + (end_time.tv_nsec - start_time.tv_nsec) / 1000000000.0f;
  //printf("Chunk Render data generation took %.4f seconds.\n", delta);

  free(vertices);

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
