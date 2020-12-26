void fillChunkRenderData(Chunk* chunk, LoadedWorld* world);
void initSphereChunk(Chunk* c);
void initChunk(Chunk* c);
void clearChunk(Chunk* c);
void fillChunkVao(Chunk* chunk);
Minecart* putMinecartAt(Chunk* c, V3i v);
MinecartTrack* putMinecartTrackAt(Chunk* c, V3i v, V3i world_pos);
MinecartTrack* getMinecartTrackAt(Chunk* c, V3i v, V3i world_pos);

inline Block getBlockAt(Chunk* c, V3i v) {
  return c->blocks[v.x][v.y][v.z];
}

inline void putBlockAt(Chunk* c, V3i v, Block block) {
  c->blocks[v.x][v.y][v.z] = block;
}


