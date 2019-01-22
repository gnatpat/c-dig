#include "main.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "models.cpp"
#include "shaders.cpp"
#include "utils.cpp"

int SCREEN_WIDTH = 800;
int SCREEN_HEIGHT = 600;

void initChunk(Chunk* c) {
  for (int x = 0; x < CHUNK_SIZE; x++) {
    for (int y = 0; y < CHUNK_SIZE; y++) {
      for (int z = 0; z < CHUNK_SIZE; z++) {
        c->blocks[x][y][z].block_shape = CUBE;
      }
    }
  }
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
  SCREEN_WIDTH = width;
  SCREEN_HEIGHT = height;
  printf("%dx%d\n", SCREEN_WIDTH, SCREEN_HEIGHT);
  glViewport(0, 0, width, height);
}

GLuint loadTexture(const char* filepath) {
  GLuint texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  // set the texture wrapping/filtering options (on the currently bound texture object)
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  // load and generate the texture
  int width, height, channels;
  unsigned char *data = stbi_load(filepath, &width, &height, &channels, 0);
  if (!data) {
    printf("Failed to load texture.");
    return 0;
  }
  GLenum format;
  if (channels == 3) {
    format = GL_RGB;
  } else {
    format = GL_RGBA;
  }
  glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
  stbi_image_free(data);
  return texture;
}

bool initOpenGLAndCreateWindow(GLFWwindow** window) {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  *window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
  if (*window == NULL)
  {
      printf("Failed to create GLFW window");
      glfwTerminate();
      return false;
  }
  glfwMakeContextCurrent(*window);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
  {
      printf("Failed to initialize GLAD");
      return false;
  }    

  glViewport(0, 0, 800, 600);
  glfwSetFramebufferSizeCallback(*window, framebufferSizeCallback);
  glfwSwapInterval(0);
  
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);

  glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

  printf("%s\n", glGetString(GL_VERSION));

  return true;
}

int main(void) {
  GLFWwindow* window;
  bool success = initOpenGLAndCreateWindow(&window);
  if (!success) {
    return -1;
  }

  GLuint shader_program = compileShader("resources/shaders/shader.vs", "resources/shaders/shader.fs");
  if (shader_program == 0) {
    return -1;
  }
  GLuint shader_program_no_texture = compileShader("resources/shaders/shader_no_texture.vs", "resources/shaders/shader_no_texture.fs");
  if (shader_program_no_texture == 0) {
    return -1;
  }

  GLuint sprite_textures = loadTexture("resources/sprites.png");
  GLuint sprite_vaos[] = { genTexturedGroundedSprite(0) };

  Sprite sprites[] = { { { 0.5, CHUNK_SIZE/2.0, -0.5 }, 0 },
                       { { 4.5, CHUNK_SIZE/2.0, 7.5 }, 0 },
                       { { -6.5, CHUNK_SIZE/2.0, -2.5 }, 0 } };


  GameData* game_data = (GameData*) malloc(sizeof(GameData));
  initChunk(&game_data->chunk);

  Chunk* chunk = &game_data->chunk;
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

    glUseProgram(shader_program_no_texture);
    GLuint transformLocation = glGetUniformLocation(shader_program, "view");
    glUniformMatrix4fv(transformLocation, 1, GL_TRUE, (float*)&view);
    transformLocation = glGetUniformLocation(shader_program, "projection");
    glUniformMatrix4fv(transformLocation, 1, GL_TRUE, (float*)&projection);
    transformLocation = glGetUniformLocation(shader_program, "model");
    glUniformMatrix4fv(transformLocation, 1, GL_TRUE, (float*)&model);

    ChunkRenderData* render_data = &(chunk->render_data);
    glBindVertexArray(render_data->vao);
    glDrawArrays(GL_TRIANGLES, 0, render_data->num_vertices);

    // Sprites
    glUseProgram(shader_program);
    transformLocation = glGetUniformLocation(shader_program, "view");
    glUniformMatrix4fv(transformLocation, 1, GL_TRUE, (float*)&view);
    transformLocation = glGetUniformLocation(shader_program, "projection");
    glUniformMatrix4fv(transformLocation, 1, GL_TRUE, (float*)&projection);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, sprite_textures);
    for (unsigned int i = 0; i < (sizeof(sprites) / sizeof(Sprite)); i++) {
      Sprite s = sprites[i];
      model = identity();
      model *= translate(s.position);
      model *= rotate(v3(0, 1, 0) * -y_rot);
      transformLocation = glGetUniformLocation(shader_program, "model");
      glUniformMatrix4fv(transformLocation, 1, GL_TRUE, (float*)&model);
      glBindVertexArray(sprite_vaos[s.index]);
      glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }

    glfwSwapBuffers(window);
    glfwPollEvents();    
  }

  free(game_data);
  glfwTerminate();
  return 0;
}
