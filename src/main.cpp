#include "main.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>


#include "models.cpp"

int SCREEN_WIDTH = 800;
int SCREEN_HEIGHT = 600;

void initChunk(Chunk* c, Constants* constants) {
  for (int x = 0; x < CHUNK_SIZE; x++) {
    for (int y = 0; y < CHUNK_SIZE; y++) {
      for (int z = 0; z < CHUNK_SIZE; z++) {
        c->blocks[x][y][z].block_shape = &constants->block_shape_flyweights.cube;
        c->
      }
    }
  }
}

void initaliseConstants(Constants* constants) {
  initFaces(&constants->faces);
  fillBlockFlyweights(&constants->block_shape_flyweights, &constants->faces);
}

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
  GLuint shaderProgramNoTexture = compileShader("shader_no_texture.vs", "shader_no_texture.fs");
  if (shaderProgramNoTexture == 0) {
    return -1;
  }

  glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
  // GLuint texture = load_texture("resources/terrain.png");
  GLuint sprite_textures = load_texture("resources/sprites.png");

  Sprite sprites[] = { { { 0.5, CHUNK_SIZE/2.0, -0.5 }, 0 },
                       { { 4.5, CHUNK_SIZE/2.0, 7.5 }, 0 },
                       { { -6.5, CHUNK_SIZE/2.0, -2.5 }, 0 } };

  GLuint sprite_vaos[] = { genTexturedGroundedSprite(0) };

  GameData* game_data = (GameData*) malloc(sizeof(GameData));
  initaliseConstants(&game_data->constants);
  initChunk(&game_data->chunk, &game_data->constants);

  Chunk* chunk = &game_data->chunk;
  //BlockShapeFlyweights* block_shape_flyweights = &game_data->constants.block_shape_flyweights;
  //chunk->blocks[0][15][1].block_shape = &block_shape_flyweights->slopes[XZ_NEG_X][0];
  //chunk->blocks[2][15][1].block_shape = &block_shape_flyweights->slopes[XZ_POS_X][0];
  //chunk->blocks[1][15][0].block_shape = &block_shape_flyweights->slopes[XZ_NEG_Z][0];
  //chunk->blocks[1][15][2].block_shape = &block_shape_flyweights->slopes[XZ_POS_Z][0];
  fillChunkRenderData(chunk);

  struct timespec old_time, new_time;
  clock_gettime(CLOCK_MONOTONIC_RAW, &old_time);

  int frames = 0;
  float acc = 0;

  float t = 0.0;
  while(!glfwWindowShouldClose(window))
  {
    clock_gettime(CLOCK_MONOTONIC_RAW, &new_time);
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

    float y_rot = M_PI / 8 * t;

    // Chunk
    Matrix4x4 model = identity();
    model *= translate(v3(-(CHUNK_SIZE-1)/2, -(CHUNK_SIZE-1)/2, -(CHUNK_SIZE-1)/2));
    Matrix4x4 view = identity();
    view *= translate(v3(0.0, 0.0, -CHUNK_SIZE - 5.0));
    view *= rotate(v3(1.0, 0.0, 0.0) * M_PI / 4);
    view *= rotate(v3(0.0, 1.0, 0.0) * y_rot);
    float ratio = float(SCREEN_WIDTH) / float(SCREEN_HEIGHT);
    Matrix4x4 projection = perspective_projection(0.1, 100.0, 45.0, ratio);

    glUseProgram(shaderProgramNoTexture);
    GLuint transformLocation = glGetUniformLocation(shaderProgram, "view");
    glUniformMatrix4fv(transformLocation, 1, GL_TRUE, (float*)&view);
    transformLocation = glGetUniformLocation(shaderProgram, "projection");
    glUniformMatrix4fv(transformLocation, 1, GL_TRUE, (float*)&projection);
    transformLocation = glGetUniformLocation(shaderProgram, "model");
    glUniformMatrix4fv(transformLocation, 1, GL_TRUE, (float*)&model);

    ChunkRenderData* render_data = &(chunk->render_data);
    glBindVertexArray(render_data->vao);
    glDrawElements(GL_TRIANGLES, render_data->num_indices, GL_UNSIGNED_INT, 0);

    // Sprites
    glUseProgram(shaderProgram);
    transformLocation = glGetUniformLocation(shaderProgram, "view");
    glUniformMatrix4fv(transformLocation, 1, GL_TRUE, (float*)&view);
    transformLocation = glGetUniformLocation(shaderProgram, "projection");
    glUniformMatrix4fv(transformLocation, 1, GL_TRUE, (float*)&projection);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, sprite_textures);
    for (unsigned int i = 0; i < (sizeof(sprites) / sizeof(Sprite)); i++) {
      Sprite s = sprites[i];
      model = identity();
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
