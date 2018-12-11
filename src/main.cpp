#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

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


void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

int getFileSize(FILE *fp) {
  fseek(fp, 0, SEEK_END);
  int size = ftell(fp);
  fseek(fp, 0, SEEK_SET);
  return size;
}

GLuint compileShaderFile(const char* path, const GLenum shaderType) {
  FILE *fp = fopen(path, "r");
  if (fp == NULL) {
    printf("Couldn't open %s.\n", path);
    return 0;
  }
  GLuint shader = glCreateShader(shaderType);

  int len = getFileSize(fp);
  char* shaderText = (char*)malloc(len * sizeof(char));
  fread(shaderText, sizeof(char), len, fp);

  glShaderSource(shader, 1, &shaderText, &len);
  free(shaderText);

  glCompileShader(shader);

  int success;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (success == GL_FALSE) {
    int len;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
    char infoLog[len];
    glGetShaderInfoLog(shader, len, NULL, infoLog);
    printf("Shader compilation error. %s.\n%s\n", path, infoLog);
    return 0;
  }

  return shader;
}

GLuint compileShader(const char* vertexPath, const char* fragmentPath) {
  GLuint vertexShader = compileShaderFile(vertexPath, GL_VERTEX_SHADER);
  if (vertexShader == 0) {
    return 0;
  }
  GLuint fragmentShader = compileShaderFile(fragmentPath, GL_FRAGMENT_SHADER);
  if (fragmentShader == 0) {
    return 0;
  }

  GLuint shader = glCreateProgram();
  glAttachShader(shader, vertexShader);
  glAttachShader(shader, fragmentShader);
  glLinkProgram(shader);

  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  int success;
  glGetProgramiv(shader, GL_LINK_STATUS, &success);
  if (success == GL_FALSE) {
    int len;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
    char infoLog[len];
    glGetProgramInfoLog(shader, len, NULL, infoLog);
    printf("Shader linker error.\n%s\n", infoLog);
    return 0;
  }

  return shader;
}

GLuint genBuffers() {
  GLuint vao, vbo;
  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);

  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);

  float vertices[] = {
      -0.5f, -0.5f, 0.0f,
       0.5f, -0.5f, 0.0f,
       0.0f,  0.5f, 0.0f
  };

  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  return vao;
}

int main(void) {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
  if (window == NULL)
  {
      printf("Failed to create GLFW window");
      glfwTerminate();
      return -1;
  }
  glfwMakeContextCurrent(window);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
  {
      printf("Failed to initialize GLAD");
      return -1;
  }    

  glViewport(0, 0, 800, 600);
  glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

  printf("%s\n", glGetString(GL_VERSION));

  GLuint shaderProgram = compileShader("shader.vs", "shader.fs");
  if (shaderProgram == 0) {
    return -1;
  }

  glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
  GLuint vao = genBuffers();

  Matrix4x4 trans = identity();
  trans *= rotate(v3(0, 0, 1) * M_PI / 2);
  trans *= scale(v3(0.5, 0.5, 0.5));
  glUseProgram(shaderProgram);
  GLuint transformLocation = glGetUniformLocation(shaderProgram, "transform");
  glUniformMatrix4fv(transformLocation, 1, GL_FALSE, (float*)&trans);

  while(!glfwWindowShouldClose(window))
  {
      glClear(GL_COLOR_BUFFER_BIT);

      glUseProgram(shaderProgram);
      glBindVertexArray(vao);
      glDrawArrays(GL_TRIANGLES, 0, 3);

      glfwSwapBuffers(window);
      glfwPollEvents();    
  }

  glfwTerminate();
  return 0;
}
