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
#include "models.h"

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
  glfwSwapInterval(0);
  
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);

  printf("%s\n", glGetString(GL_VERSION));

  GLuint shaderProgram = compileShader("shader.vs", "shader.fs");
  if (shaderProgram == 0) {
    return -1;
  }

  glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
  GLuint texture = load_texture("resources/terrain.png");
  GLuint sprite_textures = load_texture("resources/sprites.png");

  // int grassTextures[] = {3, 3, 3, 3, 2, 0};
  // GLuint grass = genTexturedCubeBuffer(grassTextures);
  int stoneTextures[] = {1, 1, 1, 1, 1, 1};
  GLuint stone = genTexturedCubeBuffer(stoneTextures);
  int stoneSlopeTextures[] = {1, 1, 1, 1, 1};
  GLuint stoneSlope = genTexturedSlopeBuffer(stoneSlopeTextures);
  int stoneSlopeCornerTextures[] = {1, 1, 1, 1};
  GLuint stoneSlopeCorner = genTexturedSlopeCornerBuffer(stoneSlopeCornerTextures);


  // Y, Z, X?
  // 0 is nothing, 1 is cube, 2 is -Z, 3 is -X, 4 is +Z, 5 is +X
  const int CHUNK_SIZE = 16;
  int chunk[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE];
  for (int i = 0; i < CHUNK_SIZE; i++) {
    for (int j = 0; j < CHUNK_SIZE; j++) {
      for (int k = 0; k < CHUNK_SIZE; k++) {
        chunk[i][j][k] = 1;
      }
    }
  }

  Sprite sprites[] = { { { 0.5, CHUNK_SIZE/2.0, -0.5 }, 0 },
                       { { 4.5, CHUNK_SIZE/2.0, 7.5 }, 0 },
                       { { -6.5, CHUNK_SIZE/2.0, -2.5 }, 0 } };

  GLuint sprite_vaos[] = { genTexturedGroundedSprite(0) };

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
    if (acc >= 2.0) {
      printf("fps: %2.2f\n", frames/acc);
      acc = 0;
      frames = 0;
    }

    t += delta;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float y_rot = M_PI / 4;

    Matrix4x4 view = identity();
    view *= translate(v3(0.0, 0.0, -CHUNK_SIZE - 5.0));
    view *= rotate(v3(1.0, 0.0, 0.0) * M_PI / 4);
    view *= rotate(v3(0.0, 1.0, 0.0) * y_rot);
    float ratio = float(SCREEN_WIDTH) / float(SCREEN_HEIGHT);
    Matrix4x4 projection = perspective_projection(0.1, 100.0, 45.0, ratio);

    glUseProgram(shaderProgram);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    GLuint transformLocation = glGetUniformLocation(shaderProgram, "view");
    glUniformMatrix4fv(transformLocation, 1, GL_TRUE, (float*)&view);
    transformLocation = glGetUniformLocation(shaderProgram, "projection");
    glUniformMatrix4fv(transformLocation, 1, GL_TRUE, (float*)&projection);

    for (int y = 0; y < CHUNK_SIZE; y++) {
      for (int z = 0; z < CHUNK_SIZE; z++) {
        for (int x = 0; x < CHUNK_SIZE; x++) {
          // 0 is nothing, 1 is cube, 2 is -Z, 3 is -X, 4 is +Z, 5 is +X
          int block = chunk[y][z][x];
          if (block == 0) {
            continue;
          }
          Matrix4x4 model = identity();
          model *= translate(v3(x, y, z) - (CHUNK_SIZE - 1)/2.0);
          switch (block) {
            case 0: break;
            case 1: break;
            case 2:
            case 6: {
              model *= rotate(v3(0, 1, 0) * M_PI);
              break;
            }
            case 3:
            case 7: {
              model *= rotate(v3(0, 1, 0) * M_PI / 2);
              break;
            }
            case 4: break;
            case 8: break;
            case 5: 
            case 9:{
              model *= rotate(v3(0, 1, 0) *  3 * M_PI / 2);
              break;
            }
          }
          transformLocation = glGetUniformLocation(shaderProgram, "model");
          glUniformMatrix4fv(transformLocation, 1, GL_TRUE, (float*)&model);
          if (block == 1) {
            glBindVertexArray(stone);
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
          } else if (block <= 5) {
            glBindVertexArray(stoneSlope);
            glDrawElements(GL_TRIANGLES, 30, GL_UNSIGNED_INT, 0);
          } else if (block <= 9) {
            glBindVertexArray(stoneSlopeCorner);
            glDrawElements(GL_TRIANGLES, 24, GL_UNSIGNED_INT, 0);
          }
        }
      }
    }

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, sprite_textures);
    for (unsigned int i = 0; i < (sizeof(sprites) / sizeof(Sprite)); i++) {
      Sprite s = sprites[i];
      Matrix4x4 model = identity();
      model *= translate(s.position);
      model *= rotate(v3(0, 1, 0) * -y_rot);
      transformLocation = glGetUniformLocation(shaderProgram, "model");
      glUniformMatrix4fv(transformLocation, 1, GL_TRUE, (float*)&model);
      glBindVertexArray(sprite_vaos[s.index]);
      glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }

    glfwSwapBuffers(window);
    glfwPollEvents();    
  }

  glfwTerminate();
  return 0;
}
