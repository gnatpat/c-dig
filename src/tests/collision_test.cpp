TestResult testVertexCollision(void) {
  V3 vertex = v3(10, 20, 30);
  V3 sphere_pos = v3(10, 20, 28);
  V3 velocity = v3(0, 0, 4);

  MaybeCollision collision = vertexCollision(vertex, sphere_pos, velocity);

  CHECK(collision.collided == true);
  CHECK(collision.time == 0.25);
  CHECK(collision.collision_point == vertex);

  SUCCESS;
}

TestResult testNoVertexCollision(void) {
  V3 vertex = v3(0, 0, 0);
  V3 sphere_pos = v3(10, 10, 10);
  V3 velocity = v3(0, 0, 1);

  MaybeCollision collision = vertexCollision(vertex, sphere_pos, velocity);

  CHECK(collision.collided == false);
  SUCCESS;
}
