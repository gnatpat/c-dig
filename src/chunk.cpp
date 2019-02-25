inline BlockShape getBlockShapeAt(LoadedWorld* w, Chunk* c, int x, int y, int z) {
  return getBlockAt(w, c->origin + v3i(x, y, z)).block_shape;
}

void fillChunkRenderData(Chunk* chunk, LoadedWorld* world) {
  //struct timespec start_time, end_time;
  //float delta;
  //clock_gettime(CLOCK_MONOTONIC_RAW, &start_time);
  //printf("Creating render data for chunk (%d, %d, %d)\n", chunk->origin.x, chunk->origin.y, chunk->origin.z);
  //printf("Allocating space for %d vertices.\n", CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE * 6 * 6);
  ChunkVertex* vertices = (ChunkVertex*)malloc(CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE * 6 * 6 * sizeof(ChunkVertex));
  ChunkVertex* vertex_cursor = vertices;

  for(int x = 0; x < CHUNK_SIZE; x++) {
    for(int y = 0; y < CHUNK_SIZE; y++) {
      for(int z = 0; z < CHUNK_SIZE; z++) {
        BlockShape block_shape = chunk->blocks[x][y][z].block_shape;
        pushBlockVertices(&vertex_cursor, chunk, world, x, y, z, block_shape);
      }
    }
  }
  
  ptrdiff_t vertex_count = vertex_cursor - vertices;
  //printf("Vertex count: %ld\n", vertex_count);

  // TODO: we're using both vertex_count and num_vertices. Use one or the other.
  chunk->render_data.num_vertices = vertex_count;
  chunk->render_data.vertices = vertices;

  chunk->render_data.state = NOT_PASSED_TO_OPENGL;

  //clock_gettime(CLOCK_MONOTONIC_RAW, &end_time);
  //delta = (end_time.tv_sec - start_time.tv_sec) + (end_time.tv_nsec - start_time.tv_nsec) / 1000000000.0f;
  //printf("Chunk Render data generation took %.4f seconds.\n", delta);
}

void fillChunkVao(Chunk* chunk) {
  assert(chunk->render_data.state == NOT_PASSED_TO_OPENGL);

  int vertex_count = chunk->render_data.num_vertices;

  GLuint vao, vbo;
  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);
  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, vertex_count * sizeof(ChunkVertex), chunk->render_data.vertices, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ChunkVertex), (void*)0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(ChunkVertex), (void*)(3 * sizeof(float)));
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(ChunkVertex), (void*)(6 * sizeof(float)));
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glEnableVertexAttribArray(2);

  chunk->render_data.vao = vao;
  chunk->render_data.state = OKAY;

  glBindVertexArray(0);
  glDeleteBuffers(1, &vbo);

  free(chunk->render_data.vertices);
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
  c->blocks[0][2][1].block_shape = NEG_X_POS_Y_SLOPE;
  c->blocks[2][2][1].block_shape = POS_X_POS_Y_SLOPE;
  c->blocks[1][2][0].block_shape = NEG_Z_POS_Y_SLOPE;
  c->blocks[1][2][2].block_shape = POS_Z_POS_Y_SLOPE;
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
  c->blocks[0][0][1].block_shape = NEG_X_NEG_Y_SLOPE;
  c->blocks[2][0][1].block_shape = POS_X_NEG_Y_SLOPE;
  c->blocks[1][0][0].block_shape = NEG_Z_NEG_Y_SLOPE;
  c->blocks[1][0][2].block_shape = POS_Z_NEG_Y_SLOPE;
  c->blocks[2][0][2].block_shape = NEG_POS_NEG_CORNER;
  c->blocks[0][0][2].block_shape = POS_POS_NEG_CORNER;
  c->blocks[0][0][0].block_shape = POS_POS_POS_CORNER;
  c->blocks[2][0][0].block_shape = NEG_POS_POS_CORNER;
}

float AMPLITUDE = 0.1;
float FREQUENCY = 0.01;

void initChunk(Chunk* c) {
  int corner_heights[CHUNK_SIZE+1][CHUNK_SIZE+1];
  for(int x = 0; x < CHUNK_SIZE+1; x++) {
    for(int z = 0; z < CHUNK_SIZE+1; z++) {
        V2 world_xz = v2(c->origin.x + x, c->origin.z + z);
        int offset = (LOADED_WORLD_SIZE * CHUNK_SIZE) / 2;
        int height = (int)(perlin(world_xz, FREQUENCY) * offset * AMPLITUDE + offset);
        corner_heights[x][z] = height;
    }
  }

  for(int x = 0; x < CHUNK_SIZE; x++) {
    for(int z = 0; z < CHUNK_SIZE; z++) {
      int xz = corner_heights[x][z];
      int Xz = corner_heights[x+1][z];
      int xZ = corner_heights[x][z+1];
      int XZ = corner_heights[x+1][z+1];
      int midheight = (xz + xZ + Xz + XZ) / 4;
      BlockShape topper;
      BlockShape smoother = CUBE;
      if (xz == Xz && Xz == xZ && xZ == XZ) {
        topper = AIR;
      } else if (xz <= midheight && xZ <= midheight && XZ > midheight && Xz > midheight) {
        topper = NEG_X_POS_Y_SLOPE;
      } else if (xz > midheight && xZ <= midheight && XZ <= midheight && Xz > midheight) {
        topper = POS_Z_POS_Y_SLOPE;
      } else if (xz > midheight && xZ > midheight && XZ <= midheight && Xz <= midheight) {
        topper = POS_X_POS_Y_SLOPE;
      } else if (xz <= midheight && xZ > midheight && XZ > midheight && Xz <= midheight) {
        topper = NEG_Z_POS_Y_SLOPE;
      } else if (xz <= midheight && xZ > midheight && XZ > midheight && Xz > midheight) {
        topper = NEG_POS_NEG_CORNERLESS;
      } else if (xz > midheight && xZ <= midheight && XZ > midheight && Xz > midheight) {
        topper = NEG_POS_POS_CORNERLESS;
      } else if (xz > midheight && xZ > midheight && XZ <= midheight && Xz > midheight) {
        topper = POS_POS_POS_CORNERLESS;
      } else if (xz > midheight && xZ > midheight && XZ > midheight && Xz <= midheight) {
        topper = POS_POS_NEG_CORNERLESS;
      } else if (xz > midheight && xZ <= midheight && XZ <= midheight && Xz <= midheight) {
        topper = NEG_NEG_NEG_CORNER;
        if(XZ < midheight) {
          smoother = POS_POS_POS_CORNERLESS;
        }
      } else if (xz <= midheight && xZ > midheight && XZ <= midheight && Xz <= midheight) {
        topper = NEG_NEG_POS_CORNER;
        if(Xz < midheight) {
          smoother = POS_POS_NEG_CORNERLESS;
        }
      } else if (xz <= midheight && xZ <= midheight && XZ > midheight && Xz <= midheight) {
        topper = POS_NEG_POS_CORNER;
        if(xz < midheight) {
          smoother = NEG_POS_NEG_CORNERLESS;
        }
      } else if (xz <= midheight && xZ <= midheight && XZ <= midheight && Xz > midheight) {
        topper = POS_NEG_NEG_CORNER;
        if (xZ < midheight) {
          smoother = NEG_POS_POS_CORNERLESS;
        }
      } else {
        topper = AIR;
      }
      int block_relative_midheight = midheight - c->origin.y;
      for(int y = 0; y < CHUNK_SIZE; y++) {
        c->blocks[x][y][z].block_shape = y < block_relative_midheight ? CUBE : AIR;
      }
      if(block_relative_midheight >= 0 && block_relative_midheight < CHUNK_SIZE) {
        c->blocks[x][block_relative_midheight][z].block_shape = topper;
      }
      if(block_relative_midheight-1 >= 0 && block_relative_midheight-1 < CHUNK_SIZE) {
        c->blocks[x][block_relative_midheight-1][z].block_shape = smoother;
      }
    }
  }
}

void clearChunk(Chunk* c) {
  for (int x = 0; x < CHUNK_SIZE; x++) {
    for (int y = 0; y < CHUNK_SIZE; y++) {
      for (int z = 0; z < CHUNK_SIZE; z++) {
        c->blocks[x][y][z].block_shape = AIR;
      }
    }
  }
}
