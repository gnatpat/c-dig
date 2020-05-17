void initDebugData(DebugData* debug_data) {
  initBasicRenderObject(&debug_data->mesh_around_player, 1000);
  initBasicRenderObject(&debug_data->collision_triangles, 60);
}

void debugMeshAroundPlayer(BasicRenderObject* debug_triangles, GameData* game_data) {
  /*debug_triangles->triangle_count = getMeshAroundPosition(debug_triangles->triangles,
                                                          &game_data->loaded_world,
                                                          toV3i(game_data->player.position)-v3i(1, 1, 1),
                                                          toV3i(game_data->player.position)+v3i(1, 2, 1));
  markDirty(debug_triangles);*/
}

void debugCollision(BasicRenderObject* debug_triangles, GameData* game_data) {
  /*if(!DEBUG_RESULT.hit) {
    debug_triangles->triangle_count = 0;
    return;
  }
  debug_triangles->triangle_count = 1;
  debug_triangles->triangles[0] = DEBUG_RESULT.hit_face;
  markDirty(debug_triangles);*/
}

void renderDebug(DebugData* debug_data, GLuint shader, Matrix4x4 view, Matrix4x4 projection) {
  //render(&debug_data->mesh_around_player, shader, view, projection, WHITE);
  //render(&debug_data->collision_triangles, shader, view, projection, RED);
}

