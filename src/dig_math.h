float clamp(float lower, float x, float upper) {
  return fminf(fmaxf(lower, x), upper);
}

float fractional_part(float x) {
  return x - floor(x);
}

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

inline V3 operator*(Matrix4x4 m, V3 v) {
  float v4[4] = { v.v[0], v.v[1], v.v[2], 1 };
  float result[4] = { 0.0, 0.0, 0.0, 0.0 };
  for(int y = 0; y < 4; y++) {
    for(int x = 0; x < 4; x++) {
      result[y] += v4[x] * m.v[y][x];
    }
  }
  return {result[0], result[1], result[2]};
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

inline void printV3(V3 v) {
  printf("[ %2.5f, %2.5f, %2.5f ]", v.x, v.y, v.z);
}

inline V3i toV3i(V3 v) {
  return {int(v.x), int(v.y), int(v.z)};
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

  result.x = v.x - u.x;
  result.y = v.y - u.y;
  result.z = v.z - u.z;
  
  return result;
}

inline V3 & operator-=(V3 &v, V3 u) {
  v = v - u;
  return v;
}

inline V3 operator-(V3 v) {
  V3 result;
  result.x = -v.x;
  result.y = -v.y;
  result.z = -v.z;

  return result;
}

inline V3 operator*(V3 v, V3 u) {
  V3 result;

  result.x = v.x * u.x;
  result.y = v.y * u.y;
  result.z = v.z * u.z;
  
  return result;
}

inline V3 & operator*=(V3 &v, V3 u) {
  v = v*u;
  return v;
}

inline V3 operator/(V3 v, V3 u) {
  V3 result;

  result.x = v.x / u.x;
  result.y = v.y / u.y;
  result.z = v.z / u.z;
  
  return result;
}

inline V3 & operator/=(V3 &v, V3 u) {
  v = v/u;
  return v;
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

inline bool operator==(V3 v, V3 u) {
  return v.x == u.x && v.y == u.y && v.z == u.z;
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

inline float min(V3 v) {
  return fminf(fminf(v.x, v.y), v.z);
}

inline float max(V3 v) {
  return fmaxf(fmaxf(v.x, v.y), v.z);
}

inline V3 normalise(V3 v) {
  float length = len(v);
  if (length == 0.0) {
    return v;
  }
  return v / length;
}

inline V3 floor(V3 v) {
  return v3(floor(v.x), floor(v.y), floor(v.z));
}

inline V3 fractional_part(V3 v) {
  return v - floor(v);
}

inline V3 abs(V3 v) {
  return v3(fabsf(v.x), fabsf(v.y), fabsf(v.z));
}


// vector vector

inline V3 cross(V3 a, V3 b) {
  return v3(a.y * b.z - a.z * b.y,
            a.z * b.x - a.x * b.z,
            a.x * b.y - a.y * b.x);
}

inline V3 copysign(V3 magnitude, V3 sign) {
  return v3(copysignf(magnitude.x, sign.x),
            copysignf(magnitude.y, sign.y),
            copysignf(magnitude.z, sign.z));
}

/***************\
 * VECTOR3 INT *
\***************/

inline V3i v3i(int x, int y, int z) {
  V3i result;

  result.x = x;
  result.y = y;
  result.z = z;

  return result;
}

inline void printV3i(V3i v) {
  printf("[ %d, %d, %d ]", v.x, v.y, v.z);
}

// TODO: I couldn't find a way to make these casts. Oh well.
inline V3 toV3(V3i v) {
  return v3((float)v.x, (float)v.y, (float)v.z);
}


// operators

inline V3i operator+(V3i v, V3i u) {
  V3i result;

  result.x = u.x + v.x;
  result.y = u.y + v.y;
  result.z = u.z + v.z;
  
  return result;
}

inline V3i & operator+=(V3i &v, V3i u) {
  v = u + v;
  return v;
}

inline V3i operator-(V3i v, V3i u) {
  V3i result;

  result.x = v.x - u.x;
  result.y = v.y - u.y;
  result.z = v.z - u.z;
  
  return result;
}

inline V3i & operator-=(V3i &v, V3i u) {
  v = u - v;
  return v;
}

inline V3i operator-(V3i v) {
  V3i result;
  result.x = -v.x;
  result.y = -v.y;
  result.z = -v.z;

  return result;
}

// V3i and int operations

inline V3i operator+(int a, V3i v) {
  V3i result;

  result.x = a + v.x;
  result.y = a + v.y;
  result.z = a + v.z;
  
  return result;
}

inline V3i operator+(V3i v, int a) {
  return a + v;
}

inline V3i & operator+=(V3i &v, int a) {
  v = a + v;
  return v;
}

inline V3i operator-(V3i v, int a) {
  V3i result;

  result.x = v.x - a ;
  result.y = v.y - a;
  result.z = v.z - a;
  
  return result;
}

inline V3i & operator-=(V3i &v, int a) {
  v = v - a;
  return v;
}

inline V3i operator*(int a, V3i v) {
  V3i result;

  result.x = a * v.x;
  result.y = a * v.y;
  result.z = a * v.z;
  
  return result;
}

inline V3i operator*(V3i v, int a) {
  return a * v;
}

inline V3i & operator*=(V3i &v, int a) {
  v = a * v;
  return v;
}

inline V3i operator/(V3i v, int a) {
  V3i result;

  result.x = v.x / a ;
  result.y = v.y / a;
  result.z = v.z / a;
  
  return result;
}

inline V3i & operator/=(V3i &v, int a) {
  v = v / a;
  return v;
}

inline V3i operator%(V3i v, int a) {
  V3i result;

  result.x = v.x % a;
  result.y = v.y % a;
  result.z = v.z % a;
  
  return result;
}

inline V3i & operator%=(V3i &v, int a) {
  v = v % a;
  return v;
}

// scalar

inline int inner(V3i v, V3i u) {
  int result = v.x * u.x + v.y * u.y + v.z * u.z;
  return result;
}

inline int dot(V3i v, V3i u) {
  return inner(v, u);
}

inline int lenSq(V3i v) {
  return inner(v, v);
}

inline int len(V3i v) {
  return sqrt(lenSq(v));
}

// vector vector

inline V3i cross(V3i a, V3i b) {
  return v3i(a.y * b.z - a.z * b.y,
            a.z * b.x - a.x * b.z,
            a.x * b.y - a.y * b.x);
}

/***********\
 * VECTOR2 *
\***********/

inline V2 v2(float x, float y) {
  V2 result;

  result.x = x;
  result.y = y;

  return result;
}

// operators

inline V2 operator+(V2 v, V2 u) {
  V2 result;

  result.x = u.x + v.x;
  result.y = u.y + v.y;
  
  return result;
}

inline V2 & operator+=(V2 &v, V2 u) {
  v = u + v;
  return v;
}

inline V2 operator-(V2 v, V2 u) {
  V2 result;

  result.x = v.x - u.x;
  result.y = v.y - u.y;
  
  return result;
}

inline V2 & operator-=(V2 &v, V2 u) {
  v = u - v;
  return v;
}

inline V2 operator-(V2 v) {
  V2 result;
  result.x = -v.x;
  result.y = -v.y;

  return result;
}

// V2 and float operations

inline V2 operator+(float a, V2 v) {
  V2 result;

  result.x = a + v.x;
  result.y = a + v.y;
  
  return result;
}

inline V2 operator+(V2 v, float a) {
  return a + v;
}

inline V2 & operator+=(V2 &v, float a) {
  v = a + v;
  return v;
}

inline V2 operator-(V2 v, float a) {
  V2 result;

  result.x = v.x - a ;
  result.y = v.y - a;
  
  return result;
}

inline V2 & operator-=(V2 &v, float a) {
  v = v - a;
  return v;
}

inline V2 operator*(float a, V2 v) {
  V2 result;

  result.x = a * v.x;
  result.y = a * v.y;
  
  return result;
}

inline V2 operator*(V2 v, float a) {
  return a * v;
}

inline V2 & operator*=(V2 &v, float a) {
  v = a * v;
  return v;
}

inline V2 operator/(V2 v, float a) {
  V2 result;

  result.x = v.x / a ;
  result.y = v.y / a;
  
  return result;
}

inline V2 & operator/=(V2 &v, float a) {
  v = v / a;
  return v;
}

// scalar

inline float inner(V2 v, V2 u) {
  float result = v.x * u.x + v.y * u.y;
  return result;
}

inline float dot(V2 v, V2 u) {
  return inner(v, u);
}

inline float lenSq(V2 v) {
  return inner(v, v);
}

inline float len(V2 v) {
  return sqrt(lenSq(v));
}


/************\
 * VECTOR2I *
\************/

inline V2i v2i(int x, int y) {
  V2i result;

  result.x = x;
  result.y = y;

  return result;
}

// operators

inline V2i operator+(V2i v, V2i u) {
  V2i result;

  result.x = u.x + v.x;
  result.y = u.y + v.y;
  
  return result;
}

inline V2i & operator+=(V2i &v, V2i u) {
  v = u + v;
  return v;
}

inline V2i operator-(V2i v, V2i u) {
  V2i result;

  result.x = v.x - u.x;
  result.y = v.y - u.y;
  
  return result;
}

inline V2i & operator-=(V2i &v, V2i u) {
  v = u - v;
  return v;
}

inline V2i operator-(V2i v) {
  V2i result;
  result.x = -v.x;
  result.y = -v.y;

  return result;
}

// V2i and int operations

inline V2i operator+(int a, V2i v) {
  V2i result;

  result.x = a + v.x;
  result.y = a + v.y;
  
  return result;
}

inline V2i operator+(V2i v, int a) {
  return a + v;
}

inline V2i & operator+=(V2i &v, int a) {
  v = a + v;
  return v;
}

inline V2i operator-(V2i v, int a) {
  V2i result;

  result.x = v.x - a ;
  result.y = v.y - a;
  
  return result;
}

inline V2i & operator-=(V2i &v, int a) {
  v = v - a;
  return v;
}

inline V2i operator*(int a, V2i v) {
  V2i result;

  result.x = a * v.x;
  result.y = a * v.y;
  
  return result;
}

inline V2i operator*(V2i v, int a) {
  return a * v;
}

inline V2i & operator*=(V2i &v, int a) {
  v = a * v;
  return v;
}

inline V2i operator/(V2i v, int a) {
  V2i result;

  result.x = v.x / a ;
  result.y = v.y / a;
  
  return result;
}

inline V2i & operator/=(V2i &v, int a) {
  v = v / a;
  return v;
}

// scalar

inline int inner(V2i v, V2i u) {
  int result = v.x * u.x + v.y * u.y;
  return result;
}

inline int dot(V2i v, V2i u) {
  return inner(v, u);
}

inline int lenSq(V2i v) {
  return inner(v, v);
}

inline float len(V2i v) {
  return sqrt(lenSq(v));
}

// MISC

QuadraticSolutions solveQuadratic(float a, float b, float c) {
  QuadraticSolutions solutions;
  if (a == 0.0) {
    if (b == 0.0) {
      solutions.solution_exists = false;
      return solutions;
    }
    solutions.solution_exists = true;
    float only_solution = -c/b;
    solutions.min = only_solution;
    solutions.max = only_solution;
    return solutions;
  }

  float determinant = b*b - 4.0f*a*c;

  if (determinant < 0.0f) {
    solutions.solution_exists = false;
    return solutions;
  }

  float sqrtD = sqrt(determinant);
  float r1 = (-b - sqrtD) / (2*a);
  float r2 = (-b + sqrtD) / (2*a);

  if (r1 > r2) {
    float temp = r2;
    r2 = r1;
    r1 = temp;
  }

  solutions.solution_exists = true;
  solutions.min = r1;
  solutions.max = r2;
  return solutions;
}


float signedDistanceFromPlane(V3 normal, V3 point_on_plane, V3 p) {
  return dot(p - point_on_plane, normal);
}

float signedDistanceFromTrianglePlane(Triangle t, V3 p) {
  return signedDistanceFromPlane(t.normal, t.vertices[0], p);
}

bool isPointInTriangle(Triangle t, V3 pos) {
  V3 axis0 = t.vertices[1] - t.vertices[0];
  V3 axis1 = t.vertices[2] - t.vertices[0];
  V3 relative_pos = pos - t.vertices[0];

  float axis0_dist_sq = dot(axis0, axis0);
  float axis1_dist_sq = dot(axis1, axis1);

  float axis_0_of_pos = dot(axis0, relative_pos);
  float axis_1_of_pos = dot(axis1, relative_pos);

  float crossover = dot(axis0, axis1);

  float inv_denom = 1 / (axis0_dist_sq * axis1_dist_sq - crossover * crossover);
  float u = (axis1_dist_sq * axis_0_of_pos - crossover * axis_1_of_pos) * inv_denom;
  float v = (axis0_dist_sq * axis_1_of_pos - crossover * axis_0_of_pos) * inv_denom;

  return u >= 0 && v >= 0 && (u + v) <= 1;
}


inline Triangle shiftTriangle(Triangle t, V3 v) {
  t.vertices[0] += v;
  t.vertices[1] += v;
  t.vertices[2] += v;
  return t;
}
