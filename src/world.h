void initWorld(LoadedWorld* world);
Block getBlockAt(LoadedWorld* world, V3i pos);
void renderWorld(LoadedWorld* loaded_world, GLuint terrain_shader, Matrix4x4* view, Matrix4x4* projection);
void shiftLoadedWorld(LoadedWorld* loaded_world, Direction direction);

inline Chunk* getChunkRelativeToLoadedWorld(LoadedWorld* world, V3i pos) {
  pos += world->wrap_break;
  pos %= LOADED_WORLD_SIZE;
  return &world->chunks[pos.x][pos.y][pos.z];
}
