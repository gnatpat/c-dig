struct MaybeCollision {
  bool collided;
  float time;
  V3 collision_point;
};

MaybeCollision vertexCollision(V3 vertex, V3 sphere_pos, V3 velocity);
MaybeCollision lineCollision(V3 line_start, V3 line_end, V3 sphere_pos, V3 velocity);
MaybeCollision earliestCollision(MaybeCollision r1, MaybeCollision r2);
