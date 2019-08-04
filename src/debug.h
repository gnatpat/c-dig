//#define MOVEMENT_DEBUG
void initDebugData(DebugData* debug_data);
void debugMeshAroundPlayer(BasicRenderObject* debug_triangles, GameData* game_data);
void debugCollision(BasicRenderObject* debug_triangles, GameData* game_data);
void renderDebug(DebugData* debug_data, GLuint shader, Matrix4x4 view, Matrix4x4 projection);
