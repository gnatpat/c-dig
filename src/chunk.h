// TODO(nathan): I'm not sure how I feel about macros. These feel like they should be functions.
// Also having these here is necessary for the code generation, should revist this.
#define PUSH_VERTEX(vertex_cursor, vertex, colour) *(vertex_cursor) = { vertex, colour }; (vertex_cursor)++;
#define PUSH_SQUARE(cursor, v1, v2, v3, v4, colour) \
        PUSH_VERTEX(cursor, v1, colour); PUSH_VERTEX(cursor, v2, colour); PUSH_VERTEX(cursor, v3, colour); \
        PUSH_VERTEX(cursor, v1, colour); PUSH_VERTEX(cursor, v3, colour); PUSH_VERTEX(cursor, v4, colour); 
#define PUSH_TRIANGLE(cursor, v1, v2, v3, colour) \
        PUSH_VERTEX(cursor, v1, colour); PUSH_VERTEX(cursor, v2, colour); PUSH_VERTEX(cursor, v3, colour);


void fillChunkRenderData(Chunk* chunk, LoadedWorld* world);
void initSphereChunk(Chunk* c);
void initChunk(Chunk* c);
void clearChunk(Chunk* c);
void fillChunkVao(Chunk* chunk);

inline Block getBlockAt(Chunk* c, V3i v) {
  return c->blocks[v.x][v.y][v.z];
}

