
TestResult testIsPointInRightAngleTriangle(void) {
  Triangle t;
  t.vertices[0] = v3(0, 0, 0);
  t.vertices[1] = v3(1, 0, 0);
  t.vertices[2] = v3(0, 1, 0);
  t.normal = v3(0, 0, 1);
  
  V3 pos = v3(0.25, 0.25, 0);
  CHECK(isPointInTriangle(t, pos));

  pos = v3(0.75, 0.5, 0);
  CHECK(!isPointInTriangle(t, pos));

  pos = v3(0.25, 0.25, 0.1);
  CHECK(!isPointInTriangle(t, pos));

  SUCCESS
}

TestResult testIsPointInAcuteTriangle(void) {
  Triangle t;
  t.vertices[0] = v3(1, 1, 1);
  t.vertices[1] = v3(2, 1, 2);
  t.vertices[2] = v3(1, 2, 2);
  t.normal = normalise(v3(-1, 1, 1));
  
  V3 pos = v3(1, 1, 1);
  CHECK(isPointInTriangle(t, pos));

  SUCCESS
}
