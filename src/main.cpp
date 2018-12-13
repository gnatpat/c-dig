#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "defs.h"
#include "dig_math.h"

int SCREEN_WIDTH = 800;
int SCREEN_HEIGHT = 600;

void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
  SCREEN_WIDTH = width;
  SCREEN_HEIGHT = height;
  printf("%dx%d\n", SCREEN_WIDTH, SCREEN_HEIGHT);
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
  GLuint vao, vbo, ebo;
  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);
  glGenBuffers(1, &ebo);

  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

  float vertices[] = {
      -0.5f, -0.5f, 0.0f,
       0.5f, -0.5f, 0.0f,
       0.5f,  0.5f, 0.0f,
      -0.5f,  0.5f, 0.0f
  };

  int indicies[] = {
      0, 1, 2,
      0, 2, 3
  };

  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicies), indicies, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  return vao;
}

GLuint genCubeBuffer() {
  GLuint vao, vbo;
  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);

  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);

  float vertices[] = {
    -0.5f, -0.5f, -0.5f, 0.6, 0.3, 0.15,
     0.5f, -0.5f, -0.5f, 0.6, 0.3, 0.15,
     0.5f,  0.5f, -0.5f, 0.6, 0.3, 0.15,
     0.5f,  0.5f, -0.5f, 0.6, 0.3, 0.15,
    -0.5f,  0.5f, -0.5f, 0.6, 0.3, 0.15,
    -0.5f, -0.5f, -0.5f, 0.6, 0.3, 0.15,

    -0.5f, -0.5f,  0.5f, 0.6, 0.3, 0.15,
     0.5f, -0.5f,  0.5f, 0.6, 0.3, 0.15,
     0.5f,  0.5f,  0.5f, 0.6, 0.3, 0.15,
     0.5f,  0.5f,  0.5f, 0.6, 0.3, 0.15,
    -0.5f,  0.5f,  0.5f, 0.6, 0.3, 0.15,
    -0.5f, -0.5f,  0.5f, 0.6, 0.3, 0.15,

    -0.5f,  0.5f,  0.5f, 0.6, 0.3, 0.15,
    -0.5f,  0.5f, -0.5f, 0.6, 0.3, 0.15,
    -0.5f, -0.5f, -0.5f, 0.6, 0.3, 0.15,
    -0.5f, -0.5f, -0.5f, 0.6, 0.3, 0.15,
    -0.5f, -0.5f,  0.5f, 0.6, 0.3, 0.15,
    -0.5f,  0.5f,  0.5f, 0.6, 0.3, 0.15,

     0.5f,  0.5f,  0.5f, 0.6, 0.3, 0.15,
     0.5f,  0.5f, -0.5f, 0.6, 0.3, 0.15,
     0.5f, -0.5f, -0.5f, 0.6, 0.3, 0.15,
     0.5f, -0.5f, -0.5f, 0.6, 0.3, 0.15,
     0.5f, -0.5f,  0.5f, 0.6, 0.3, 0.15,
     0.5f,  0.5f,  0.5f, 0.6, 0.3, 0.15,

    -0.5f, -0.5f, -0.5f, 0.3, 0.15, 0.0,
     0.5f, -0.5f, -0.5f, 0.3, 0.15, 0.0,
     0.5f, -0.5f,  0.5f, 0.3, 0.15, 0.0,
     0.5f, -0.5f,  0.5f, 0.3, 0.15, 0.0,
    -0.5f, -0.5f,  0.5f, 0.3, 0.15, 0.0,
    -0.5f, -0.5f, -0.5f, 0.3, 0.15, 0.0,

    -0.5f,  0.5f, -0.5f, 0.0, 0.5, 0.0,
     0.5f,  0.5f, -0.5f, 0.0, 0.5, 0.0,
     0.5f,  0.5f,  0.5f, 0.0, 0.5, 0.0,
     0.5f,  0.5f,  0.5f, 0.0, 0.5, 0.0,
    -0.5f,  0.5f,  0.5f, 0.0, 0.5, 0.0,
    -0.5f,  0.5f, -0.5f, 0.0, 0.5, 0.0,
  };

  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

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
  
  glEnable(GL_DEPTH_TEST);

  printf("%s\n", glGetString(GL_VERSION));

  GLuint shaderProgram = compileShader("shader.vs", "shader.fs");
  if (shaderProgram == 0) {
    return -1;
  }

  glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
  GLuint vao = genCubeBuffer();

  struct timespec old_time, new_time;
  clock_gettime(CLOCK_MONOTONIC_RAW, &old_time);

  int frames = 0;
  float acc = 0;

  float t = 0.0;
  while(!glfwWindowShouldClose(window))
  {
    clock_gettime(CLOCK_MONOTONIC, &new_time);
    float delta = (new_time.tv_sec - old_time.tv_sec) + (new_time.tv_nsec - old_time.tv_nsec) / 1000000000.0f;
    old_time = new_time;

    frames += 1;
    acc += delta;
    if (acc >= 0.5) {
      printf("fps: %2.2f\n", frames/acc);
      acc = 0;
      frames = 0;
    }

    t += delta;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    Matrix4x4 view = identity();
    view *= translate(v3(0.0, 0.0, -35.0));
    view *= rotate(v3(1.0, 0.0, 0.0) * M_PI / 5);
    view *= rotate(v3(0.0, 1.0, 0.0) * M_PI / 4 * t * 0.234);
    float ratio = float(SCREEN_WIDTH) / float(SCREEN_HEIGHT);
    Matrix4x4 projection = perspective_projection(0.1, 100, 45.0, ratio);

    glUseProgram(shaderProgram);
    GLuint transformLocation = glGetUniformLocation(shaderProgram, "view");
    glUniformMatrix4fv(transformLocation, 1, GL_TRUE, (float*)&view);
    transformLocation = glGetUniformLocation(shaderProgram, "projection");
    glUniformMatrix4fv(transformLocation, 1, GL_TRUE, (float*)&projection);

    glUseProgram(shaderProgram);
    glBindVertexArray(vao);
    int max = 101;
    float wavelength = 4.0;
    float freq = 3.0;
    float amp = 1.0;
    for (int xx = 0; xx < max; xx++) {
      for (int zz = 0; zz < max; zz++) {
        float x = (float)xx - max/2;
        float z = (float)zz - max/2;
        float y = (cos(-abs(x)/wavelength + t * freq) + cos(-abs(z)/wavelength + t * freq)) * amp;
        Matrix4x4 model = identity();
        model *= translate(v3(x, y, z));
        transformLocation = glGetUniformLocation(shaderProgram, "model");
        glUniformMatrix4fv(transformLocation, 1, GL_TRUE, (float*)&model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
      }
    }

    glfwSwapBuffers(window);
    glfwPollEvents();    
  }

  glfwTerminate();
  return 0;
}
