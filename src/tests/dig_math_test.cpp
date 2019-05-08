
TestResult testSignedDistanceFromXyPlane(void) {
  Triangle t;
  t.vertices[0] = v3(0, 0, 0);
  t.vertices[1] = v3(4, 0, 0);
  t.vertices[2] = v3(0, 4, 0);
  t.normal = v3(0, 0, 1);

  V3 pos = v3(0, 0, 15);
  CHECK(signedDistanceFromTrianglePlane(t, pos) == 15.0);

  pos = v3(0, 0, -15);
  CHECK(signedDistanceFromTrianglePlane(t, pos) == -15.0);

  t.vertices[0] = v3(0, 0, 42);
  t.vertices[1] = v3(4, 0, 0);
  t.vertices[2] = v3(0, 4, 0);
  t.normal = v3(0, 0, 1);

  pos = v3(0, 0, 43);
  CHECK(signedDistanceFromTrianglePlane(t, pos) == 1.0);

  pos = v3(0, 0, 22);
  CHECK(signedDistanceFromTrianglePlane(t, pos) == -20.0);
  SUCCESS
}

TestResult testIsPointInRightAngleTriangle(void) {
  Triangle t;
  t.vertices[0] = v3(0, 0, 0);
  t.vertices[1] = v3(4, 0, 0);
  t.vertices[2] = v3(0, 4, 0);
  t.normal = v3(0, 0, 1);
  
  V3 pos = v3(1, 1, 0);
  CHECK(isPointInTriangle(t, pos));

  pos = v3(3, 2, 0);
  CHECK(!isPointInTriangle(t, pos));

  pos = v3(1, 1, 1);
  CHECK(isPointInTriangle(t, pos));

  SUCCESS
}

TestResult testIsPointOnEdge(void) {
  Triangle t;
  t.vertices[0] = v3(1, 1, 1);
  t.vertices[1] = v3(2, 1, 2);
  t.vertices[2] = v3(1, 2, 2);
  t.normal = normalise(v3(-1, 1, 1));
  
  V3 pos = v3(1, 1, 1);
  CHECK(isPointInTriangle(t, pos));

  SUCCESS
}

TestResult testPointIsOnAcuteTriangle() {
  Triangle t;
  t.vertices[0] = v3(0, 0, 0);
  t.vertices[1] = v3(4, 0, 0);
  t.vertices[2] = v3(2, 3, 0);
  t.normal = v3(0, 0, 1);
  
  V3 pos = v3(1, 1, 0);
  CHECK(isPointInTriangle(t, pos));

  pos = v3(3, 2, 0);
  CHECK(!isPointInTriangle(t, pos));

  SUCCESS
}
