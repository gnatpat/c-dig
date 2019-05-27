//#define MOVEMENT_DEBUG

void initDebugData(DebugData* debug_data);
void initDebugTriangles(DebugTriangles* debug_triangles, int size);
void debugMeshAroundPlayer(DebugTriangles* debug_triangles, GameData* game_data);
void renderDebug(DebugData* debug_data, GLuint shader, Matrix4x4 view, Matrix4x4 projection);
void fillDebugTriangleRenderData(DebugTriangles* debug_triangles);
void renderDebugTriangles(DebugTriangles* debug_triangles, GLuint shader, Matrix4x4 view, Matrix4x4 projection, V3 col);
