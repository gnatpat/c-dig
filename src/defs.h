#pragma GCC diagnostic ignored "-Wpedantic"

const int CHUNK_SIZE = 16;

union Matrix4x4 {
  float seq[16];
  // this is v[row][column] (row major?)
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
  POS_X,
  POS_Y,
  POS_Z,
  NEG_X,
  NEG_Y,
  NEG_Z,

  DIRECTION_COUNT
};

enum XZDirection {
  XZ_POS_Z,
  XZ_POS_X,
  XZ_NEG_Z,
  XZ_NEG_X,

  XZ_DIRECTION_COUNT
};


// Oriented towards the triangle, with y-axis positive being up if possible, else x axis pointing right.
enum TriangleRotation {
  TOP_RIGHT,
  TOP_LEFT,
  BOTTOM_LEFT,
  BOTTOM_RIGHT,

  TRIANGLE_ROTATION_COUNT
};

enum FaceType {
  EMPTY,
  TRIANGLE,
  SQUARE
};

struct Face {
  FaceType type;
  TriangleRotation triangle_rotation;
  V3 vertices[DIRECTION_COUNT][4];
};

enum BlockType {
  AIR,
  CUBE,
  SLOPE,
  CORNER_SLOPE,

  BLOCK_TYPE_COUNT
};

union BlockDirection {
  struct {
    bool x;
    bool y;
    bool z;
  } corner_direction;
  struct {
    XZDirection facing;
    bool sloping_down;
  } slope_direction;
};

struct BlockShapeFlyweight {
  BlockType block_type;
  BlockDirection block_direction;
  Face* side_faces[DIRECTION_COUNT];
  V3 slope_vertices[4];
}; 

struct Block {
  BlockShapeFlyweight* block_shape;
  V3 verticies[24];
};

struct ChunkRenderData {
  GLuint vao;
};

struct Chunk {
  Block blocks[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE];
  ChunkRenderData render_data;
};

struct Faces {
  Face empty_face;
  Face square_face;
  Face triangle_faces[TRIANGLE_ROTATION_COUNT];
};

struct BlockShapeFlyweights {
  BlockShapeFlyweight empty;
  BlockShapeFlyweight cube;
  BlockShapeFlyweight slopes[XZ_DIRECTION_COUNT][2];
  BlockShapeFlyweight corner_slopes[2][2][2];
};

struct Constants {
  BlockShapeFlyweights block_shape_flyweights;
  Faces faces;
  GLuint chunk_element_array_buffer;
};

struct GameData {
  Constants constants;
  Chunk chunk;
};

#pragma GCC diagnostic ignored "-Wpedantic"
