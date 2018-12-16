#pragma once
#pragma GCC diagnostic ignored "-Wpedantic"

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

#pragma GCC diagnostic ignored "-Wpedantic"
