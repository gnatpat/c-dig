// TODO(nathan): I'm not sure how I feel about macros. These feel like they should be functions.
#define PUSH_VERTEX(vertex_cursor, vertex, colour) *(vertex_cursor) = { vertex, colour }; vertex_cursor++;
#define PUSH_SQUARE(cursor, v1, v2, v3, v4, colour) \
        PUSH_VERTEX(cursor, v1, colour); PUSH_VERTEX(cursor, v2, colour); PUSH_VERTEX(cursor, v3, colour); \
        PUSH_VERTEX(cursor, v1, colour); PUSH_VERTEX(cursor, v3, colour); PUSH_VERTEX(cursor, v4, colour); 
#define PUSH_TRIANGLE(cursor, v1, v2, v3, colour) \
        PUSH_VERTEX(cursor, v1, colour); PUSH_VERTEX(cursor, v2, colour); PUSH_VERTEX(cursor, v3, colour);

V3 RED = v3(1, 0, 0);
V3 GREEN = v3(0, 1, 0);
V3 BLUE = v3(0, 0, 1);
V3 YELLOW = v3(1, 1, 0);
V3 MAGENTA = v3(1, 0, 1);
V3 CYAN = v3(0, 1, 1);


inline BlockShape getBlockShapeAt(Chunk* c, int x, int y, int z) {
  if (x < 0 || y < 0 || z < 0 || x >= CHUNK_SIZE || y >= CHUNK_SIZE || z >= CHUNK_SIZE) {
    return AIR;
  }
  return c->blocks[x][y][z].block_shape;
}


void fillChunkRenderData(Chunk* chunk) {
  struct timespec start_time, end_time;
  float delta;
  clock_gettime(CLOCK_MONOTONIC_RAW, &start_time);
  printf("Allocating space for %d vertices.\n", CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE * 6 * 6);
  ChunkVertex* vertices = (ChunkVertex*)malloc(CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE * 6 * 6 * sizeof(ChunkVertex));
  ChunkVertex* vertex_cursor = vertices;

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
          case NEG_POS_DIAGONAL:
          case POS_POS_DIAGONAL:
            if (block_side_occlusion[FULL]) {
              break;
            }
            PUSH_SQUARE(vertex_cursor, xyZ, XyZ, XYZ, xYZ, BLUE)
            break;

          case POS_X_NEG_Y_SLOPE:
          case NEG_NEG_POS_CORNER:
            if (block_side_occlusion[BOTTOM_LEFT]) {
              break;
            }
            PUSH_TRIANGLE(vertex_cursor, xyZ, XyZ, xYZ, BLUE);
            break;

          case NEG_X_NEG_Y_SLOPE:
          case POS_NEG_POS_CORNER:
            if (block_side_occlusion[BOTTOM_RIGHT]) {
              break;
            }
            PUSH_TRIANGLE(vertex_cursor, xyZ, XyZ, XYZ, BLUE);
            break;

          case POS_X_POS_Y_SLOPE:
          case NEG_POS_POS_CORNER:
            if (block_side_occlusion[TOP_LEFT]) {
              break;
            }
            PUSH_TRIANGLE(vertex_cursor, xyZ, XYZ, xYZ, BLUE);
            break;

          case NEG_X_POS_Y_SLOPE:
          case POS_POS_POS_CORNER:
            if (block_side_occlusion[TOP_RIGHT]) {
              break;
            }
            PUSH_TRIANGLE(vertex_cursor, XyZ, XYZ, xYZ, BLUE);
            break;
      
          case AIR:
          case POS_Z_POS_Y_SLOPE:
          case POS_Z_NEG_Y_SLOPE:
          case NEG_NEG_NEG_CORNER:
          case POS_NEG_NEG_CORNER:
          case NEG_POS_NEG_CORNER:
          case POS_POS_NEG_CORNER:
          case NEG_NEG_DIAGONAL:
          case POS_NEG_DIAGONAL:
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
          case POS_NEG_DIAGONAL:
          case POS_POS_DIAGONAL:
            if (block_side_occlusion[FULL]) {
              break;
            }
            PUSH_SQUARE(vertex_cursor, XyZ, Xyz, XYz, XYZ, RED);
            break;

          case NEG_Z_NEG_Y_SLOPE:
          case POS_NEG_POS_CORNER:
            if (block_side_occlusion[BOTTOM_LEFT]) {
              break;
            }
            PUSH_TRIANGLE(vertex_cursor, XyZ, Xyz, XYZ, RED);
            break;

          case POS_Z_NEG_Y_SLOPE:
          case POS_NEG_NEG_CORNER:
            if (block_side_occlusion[BOTTOM_RIGHT]) {
              break;
            }
            PUSH_TRIANGLE(vertex_cursor, XyZ, Xyz, XYz, RED);
            break;

          case NEG_Z_POS_Y_SLOPE:
          case POS_POS_POS_CORNER:
            if (block_side_occlusion[TOP_LEFT]) {
              break;
            }
            PUSH_TRIANGLE(vertex_cursor, XyZ, XYz, XYZ, RED);
            break;

          case POS_Z_POS_Y_SLOPE:
          case POS_POS_NEG_CORNER:
            if (block_side_occlusion[TOP_RIGHT]) {
              break;
            }
            PUSH_TRIANGLE(vertex_cursor, Xyz, XYz, XYZ, RED);
            break;
      
          case AIR:
          case POS_X_POS_Y_SLOPE:
          case POS_X_NEG_Y_SLOPE:
          case NEG_NEG_NEG_CORNER:
          case NEG_NEG_POS_CORNER:
          case NEG_POS_NEG_CORNER:
          case NEG_POS_POS_CORNER:
          case NEG_NEG_DIAGONAL:
          case NEG_POS_DIAGONAL:
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
          case NEG_NEG_DIAGONAL:
          case POS_NEG_DIAGONAL:
            if (block_side_occlusion[FULL]) {
              break;
            }
            PUSH_SQUARE(vertex_cursor, xyz, xYz, XYz, Xyz, YELLOW);
            break;

          case NEG_X_NEG_Y_SLOPE:
          case POS_NEG_NEG_CORNER:
            if (block_side_occlusion[BOTTOM_LEFT]) {
              break;
            }
            PUSH_TRIANGLE(vertex_cursor, Xyz, xyz, XYz, YELLOW);
            break;

          case POS_X_NEG_Y_SLOPE:
          case NEG_NEG_NEG_CORNER:
            if (block_side_occlusion[BOTTOM_RIGHT]) {
              break;
            }
            PUSH_TRIANGLE(vertex_cursor, Xyz, xyz, xYz, YELLOW);
            break;

          case NEG_X_POS_Y_SLOPE:
          case POS_POS_NEG_CORNER:
            if (block_side_occlusion[TOP_LEFT]) {
              break;
            }
            PUSH_TRIANGLE(vertex_cursor, Xyz, xYz, XYz, YELLOW);
            break;

          case POS_X_POS_Y_SLOPE:
          case NEG_POS_NEG_CORNER:
            if (block_side_occlusion[TOP_RIGHT]) {
              break;
            }
            PUSH_TRIANGLE(vertex_cursor, xyz, xYz, XYz, YELLOW);
            break;
      
          case AIR:
          case NEG_Z_POS_Y_SLOPE:
          case NEG_Z_NEG_Y_SLOPE:
          case POS_NEG_POS_CORNER:
          case NEG_NEG_POS_CORNER:
          case NEG_POS_POS_CORNER:
          case POS_POS_POS_CORNER:
          case NEG_POS_DIAGONAL:
          case POS_POS_DIAGONAL:
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
          case NEG_NEG_DIAGONAL:
          case NEG_POS_DIAGONAL:
            if (block_side_occlusion[FULL]) {
              break;
            }
            PUSH_SQUARE(vertex_cursor, xyz, xyZ, xYZ, xYz, CYAN);
            break;

          case POS_Z_NEG_Y_SLOPE:
          case NEG_NEG_NEG_CORNER:
            if (block_side_occlusion[BOTTOM_LEFT]) {
              break;
            }
            PUSH_TRIANGLE(vertex_cursor, xyz, xyZ, xYz, CYAN);
            break;

          case NEG_Z_NEG_Y_SLOPE:
          case NEG_NEG_POS_CORNER:
            if (block_side_occlusion[BOTTOM_RIGHT]) {
              break;
            }
            PUSH_TRIANGLE(vertex_cursor, xyz, xyZ, xYZ, CYAN);
            break;

          case POS_Z_POS_Y_SLOPE:
          case NEG_POS_NEG_CORNER:
            if (block_side_occlusion[TOP_LEFT]) {
              break;
            }
            PUSH_TRIANGLE(vertex_cursor, xyz, xYZ, xYz, CYAN);
            break;

          case NEG_Z_POS_Y_SLOPE:
          case NEG_POS_POS_CORNER:
            if (block_side_occlusion[TOP_RIGHT]) {
              break;
            }
            PUSH_TRIANGLE(vertex_cursor, xyZ, xYZ, xYz, CYAN);
            break;
      
          case NEG_X_POS_Y_SLOPE:
          case NEG_X_NEG_Y_SLOPE:
          case AIR:
          case POS_NEG_NEG_CORNER:
          case POS_NEG_POS_CORNER:
          case POS_POS_NEG_CORNER:
          case POS_POS_POS_CORNER:
          case POS_NEG_DIAGONAL:
          case POS_POS_DIAGONAL:
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

          case NEG_POS_POS_CORNER:
          case NEG_POS_DIAGONAL:
            if (block_side_occlusion[BOTTOM_LEFT]) {
              break;
            }
            PUSH_TRIANGLE(vertex_cursor, xYZ, XYZ, xYz, GREEN);
            break;

          case POS_POS_POS_CORNER:
          case POS_POS_DIAGONAL:
            if (block_side_occlusion[BOTTOM_RIGHT]) {
              break;
            }
            PUSH_TRIANGLE(vertex_cursor, xYZ, XYZ, XYz, GREEN);
            break;

          case NEG_POS_NEG_CORNER:
          case NEG_NEG_DIAGONAL:
            if (block_side_occlusion[TOP_LEFT]) {
              break;
            }
            PUSH_TRIANGLE(vertex_cursor, xYZ, XYz, xYz, GREEN);
            break;

          case POS_POS_NEG_CORNER:
          case POS_NEG_DIAGONAL:
            if (block_side_occlusion[TOP_RIGHT]) {
              break;
            }
            PUSH_TRIANGLE(vertex_cursor, XYZ, XYz, xYz, GREEN);
            break;

          case AIR:
          case POS_Z_NEG_Y_SLOPE:
          case POS_X_NEG_Y_SLOPE:
          case NEG_Z_NEG_Y_SLOPE:
          case NEG_X_NEG_Y_SLOPE:
          case POS_NEG_NEG_CORNER:
          case NEG_NEG_NEG_CORNER:
          case NEG_NEG_POS_CORNER:
          case POS_NEG_POS_CORNER:
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
            PUSH_SQUARE(vertex_cursor, xyz, Xyz, XyZ, xyZ, MAGENTA);
            break;

          case NEG_NEG_NEG_CORNER:
          case NEG_NEG_DIAGONAL:
            if (block_side_occlusion[BOTTOM_LEFT]) {
              break;
            }
            PUSH_TRIANGLE(vertex_cursor, xyz, Xyz, xyZ, MAGENTA);
            break;

          case POS_NEG_NEG_CORNER:
          case POS_NEG_DIAGONAL:
            if (block_side_occlusion[BOTTOM_RIGHT]) {
              break;
            }
            PUSH_TRIANGLE(vertex_cursor, Xyz, XyZ, xyz, MAGENTA);
            break;

          case NEG_NEG_POS_CORNER:
          case NEG_POS_DIAGONAL:
            if (block_side_occlusion[TOP_LEFT]) {
              break;
            }
            PUSH_TRIANGLE(vertex_cursor, xyz, XyZ, xyZ, MAGENTA);
            break;

          case POS_NEG_POS_CORNER:
          case POS_POS_DIAGONAL:
            if (block_side_occlusion[TOP_RIGHT]) {
              break;
            }
            PUSH_TRIANGLE(vertex_cursor, Xyz, XyZ, xyZ, MAGENTA);
            break;

          case AIR:
          case POS_X_POS_Y_SLOPE:
          case POS_Z_POS_Y_SLOPE:
          case NEG_Z_POS_Y_SLOPE:
          case NEG_X_POS_Y_SLOPE:
          case NEG_POS_NEG_CORNER:
          case POS_POS_NEG_CORNER:
          case NEG_POS_POS_CORNER:
          case POS_POS_POS_CORNER:
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

          case POS_NEG_NEG_CORNER:
            PUSH_TRIANGLE(vertex_cursor, XYz, xyz, XyZ, YELLOW);
            break;
          case NEG_NEG_NEG_CORNER:
            PUSH_TRIANGLE(vertex_cursor, xYz, xyZ, Xyz, YELLOW);
            break;
          case NEG_NEG_POS_CORNER:
            PUSH_TRIANGLE(vertex_cursor, xYZ, XyZ, xyz, YELLOW);
            break;
          case POS_NEG_POS_CORNER:
            PUSH_TRIANGLE(vertex_cursor, XYZ, Xyz, xyZ, YELLOW);
            break;
          case POS_POS_NEG_CORNER:
            PUSH_TRIANGLE(vertex_cursor, Xyz, XYZ, xYz, YELLOW);
            break;
          case NEG_POS_NEG_CORNER:
            PUSH_TRIANGLE(vertex_cursor, xyz, XYz, xYZ, YELLOW);
            break;
          case NEG_POS_POS_CORNER:
            PUSH_TRIANGLE(vertex_cursor, xyZ, xYz, XYZ, YELLOW);
            break;
          case POS_POS_POS_CORNER:
            PUSH_TRIANGLE(vertex_cursor, XyZ, xYZ, XYz, YELLOW);
            break;

          case AIR:
          case CUBE:
          case POS_NEG_DIAGONAL:
            PUSH_SQUARE(vertex_cursor, XyZ, XYZ, xYz, xyz, YELLOW);
            break;
          case NEG_NEG_DIAGONAL:
            PUSH_SQUARE(vertex_cursor, Xyz, XYz, xYZ, xyZ, YELLOW);
            break;
          case NEG_POS_DIAGONAL:
            PUSH_SQUARE(vertex_cursor, xyz, xYz, XYZ, XyZ, YELLOW);
            break;
          case POS_POS_DIAGONAL:
            PUSH_SQUARE(vertex_cursor, xyZ, xYZ, XYz, Xyz, YELLOW);
          case BLOCK_SHAPE_COUNT:
            break;
        }
      }
    }
  }
  
  ptrdiff_t vertex_count = vertex_cursor - vertices;
  printf("Vertex count: %ld\n", vertex_count);

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

  free(vertices);

  clock_gettime(CLOCK_MONOTONIC_RAW, &end_time);
  delta = (end_time.tv_sec - start_time.tv_sec) + (end_time.tv_nsec - start_time.tv_nsec) / 1000000000.0f;
  printf("Chunk Render data generation took %.4f seconds.\n", delta);
}


void initSphereChunk(Chunk* c) {
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
