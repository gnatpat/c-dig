// Note: this file is not generated.
inline void pushVertex(ChunkVertex** vertex_cursor, V3 vertex, V3 colour, V3 normal) {
  **vertex_cursor = { vertex, colour, normal };
  (*vertex_cursor)++;
}

inline void pushTriangle(ChunkVertex** vertex_cursor, V3 v1, V3 v2, V3 v3, V3 colour, V3 normal) {
  pushVertex(vertex_cursor, v1, colour, normal);
  pushVertex(vertex_cursor, v2, colour, normal);
  pushVertex(vertex_cursor, v3, colour, normal);
}

inline void pushSquare(ChunkVertex** vertex_cursor, V3 v1, V3 v2, V3 v3, V3 v4, V3 colour, V3 normal) {
  pushTriangle(vertex_cursor, v1, v2, v3, colour, normal);
  pushTriangle(vertex_cursor, v1, v3, v4, colour, normal);
}

void pushBlockVertices(ChunkVertex **vertex_cursor, Chunk *chunk, LoadedWorld *world, int x, int y, int z, BlockShape block_shape);
inline bool* getBlockSideOcclusion(BlockShape block_shape, Direction side);
