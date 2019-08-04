void initInterface(Interface* interface) {
  initBasicRenderObject(&interface->block_focus, 20);
}

void updateInterface(GameData* game_data) {
  Interface* interface = &game_data->interface;
  Player* player= &game_data->player;

  MaybeV3i looking_at = player->focus;
  BasicRenderObject* block_focus = &interface->block_focus;
  if (looking_at.nothing) {
    block_focus->triangle_count = 0;
  } else {
    Block block = getBlockAt(&game_data->loaded_world, looking_at.value);
    block_focus->triangle_count = getBlockMesh(block_focus->triangles, block, looking_at.value);
    markDirty(block_focus);
  }
}

void renderInterface(Interface* interface, GLuint shader, Matrix4x4 view, Matrix4x4 projection) {
  render(&interface->block_focus, shader, view, projection, WHITE);
}

