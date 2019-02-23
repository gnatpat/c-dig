#pragma GCC diagnostic ignored "-Wpedantic"

typedef pthread_t Thread;
typedef pthread_mutex_t Mutex;
typedef pthread_cond_t MutexCondition;

// TODO: move these into some contstants object?
int SCREEN_WIDTH = 800;
int SCREEN_HEIGHT = 600;

const int CHUNK_SIZE = 16;
const int LOADED_WORLD_SIZE = 16;

const float SQRT2 = 1.4142136;
const float ONE_OVER_SQRT2 = 1.0 / SQRT2;

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

union V2 {
  struct {
    float x;
    float y;
  };
  float v[2];
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

struct LinkedList {
  void* content;
  LinkedList* next;
};

// TODO - actually use this LockedLinkedList
struct LockedLinkedList {
  LinkedList* list;
  Mutex lock = PTHREAD_MUTEX_INITIALIZER;
  MutexCondition new_element_condition = PTHREAD_COND_INITIALIZER;
};


enum InputKey {
  PLAYER_MOVE_FORWARD_KEY,
  PLAYER_MOVE_BACKWARD_KEY,
  PLAYER_MOVE_LEFT_KEY,
  PLAYER_MOVE_RIGHT_KEY,
  SWITCH_TO_BLOCK_VIEWER_KEY,
  QUIT_KEY,

  INPUT_KEY_COUNT
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
  POS_Z_POS_Y_SLOPE,
  POS_X_POS_Y_SLOPE,
  NEG_Z_POS_Y_SLOPE,
  NEG_X_POS_Y_SLOPE,
  POS_Z_NEG_Y_SLOPE,
  POS_X_NEG_Y_SLOPE,
  NEG_Z_NEG_Y_SLOPE,
  NEG_X_NEG_Y_SLOPE,
  POS_NEG_NEG_CORNER,
  NEG_NEG_NEG_CORNER,
  NEG_NEG_POS_CORNER,
  POS_NEG_POS_CORNER,
  POS_POS_NEG_CORNER,
  NEG_POS_NEG_CORNER,
  NEG_POS_POS_CORNER,
  POS_POS_POS_CORNER,
  POS_POS_NEG_CORNERLESS,
  NEG_POS_NEG_CORNERLESS,
  NEG_POS_POS_CORNERLESS,
  POS_POS_POS_CORNERLESS,
  POS_NEG_NEG_CORNERLESS,
  NEG_NEG_NEG_CORNERLESS,
  NEG_NEG_POS_CORNERLESS,
  POS_NEG_POS_CORNERLESS,
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

enum ChunkRenderDataState {
  NO_RENDER_DATA,
  NOT_PASSED_TO_OPENGL,
  OKAY,

  CHUNK_RENDER_DATA_STATE_COUNT
};

struct ChunkRenderData {
  ChunkRenderDataState state;
  // num_vertices should be set unless state is NO_RENDER_DATA
  int num_vertices;
  // Render data should only exist here when state is NOT_PASSED_TO_OPENGL
  ChunkVertex* vertices;
  // VAO should only exist when state is OKAY
  GLuint vao;
};

struct Chunk {
  Block blocks[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE];
  ChunkRenderData render_data;
  V3i origin;
};

// TODO - here, State is a struct, but earlier it's an Enum. Not sure how much I like that.
struct WorldRenderState {
  LinkedList* dirty_chunks;
  LinkedList* new_chunks;
  Mutex new_chunk_lock = PTHREAD_MUTEX_INITIALIZER;
  MutexCondition new_chunk_condition = PTHREAD_COND_INITIALIZER;
};

struct LoadedWorld {
  Chunk chunks[LOADED_WORLD_SIZE][LOADED_WORLD_SIZE][LOADED_WORLD_SIZE];
  V3i origin;
  WorldRenderState render_state;
};

struct Player {
  V3 position;
  float pitch;
  float yaw;
  V3 facing;
};

struct GameData {
  Player player;
  LoadedWorld loaded_world;
};

struct BlockViewerData {
  Chunk* c;
  ChunkRenderData occlusion_render_data;
  BlockShape block_shape;
  LoadedWorld empty_world;
};

#pragma GCC diagnostic ignored "-Wpedantic"
