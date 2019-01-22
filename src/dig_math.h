/**************\
 * QUATERNION *
\**************/

inline Quaternion operator*(Quaternion q, Quaternion r) {
  Quaternion result;
  result.x = r.w*q.x + r.x*q.w - r.y*q.z + r.z*q.y;
  result.y = r.w*q.y + r.x*q.z + r.y*q.w - r.z*q.x;
  result.z = r.w*q.z - r.x*q.y + r.y*q.x + r.z*q.w;
  result.w = r.w*q.w - r.x*q.x - r.y*q.y - r.z*q.z;
  return result;
}

inline Quaternion operator*(Quaternion q, V3 v) {
  Quaternion r;
  r.x = v.x;
  r.y = v.y;
  r.z = v.z;
  return q * r;
}

inline Quaternion conj(Quaternion q) {
  Quaternion result;
  result.w = q.w;
  result.x = -q.x;
  result.y = -q.y;
  result.z = -q.z;
  return result;
}

inline V3 rotate(Quaternion q, V3 v) {
  Quaternion q_result = (q * v) * conj(q);
  V3 result;
  result.x = q_result.x;
  result.y = q_result.y;
  result.z = q_result.z;
  return result;
}

inline Quaternion from_vector_and_angle(V3 v, float theta) {
  Quaternion result;
  result.w = cos(theta/2);
  result.x = v.x * sin(theta/2);
  result.y = v.y * sin(theta/2);
  result.z = v.z * sin(theta/2);
  return result;
}

/*************\
 * MATRIX4x4 *
\*************/

inline Matrix4x4 identity() {
  Matrix4x4 identity = {};
  identity.v[0][0] = 1.0;
  identity.v[1][1] = 1.0;
  identity.v[2][2] = 1.0;
  identity.v[3][3] = 1.0;

  return identity;
}

Matrix4x4 perspective_projection(float n, float f, float fov, float ratio) {
  float t = tan(fov/2) * n;
  float b = -t;
  float r = t * ratio;
  float l = -r;
  Matrix4x4 m = { { 2*n/(r-l), 0.0,        (r+l)/(r-l), 0.0         ,
                    0.0,       2*n/(t-b),  (t+b)/(t-b), 0.0         ,
                    0.0,       0.0,       -(f+n)/(f-n), -2*f*n/(f-n),
                    0.0,       0.0,       -1.0,         0.0         } };
  return m;
}

void printMatrix4x4(Matrix4x4 m) {
  for(int row = 0; row < 4; row++) {
    for(int col = 0; col < 4; col++) {
      printf("%2.2f ", m.v[row][col]);
    }
    printf("\n");
  }
}

// operators

inline Matrix4x4 operator+(Matrix4x4 a, Matrix4x4 b) {
  Matrix4x4 result;
  for (int i = 0; i < 16; i++) { 
    result.seq[i] = a.seq[i] + b.seq[i];
  }
  return result;
}

inline Matrix4x4 & operator+=(Matrix4x4 &a, Matrix4x4 b) {
  a = a+b;
  return a;
}

inline Matrix4x4 operator-(Matrix4x4 a, Matrix4x4 b) {
  Matrix4x4 result;
  for (int i = 0; i < 16; i++) { 
    result.seq[i] = a.seq[i] - b.seq[i];
  }
  return result;
}

inline Matrix4x4 & operator-=(Matrix4x4 &a, Matrix4x4 b) {
  a = a-b;
  return a;
}

inline Matrix4x4 operator*(Matrix4x4 a, Matrix4x4 b) {
  Matrix4x4 result = {};
  for (int row = 0; row < 4; row++) {
    for (int col = 0; col < 4; col++) {
      for (int i = 0; i < 4; i++) {
        result.v[row][col] += a.v[row][i] * b.v[i][col];
      }
    }
  }
  return result;
}

inline Matrix4x4 & operator*=(Matrix4x4 &a, Matrix4x4 b) {
  a = a * b;
  return a;
}

inline Matrix4x4 operator/(Matrix4x4 a, Matrix4x4 b) {
  Matrix4x4 result;
  for (int i = 0; i < 16; i++) { 
    result.seq[i] = a.seq[i] / b.seq[i];
  }
  return result;
}

inline Matrix4x4 & operator/=(Matrix4x4 &a, Matrix4x4 b) {
  a = a / b;
  return a;
}

// scalar operators

inline Matrix4x4 operator+(Matrix4x4 m, float a) {
  Matrix4x4 result;
  for (int i = 0; i < 16; i++) { 
    result.seq[i] = m.seq[i] + a;
  }
  return result;
}

inline Matrix4x4 operator+(float a, Matrix4x4 m) {
  return m + a;
}

inline Matrix4x4 & operator+=(Matrix4x4 &m, float a) {
  m = m + a;
  return m;
}

inline Matrix4x4 operator-(Matrix4x4 m, float a) {
  Matrix4x4 result;
  for (int i = 0; i < 16; i++) { 
    result.seq[i] = m.seq[i] - a;
  }
  return result;
}

inline Matrix4x4 & operator-=(Matrix4x4 &m, float a) {
  m = m - a;
  return m;
}

inline Matrix4x4 operator*(Matrix4x4 m, float a) {
  Matrix4x4 result;
  for (int i = 0; i < 16; i++) { 
    result.seq[i] = m.seq[i] * a;
  }
  return result;
}

inline Matrix4x4 operator*(float a, Matrix4x4 m) {
  return m * a;
}

inline Matrix4x4 & operator*=(Matrix4x4 &m, float a) {
  m = m * a;
  return m;
}

inline Matrix4x4 operator/(Matrix4x4 m, float a) {
  Matrix4x4 result;
  for (int i = 0; i < 16; i++) { 
    result.seq[i] = m.seq[i] / a;
  }
  return result;
}

inline Matrix4x4 & operator/=(Matrix4x4 &m, float a) {
  m = m / a;
  return m;
}

// transformations 

inline Matrix4x4 scale(V3 v) {
  Matrix4x4 result = {};
  result.v[0][0] = v.x;
  result.v[1][1] = v.y;
  result.v[2][2] = v.z;
  result.v[3][3] = 1;
  return result;
}

inline Matrix4x4 translate(V3 v) {
  Matrix4x4 result = identity();
  result.v[0][3] = v.x;
  result.v[1][3] = v.y;
  result.v[2][3] = v.z;

  return result;
}

inline Matrix4x4 rotate(V3 v) {
  Matrix4x4 mx, my, mz;
  mx = my = mz = {};
  mx.v[0][0] = 1.0;
  mx.v[1][1] = cos(v.x);
  mx.v[1][2] = -sin(v.x);
  mx.v[2][1] = sin(v.x);
  mx.v[2][2] = cos(v.x);
  mx.v[3][3] = 1.0;
  
  my.v[0][0] = cos(v.y);
  my.v[0][2] = -sin(v.y);
  my.v[1][1] = 1.0;
  my.v[2][0] = sin(v.y);
  my.v[2][2] = cos(v.y);
  my.v[3][3] = 1.0;
  
  mz.v[0][0] = cos(v.z);
  mz.v[0][1] = -sin(v.z);
  mz.v[1][0] = sin(v.z);
  mz.v[1][1] = cos(v.z);
  mz.v[2][2] = 1.0;
  mz.v[3][3] = 1.0;

  return mz * my * mx;
}

// matrix matrix

inline Matrix4x4 mul(Matrix4x4 a, Matrix4x4 b) {
  Matrix4x4 result = {};
  for (int i = 0; i < 16; i++) { 
    result.seq[i] = a.seq[i] * b.seq[i];
  }
  return result;
}

/***********\
 * VECTOR3 *
\***********/

inline V3 v3(float x, float y, float z) {
  V3 result;

  result.x = x;
  result.y = y;
  result.z = z;

  return result;
}

// operators

inline V3 operator+(V3 v, V3 u) {
  V3 result;

  result.x = u.x + v.x;
  result.y = u.y + v.y;
  result.z = u.z + v.z;
  
  return result;
}

inline V3 & operator+=(V3 &v, V3 u) {
  v = u + v;
  return v;
}

inline V3 operator-(V3 v, V3 u) {
  V3 result;

  result.x = u.x - v.x;
  result.y = u.y - v.y;
  result.z = u.z - v.z;
  
  return result;
}

inline V3 & operator-=(V3 &v, V3 u) {
  v = u - v;
  return v;
}

inline V3 operator-(V3 v) {
  V3 result;
  result.x = -v.x;
  result.y = -v.y;
  result.z = -v.z;

  return result;
}

// V3 and float operations

inline V3 operator+(float a, V3 v) {
  V3 result;

  result.x = a + v.x;
  result.y = a + v.y;
  result.z = a + v.z;
  
  return result;
}

inline V3 operator+(V3 v, float a) {
  return a + v;
}

inline V3 & operator+=(V3 &v, float a) {
  v = a + v;
  return v;
}

inline V3 operator-(V3 v, float a) {
  V3 result;

  result.x = v.x - a ;
  result.y = v.y - a;
  result.z = v.z - a;
  
  return result;
}

inline V3 & operator-=(V3 &v, float a) {
  v = v - a;
  return v;
}

inline V3 operator*(float a, V3 v) {
  V3 result;

  result.x = a * v.x;
  result.y = a * v.y;
  result.z = a * v.z;
  
  return result;
}

inline V3 operator*(V3 v, float a) {
  return a * v;
}

inline V3 & operator*=(V3 &v, float a) {
  v = a * v;
  return v;
}

inline V3 operator/(V3 v, float a) {
  V3 result;

  result.x = v.x / a ;
  result.y = v.y / a;
  result.z = v.z / a;
  
  return result;
}

inline V3 & operator/=(V3 &v, float a) {
  v = v / a;
  return v;
}

// scalar

inline float inner(V3 v, V3 u) {
  float result = v.x * u.x + v.y * u.y + v.z * u.z;
  return result;
}

inline float dot(V3 v, V3 u) {
  return inner(v, u);
}

inline float lenSq(V3 v) {
  return inner(v, v);
}

inline float len(V3 v) {
  return sqrt(lenSq(v));
}

// vector vector

inline V3 cross(V3 a, V3 b) {
  return v3(a.y * b.z - a.z * b.y,
            a.z * b.x - a.x * b.z,
            a.x * b.y - a.y * b.x);
}