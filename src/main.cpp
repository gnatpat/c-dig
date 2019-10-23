#include "includes.cpp"

void* newChunkRenderMain(void* game_data_as_void_pointer) {
  GameData* game_data = (GameData*) game_data_as_void_pointer;
  WorldRenderState* render_state = &game_data->loaded_world.render_state;

  // The Mutex lock/unlock logic in here is a bit hairy. In some ways it's nice - it shows that we only unlock
  // the mutex whilst rendering the chunk (and implicitly when waiting for the condition), but having the first lock
  // outside the loop and only unlocking inside two inner loops doesn't seem right, and it takes some thinking to
  // convince yourself the logic is correct.
  Mutex* new_chunk_lock = &render_state->new_chunk_lock;
  lockMutex(new_chunk_lock);
  while (true) {
    Chunk* chunk = (Chunk*) removefromLinkedList(&render_state->new_chunks);
    while (chunk != NULL) {
      unlockMutex(new_chunk_lock);
      fillChunkRenderData(chunk, &game_data->loaded_world);
      pthread_testcancel();
      lockMutex(new_chunk_lock);
      chunk = (Chunk*) removefromLinkedList(&render_state->new_chunks);
    }
    waitForCondition(&render_state->new_chunk_condition, new_chunk_lock);
  }
}

int main(void) {
  setbuf(stdout, NULL);
  GLFWwindow* window;
  bool success = initOpenGLAndCreateWindow(&window);
  if (!success) {
    return -1;
  }

  GLuint terrain_shader = compileShaderOrDie("resources/shaders/terrain_shader.vs",
                                             "resources/shaders/terrain_shader.fs");
  GLuint debug_triangle_shader = compileShaderOrDie("resources/shaders/debug_triangles.vs",
                                                    "resources/shaders/debug_triangles.fs");
  GLuint text_shader = compileShaderOrDie("resources/shaders/text_shader.vs",
                                          "resources/shaders/text_shader.fs");

  GameData* game_data = (GameData*) calloc(1, sizeof(GameData));
  initWorld(&game_data->loaded_world);
  initInterface(&game_data->interface);

  pthread_t chunk_render_thread;
  pthread_create(&chunk_render_thread, NULL, newChunkRenderMain, game_data);

  initInput();
  initPlayer(&game_data->player);

  bool block_viewer_mode = false;
  BlockViewerData* block_viewer_data = (BlockViewerData*) calloc(1, sizeof(BlockViewerData));
  initBlockViewer(block_viewer_data);

  bool debug_mode = false;
  DebugData* debug_data = (DebugData*) calloc(1, sizeof(DebugData));
  initDebugData(debug_data);

  Font* font = (Font*) calloc(1, sizeof(Font));
  loadFontOrDie(font, "resources/font/lato.png", "resources/font/lato.fnt");
  Font* font2 = (Font*) calloc(1, sizeof(Font));
  loadFontOrDie(font2, "resources/font/Suruma.png", "resources/font/Suruma.fnt");

  Text test_text = createText("The quick brown fox jumps over the lazy dog.", font);
  Text test_text2 = createText("The quick brown fox jumps over the lazy dog.", font2);

  float t = 0.0;

  int frames = 0;
  float acc = 0;

  struct timespec old_time, new_time;
  clock_gettime(CLOCK_MONOTONIC_RAW, &old_time);

  while(!glfwWindowShouldClose(window) && !isKeyDown(QUIT_KEY))
  {
    clock_gettime(CLOCK_MONOTONIC_RAW, &new_time);
    float delta = (new_time.tv_sec - old_time.tv_sec) + (new_time.tv_nsec - old_time.tv_nsec) / 1000000000.0f;
    old_time = new_time;


    frames += 1;
    acc += delta;
    if (acc >= 1.0) {
      printf("fps: %2.2f\n", frames/acc);
      acc = 0;
      frames = 0;
    }

    t += delta;

    // UPDATE

    if (isKeyPressed(SWITCH_TO_BLOCK_VIEWER_KEY)) {
      block_viewer_mode = !block_viewer_mode;
    }

    if (block_viewer_mode) {
      updateBlockViewer(block_viewer_data);
    } else {
      if (isKeyPressed(SWITCH_TO_DEBUG_MODE_KEY)) {
        debug_mode = !debug_mode;
      }

      updatePlayer(&game_data->player, delta, &game_data->loaded_world);
      updateInterface(game_data);

      if (debug_mode) {
        debugMeshAroundPlayer(&debug_data->mesh_around_player, game_data);
        debugCollision(&debug_data->collision_triangles, game_data);
      }
    }

    // RENDER
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (block_viewer_mode) {
      renderBlockViewer(block_viewer_data, t, terrain_shader);
    } else {
      Player* player = &game_data->player;
      Matrix4x4 yaw_rotation = rotate(v3(0, 1, 0) * player->yaw);
      Matrix4x4 pitch_rotation = rotate(v3(1, 0, 0) * player->pitch);

      Matrix4x4 view = identity();
      view *= pitch_rotation;
      view *= yaw_rotation;
      view *= translate(-game_data->player.position);
      view *= translate(v3(-0.0, -PLAYER_HEIGHT/2, 0.0));

      float ratio = float(SCREEN_WIDTH) / float(SCREEN_HEIGHT);
      Matrix4x4 projection = perspective_projection(0.1, 1000.0, 45.0, ratio);

      renderWorld(&game_data->loaded_world, terrain_shader, view, projection);
      renderInterface(&game_data->interface, debug_triangle_shader, view, projection);

      if(debug_mode) {
        renderDebug(debug_data, debug_triangle_shader, view, projection);
      }
      renderText(test_text, v2(-0.5, 0.0), 0.06, text_shader);
      renderText(test_text2, v2(-0.5, 0.25), 0.06, text_shader);
    }

    glfwSwapBuffers(window);

    updateInput();
    glfwPollEvents();    
  }

  pthread_cancel(chunk_render_thread);
  pthread_join(chunk_render_thread, NULL);
  free(game_data);
  free(block_viewer_data);
  glfwTerminate();
  return 0;
}

