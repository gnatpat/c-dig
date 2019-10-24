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
const float SQRT3 = sqrt(3);
const float ONE_OVER_SQRT3 = 1.0 / SQRT3;

const float GRAVITY = 100;
const float MAX_Y_SPEED = 200;
const float PLAYER_WIDTH = 0.8;
const float PLAYER_HEIGHT = 1.8;

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

union V2i {
  struct {
    int x;
    int y;
  };
  int v[2];
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

struct Triangle {
  V3 vertices[3];
  V3 normal;
};

struct Line {
  V3 start;
  V3 end;
};

struct QuadraticSolutions {
  bool solution_exists;
  float min;
  float max;
};

struct BasicRenderObject {
  unsigned int vao;
  unsigned int vbo;
  V3* vertices;
  Triangle* triangles;
  int triangle_count;
  bool dirty;
};

struct DebugData {
  BasicRenderObject mesh_around_player;
  BasicRenderObject collision_triangles;
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

enum CollisionType {
  POINT_COLLISION,
  LINE_COLLISION,
  FACE_COLLISION,

  COLLISION_TYPE_COUNT
};

struct MaybeCollision {
  bool collided;
  float time;
  V3 collision_point;
  CollisionType collision_type;
  union {
    Line line_info;
    Triangle triangle_info;
  };
};

// TODO: I wonder if there's a way to generalize the `Maybe` - typesafe monads in C through macro craziness?
struct MaybeRayTraceResult {
  bool hit;
  V3i block_position;
  V3 hit_position;
  Triangle hit_face;
  float distance;
};


enum InputKey {
  PLAYER_MOVE_FORWARD_KEY,
  PLAYER_MOVE_BACKWARD_KEY,
  PLAYER_MOVE_LEFT_KEY,
  PLAYER_MOVE_RIGHT_KEY,
  PLAYER_JUMP_KEY,
  SWITCH_TO_BLOCK_VIEWER_KEY,
  SWITCH_TO_FLYING_MODE_KEY,
  SWITCH_TO_DEBUG_MODE_KEY,
  QUIT_KEY,

  INPUT_KEY_COUNT
};


struct Sprite {
  V3 position;
  int index;
};

struct BlockModel {
  Triangle mesh[14];
  int triangle_count;
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

enum FaceShape {
  FULL,
  TOP_RIGHT,
  TOP_LEFT,
  BOTTOM_LEFT,
  BOTTOM_RIGHT
};


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

struct ChunkVertex {
  V3 vertex;
  V3 colour;
  V3 normal;
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
  V3i wrap_break;
  WorldRenderState render_state;
};

struct Player {
  V3 position;
  float pitch;
  float yaw;
  V3 facing;
  bool flying;
  V3 speed;
  float jump_timer;
  bool on_ground;
  MaybeCollision latest_collision;
  MaybeRayTraceResult focus;
};

struct Interface {
  BasicRenderObject block_focus;
  BasicRenderObject block_focus_face;
  BasicRenderObject centre_point;
};

struct GameData {
  Player player;
  LoadedWorld loaded_world;
  Interface interface;
};

struct BlockViewerData {
  Chunk* c;
  ChunkRenderData occlusion_render_data;
  BlockShape block_shape;
  LoadedWorld empty_world;
};

struct FontCharData {
  int x;
  int y;
  int width;
  int height;
  int x_offset;
  int y_offset;
  int x_advance;
};

struct FontData {
  int line_height;
  int line_base;
  int texture_width;
  int texture_height;
  FontCharData chars[128];
  int kerning[128][128];
  int padding_left;
  int padding_right;
};

struct Font {
  GLuint texture;
  FontData data;
};

struct Text {
  GLuint vao;
  int length;
  Font* font;
};

#pragma GCC diagnostic error "-Wpedantic"
