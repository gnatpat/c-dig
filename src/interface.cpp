void initInterface(Interface* interface) {
  initBasicRenderObject(&interface->block_focus, 20);
  initBasicRenderObject(&interface->block_focus_face, 1);
  initBasicRenderObject(&interface->centre_point, 2);
  interface->centre_point.triangle_count = 2;
}

void updateInterface(GameData* game_data) {
  Interface* interface = &game_data->interface;
  Player* player= &game_data->player;

  MaybeRayTraceResult looking_at = player->focus;
  BasicRenderObject* block_focus = &interface->block_focus;
  BasicRenderObject* block_focus_face = &interface->block_focus_face;
  if (!looking_at.hit) {
    block_focus->triangle_count = 0;
    block_focus_face->triangle_count = 0;
  } else {
    Block block = getBlockAt(&game_data->loaded_world, looking_at.block_position);
    block_focus->triangle_count = getBlockMesh(block_focus->triangles, block, looking_at.block_position);
    blowOutRenderObject(block_focus, 0.0001);
    markDirty(block_focus);

    block_focus_face->triangle_count = 1;
    block_focus_face->triangles[0] = looking_at.hit_face;
    blowOutRenderObject(block_focus_face, 0.002);
    markDirty(block_focus_face);
  }
  

  interface->centre_point.triangle_count = 2;
  float size = 10;
  float width = size/SCREEN_WIDTH;
  float height = size/SCREEN_HEIGHT;
  interface->centre_point.triangles[0] = {{{width, 0.0, -1.0}, {0.0, height, -1.0}, {-width, 0.0, -1.0}}, {0.0, 0.0, 1.0}};
  interface->centre_point.triangles[1] = {{{-width, 0.0, -1.0}, {0.0, -height, -1.0}, {width, 0.0, -1.0}}, {0.0, 0.0, 1.0}};
  markDirty(&interface->centre_point);
}

void renderInterface(Interface* interface, GLuint shader, Matrix4x4 view, Matrix4x4 projection) {
  render(&interface->block_focus, shader, view, projection, WHITE);
  render(&interface->block_focus_face, shader, view, projection, RED);
  render(&interface->centre_point, shader, identity(), identity(), WHITE);
}

