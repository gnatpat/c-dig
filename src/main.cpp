#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

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
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
    char* infoLog = (char*)malloc(len * sizeof(char));
    glGetShaderInfoLog(shader, len, NULL, infoLog);
    printf("Shader compilation error. %s.\n%s\n", path, infoLog);
    free(infoLog);
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
    char* infoLog = (char*)malloc(len * sizeof(char));
    glGetProgramInfoLog(shader, len, NULL, infoLog);
    printf("Shader linker error.\n%s\n", infoLog);
    free(infoLog);
    return 0;
  }

  return shader;
}

GLuint load_texture(const char* filepath) {
  GLuint texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  // set the texture wrapping/filtering options (on the currently bound texture object)
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  // load and generate the texture
  int width, height, nrChannels;
  unsigned char *data = stbi_load(filepath, &width, &height, &nrChannels, 0);
  if (!data) {
    printf("Failed to load texture.");
    return 0;
  }
  GLenum format;
  if (nrChannels == 3) {
    format = GL_RGB;
  } else {
    format = GL_RGBA;
  }
  glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
  stbi_image_free(data);
  return texture;
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


void texture_indices_to_texture_coords(int textures[6], Rectangle* texture_coords) {
  // NOTE(nathan): Texture map size is hardcoded for now, will need to change.
  float map_width = 16.0 * 16.0;
  float unit = 16.0 / map_width;
  for(int i = 0; i < 6; i++) {
    int index = textures[i];
    float y = ((index / 16) * 16) / map_width;
    float x = ((index % 16) * 16) / map_width;
    texture_coords[i].left = x;
    // NOTE(nathan): Is this right? It was upside down. Who knows??
    texture_coords[i].bottom = y + unit;
    texture_coords[i].top = y;
    texture_coords[i].right = x + unit;
  }
}

// Textures go near, far, left, right, bottom, top
GLuint genTexturedCubeBuffer(int textures[6]) {
  Rectangle texture_coords[6];
  texture_indices_to_texture_coords(textures, texture_coords);

  GLuint vao, vbo, ebo;
  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);
  glGenBuffers(1, &ebo);

  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

  float vertices[] = {
    -0.5f, -0.5f, -0.5f, texture_coords[0].left, texture_coords[0].bottom,
     0.5f, -0.5f, -0.5f, texture_coords[0].right, texture_coords[0].bottom,
     0.5f,  0.5f, -0.5f, texture_coords[0].right, texture_coords[0].top,
    -0.5f,  0.5f, -0.5f, texture_coords[0].left, texture_coords[0].top,    

    -0.5f, -0.5f,  0.5f, texture_coords[1].left, texture_coords[1].bottom,
     0.5f, -0.5f,  0.5f, texture_coords[1].right, texture_coords[1].bottom,
     0.5f,  0.5f,  0.5f, texture_coords[1].right, texture_coords[1].top,
    -0.5f,  0.5f,  0.5f, texture_coords[1].left, texture_coords[1].top,

    -0.5f,  0.5f,  0.5f, texture_coords[2].right, texture_coords[2].top,    
    -0.5f,  0.5f, -0.5f, texture_coords[2].left, texture_coords[2].top,
    -0.5f, -0.5f, -0.5f, texture_coords[2].left, texture_coords[2].bottom,
    -0.5f, -0.5f,  0.5f, texture_coords[2].right, texture_coords[2].bottom,

     0.5f,  0.5f,  0.5f, texture_coords[3].right, texture_coords[3].top,
     0.5f,  0.5f, -0.5f, texture_coords[3].left, texture_coords[3].top,
     0.5f, -0.5f, -0.5f, texture_coords[3].left, texture_coords[3].bottom,
     0.5f, -0.5f,  0.5f, texture_coords[3].right, texture_coords[3].bottom,    

     0.5f, -0.5f, -0.5f, texture_coords[4].left, texture_coords[4].bottom,
     0.5f, -0.5f,  0.5f, texture_coords[4].right, texture_coords[4].bottom,
    -0.5f, -0.5f,  0.5f, texture_coords[4].right, texture_coords[4].top,
    -0.5f, -0.5f, -0.5f, texture_coords[4].left, texture_coords[4].top,    

     0.5f,  0.5f, -0.5f, texture_coords[5].left, texture_coords[5].bottom,
     0.5f,  0.5f,  0.5f, texture_coords[5].right, texture_coords[5].bottom,
    -0.5f,  0.5f,  0.5f, texture_coords[5].right, texture_coords[5].top,
    -0.5f,  0.5f, -0.5f, texture_coords[5].left, texture_coords[5].top,    
  };

  unsigned int indices[] = {
    0, 1, 2,
    3, 2, 0,
    4, 5, 6,
    7, 6, 4,
    8, 9, 10,
    11, 10, 8,
    12, 13, 14,
    15, 14, 12,
    16, 17, 18,
    19, 18, 16,
    20, 21, 22,
    23, 22, 20
  };

  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
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
  // Textures go near, far, left, right, bottom, top
  GLuint texture = load_texture("resources/terrain.png");
  int textures[] = {3, 3, 3, 3, 2, 0};
  GLuint vao = genTexturedCubeBuffer(textures);

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
    glActiveTexture(GL_TEXTURE0); // activate the texture unit first before binding texture
    glBindTexture(GL_TEXTURE_2D, texture);
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
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
      }
    }

    glfwSwapBuffers(window);
    glfwPollEvents();    
  }

  glfwTerminate();
  return 0;
}
