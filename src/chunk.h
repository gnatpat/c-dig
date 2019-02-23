void fillChunkRenderData(Chunk* chunk, LoadedWorld* world);
void initSphereChunk(Chunk* c);
void initChunk(Chunk* c);
void clearChunk(Chunk* c);
void fillChunkVao(Chunk* chunk);

inline Block getBlockAt(Chunk* c, V3i v) {
  return c->blocks[v.x][v.y][v.z];
}

