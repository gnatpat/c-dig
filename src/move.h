struct MoveResult {
  V3 end_pos;
  bool on_ground;
  MaybeCollision collision;
};

MoveResult move(V3 position, V3 velocity, Triangle* triangles, int triangle_count);

