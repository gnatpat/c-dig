void initWorld(LoadedWorld* world);
Block getBlockAt(LoadedWorld* world, V3i pos);
MaybeV3i getFirstNonAirBlockPosition(LoadedWorld* world, V3 from, V3 direction, float max_distance);
void renderWorld(LoadedWorld* loaded_world, GLuint terrain_shader, Matrix4x4* view, Matrix4x4* projection);
void shiftLoadedWorld(LoadedWorld* loaded_world, Direction direction);
bool isPointAir(LoadedWorld* world, V3 pos);
int getMeshAroundPosition(Triangle* mesh_buffer, LoadedWorld* world, V3i from, V3i to);
void markChunkAsDirty(LoadedWorld* world, Chunk* c);
void putBlockAt(LoadedWorld* world, V3i pos, Block block);

inline Chunk* getChunkRelativeToLoadedWorld(LoadedWorld* world, V3i pos) {
  pos += world->wrap_break;
  pos %= LOADED_WORLD_SIZE;
  return &world->chunks[pos.x][pos.y][pos.z];
}
