void initWorld(LoadedWorld* world);
Block getBlockAt(LoadedWorld* world, V3i pos);
MaybeRayTraceResult blockRayTrace(LoadedWorld* world, V3 from, V3 direction, float distance);
MaybeRayTraceResult getClosestResult(MaybeRayTraceResult r1, MaybeRayTraceResult r2);
void renderWorld(GameData* game_data, GLuint terrain_shader, GLuint debugShader, Matrix4x4 view, Matrix4x4 projection);
void shiftWorldBasedOnPositionIfNessecary(LoadedWorld* loaded_world, V3 position);
bool isPointSolid(LoadedWorld* world, V3 pos);
int getMeshAroundPosition(Triangle* mesh_buffer, LoadedWorld* world, V3i from, V3i to);
void markChunkAsDirty(LoadedWorld* world, Chunk* c);
void putBlockAt(LoadedWorld* world, V3i pos, Block block);
void putMinecartTrackAt(LoadedWorld* world, V3i pos);
MinecartTrack* getMinecartTrackAt(LoadedWorld* world, V3i pos);

inline Chunk* getChunkRelativeToLoadedWorld(LoadedWorld* world, V3i pos) {
  pos += world->wrap_break;
  pos %= LOADED_WORLD_SIZE;
  return &world->chunks[pos.x][pos.y][pos.z];
}
