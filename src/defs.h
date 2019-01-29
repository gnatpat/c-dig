#pragma GCC diagnostic ignored "-Wpedantic"

// TODO: move these into some contstants object?
int SCREEN_WIDTH = 800;
int SCREEN_HEIGHT = 600;

const int CHUNK_SIZE = 16;
const int LOADED_WORLD_SIZE = 8;

union Matrix4x4 {
  float seq[16];
  // this is v[row][column] (row major?) (so v[y][x])
  float v[4][4];
};

union V3 {
  struct {
    float x;
    float y;
    float z;
  };
  float v[3];
};

union V3i {
  struct {
    int x;
    int y;
    int z;
  };
  int v[3];
};

union Quaternion {
  struct {
    float w;
    float x;
    float y;
    float z;
  };
  float v[4];
};

struct Rectangle {
  float left;
  float right;
  float bottom;
  float top;
};

struct Sprite {
  V3 position;
  int index;
};

enum Direction {
  POS_Z,
  POS_X,
  NEG_Z,
  NEG_X,
  POS_Y,
  NEG_Y,

  DIRECTION_COUNT
};

enum XZDirection {
  XZ_POS_Z,
  XZ_POS_X,
  XZ_NEG_Z,
  XZ_NEG_X,

  XZ_DIRECTION_COUNT
};

//enum BlockType {
//  AIR,
//  CUBE,
//  SLOPE,
//  CORNER_SLOPE,
//
//  BLOCK_TYPE_COUNT
//};
//
//union BlockDirection {
//  struct {
//    bool x;
//    bool y;
//    bool z;
//  } corner_direction;
//  struct {
//    XZDirection facing;
//    bool sloping_down;
//  } slope_direction;
//};

enum BlockShape {
  AIR,
  CUBE,
  POS_Z_NEG_Y_SLOPE,
  POS_X_NEG_Y_SLOPE,
  NEG_Z_NEG_Y_SLOPE,
  NEG_X_NEG_Y_SLOPE,
  POS_Z_POS_Y_SLOPE,
  POS_X_POS_Y_SLOPE,
  NEG_Z_POS_Y_SLOPE,
  NEG_X_POS_Y_SLOPE,
  POS_NEG_NEG_CORNER,
  NEG_NEG_NEG_CORNER,
  NEG_NEG_POS_CORNER,
  POS_NEG_POS_CORNER,
  POS_POS_NEG_CORNER,
  NEG_POS_NEG_CORNER,
  NEG_POS_POS_CORNER,
  POS_POS_POS_CORNER,
  POS_NEG_DIAGONAL,
  NEG_NEG_DIAGONAL,
  NEG_POS_DIAGONAL,
  POS_POS_DIAGONAL,

  BLOCK_SHAPE_COUNT
};

enum FaceShape {
  FULL,
  TOP_RIGHT,
  TOP_LEFT,
  BOTTOM_LEFT,
  BOTTOM_RIGHT
};

struct ChunkVertex {
  V3 vertex;
  V3 colour;
};

struct Block {
  // TODO(nathan): should all of these be here? They're all representations of the block shape. Who knows.
  //BlockType block_type;
  //BlockDirection block_direction;
  BlockShape block_shape;
};

struct ChunkRenderData {
  GLuint vao;
  int num_vertices;
};

struct Chunk {
  Block blocks[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE];
  ChunkRenderData render_data;
  V3i origin;
};

struct LoadedWorld {
  Chunk chunks[LOADED_WORLD_SIZE][LOADED_WORLD_SIZE][LOADED_WORLD_SIZE];
  V3i origin;
};

struct GameData {
  LoadedWorld loaded_world;
};

// Generated from py/gen_face_bitfields.py
bool BLOCK_SIDE_OCCLUSION_BITFIELD[] = {false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, false, false, false, false, false, false, false, false, true, false, true, true, true, true, true, false, false, false, false, true, false, false, false, false, false, true, true, true, true, true, false, false, false, false, true, false, false, false, false, false, false, false, false, true, false, true, true, true, true, true, false, false, false, false, false, true, true, true, true, true, true, true, true, true, true, false, false, false, false, true, false, false, false, false, false, false, false, false, true, false, false, false, false, false, false, true, true, true, true, true, false, false, false, true, false, true, true, true, true, true, false, false, false, false, true, false, false, false, false, false, false, false, false, false, false, true, true, true, true, true, false, false, false, false, false, false, false, true, false, false, true, true, true, true, true, false, true, false, false, false, true, true, true, true, true, false, false, false, false, false, false, true, false, false, false, false, false, false, false, false, false, false, true, false, false, true, true, true, true, true, true, true, true, true, true, false, false, false, false, false, true, true, true, true, true, false, true, false, false, false, false, false, false, false, false, false, false, true, false, false, true, true, true, true, true, false, false, false, false, false, false, false, true, false, false, true, true, true, true, true, false, true, false, false, false, false, false, false, false, false, true, true, true, true, true, false, false, false, false, false, false, false, false, false, false, false, false, false, true, false, false, false, false, false, true, false, false, false, false, false, false, false, false, false, false, false, true, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, true, false, false, false, false, false, true, false, false, false, false, false, false, false, true, false, false, false, false, false, false, true, false, false, false, false, false, false, false, false, false, false, false, false, false, true, false, false, false, false, false, false, false, false, false, true, false, false, false, false, true, false, false, false, false, false, true, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, true, false, false, false, false, false, false, false, true, false, false, false, true, false, false, false, false, false, false, false, false, false, false, false, false, true, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, true, false, false, false, true, false, false, false, false, false, false, true, false, false, false, false, false, false, false, true, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, true, false, false, false, false, true, false, false, false, false, false, false, false, false, false, true, false, false, false, true, false, false, false, false, false, false, false, false, false, false, false, false, false, false, true, false, false, false, false, false, false, false, false, false, false, false, false, false, true, true, true, true, true, true, true, true, true, true, false, false, false, false, false, false, false, false, false, true, false, true, false, false, false, false, false, false, false, false, false, false, false, false, false, true, true, true, true, true, true, true, true, true, true, false, false, false, true, false, false, false, true, false, false, true, true, true, true, true, false, false, false, false, false, false, false, false, false, false, true, true, true, true, true, false, false, true, false, false, false, false, false, true, false, true, true, true, true, true, true, true, true, true, true, false, false, false, false, false, false, false, false, false, false, false, true, false, false, false, false, false, false, false, true};
#pragma GCC diagnostic ignored "-Wpedantic"
