void fillChunkRenderData(Chunk* chunk);
void initSphereChunk(Chunk* c);
void initChunk(Chunk* c, BlockShape shape);

inline Block getBlockAt(Chunk* c, V3i v) {
  return c->blocks[v.x][v.y][v.z];
}
