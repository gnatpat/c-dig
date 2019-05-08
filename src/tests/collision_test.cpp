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

TestResult testLineCollision(void) {
  V3 line_start = v3(0, 0, 0);
  V3 line_end = v3(0, 2, 0);
  V3 sphere_pos = v3(-2, 1, 0);
  V3 velocity = v3(4, 0, 0);

  MaybeCollision collision = lineCollision(line_start, line_end, sphere_pos, velocity);

  CHECK(collision.collided == true);
  CHECK(collision.time == 0.25);
  printV3(collision.collision_point);
  CHECK(collision.collision_point == v3(0, 1, 0));
}

TestResult testLineCollisionWithLineTheOtherWay(void) {
  V3 line_end = v3(0, 0, 0);
  V3 line_start = v3(0, 2, 0);
  V3 sphere_pos = v3(-2, 1, 0);
  V3 velocity = v3(4, 0, 0);

  MaybeCollision collision = lineCollision(line_start, line_end, sphere_pos, velocity);

  CHECK(collision.collided == true);
  CHECK(collision.time == 0.25);
  printV3(collision.collision_point);
  CHECK(collision.collision_point == v3(0, 1, 0));
}

TestResult testLineCollisionWithLongerLine(void) {
  V3 line_start = v3(0, 0, 0);
  V3 line_end = v3(0, 4, 0);
  V3 sphere_pos = v3(-2, 1, 0);
  V3 velocity = v3(8, 0, 0);

  MaybeCollision collision = lineCollision(line_start, line_end, sphere_pos, velocity);

  CHECK(collision.collided == true);
  printV3(collision.collision_point);
  CHECK(collision.collision_point == v3(0, 1, 0));
  CHECK(collision.time == 0.125);
}

