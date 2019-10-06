void initInterface(Interface* interface) {
  initBasicRenderObject(&interface->block_focus, 20);
}

void updateInterface(GameData* game_data) {
  Interface* interface = &game_data->interface;
  Player* player= &game_data->player;

  MaybeRayTraceResult looking_at = player->focus;
  BasicRenderObject* block_focus = &interface->block_focus;
  if (!looking_at.hit) {
    block_focus->triangle_count = 0;
  } else {
    Block block = getBlockAt(&game_data->loaded_world, looking_at.block_position);
    block_focus->triangle_count = getBlockMesh(block_focus->triangles, block, looking_at.block_position);
    markDirty(block_focus);
  }
}

void renderInterface(Interface* interface, GLuint shader, Matrix4x4 view, Matrix4x4 projection) {
  render(&interface->block_focus, shader, view, projection, WHITE);
}

