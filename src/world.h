void initWorld(LoadedWorld* world);
Block getBlockAt(LoadedWorld* world, V3i pos);

inline Chunk* getChunkAt(LoadedWorld* world, V3i pos) {
  return &world->chunks[pos.x][pos.y][pos.z];
}
