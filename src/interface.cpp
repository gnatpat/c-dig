void initInterface(Interface* interface) {
  initBasicRenderObject(&interface->block_focus, 20);
  initBasicRenderObject(&interface->block_focus_face, 1);
  initBasicRenderObject(&interface->centre_point, 2);
  interface->centre_point.triangle_count = 2;

  initBasicRenderObject(&interface->direction_dongle, 12);
  interface->direction_dongle.triangle_count = 12;
  make3dTriangle(interface->direction_dongle.triangles, v3(1, 0, 0), v3(0, 1, 0) * 0.2, RED, 0.2f);
  make3dTriangle(interface->direction_dongle.triangles + 4, v3(0, 1, 0), v3(1, 0, 0) * 0.2, GREEN, 0.2f);
  make3dTriangle(interface->direction_dongle.triangles + 8, v3(0, 0, 1), v3(0, 1, 0) * 0.2, BLUE, 0.2f);
  markDirty(&interface->direction_dongle);
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
    Triangle block_mesh[20];
    Block block = getBlockAt(&game_data->loaded_world, looking_at.block_position);
    block_focus->triangle_count = getBlockMesh(&(block_mesh[0]), block, looking_at.block_position);
    ObjectTriangle* cursor = block_focus->triangles;
    for (int i = 0; i < block_focus->triangle_count; i++) {
      Triangle t = block_mesh[i];
      memcpy(cursor->vertices, t.vertices, 3 * sizeof(V3));
      cursor->colour = WHITE;
      cursor->normal = t.normal;
      cursor->alpha = 0.2f;
      cursor++;
    }
    blowOutRenderObject(block_focus, 0.0001);
    markDirty(block_focus);

    block_focus_face->triangle_count = 1;
    cursor = block_focus_face->triangles;
    memcpy(cursor->vertices, looking_at.hit_face.vertices, 3 * sizeof(V3));
    cursor->colour = RED;
    cursor->normal = looking_at.hit_face.normal;
    cursor->alpha = 0.2f;
    blowOutRenderObject(block_focus_face, 0.002);
    markDirty(block_focus_face);
  }
  

  interface->centre_point.triangle_count = 2;
  float size = 10;
  float width = size/SCREEN_WIDTH;
  float height = size/SCREEN_HEIGHT;
  interface->centre_point.triangles[0] = {{{width, 0.0, -1.0}, {0.0, height, -1.0}, {-width, 0.0, -1.0}}, {0.0, 0.0, 1.0}, {1.0, 1.0, 1.0}, 0.2};
  interface->centre_point.triangles[1] = {{{-width, 0.0, -1.0}, {0.0, -height, -1.0}, {width, 0.0, -1.0}}, {0.0, 0.0, 1.0}, {1.0, 1.0, 1.0}, 0.2};
  markDirty(&interface->centre_point);

  interface->pitch = player->pitch;
  interface->yaw = player->yaw;
}

void renderInterface(Interface* interface, GLuint shader, Matrix4x4 view, Matrix4x4 projection) {
  //render(&interface->block_focus, shader, view, projection);
  render(&interface->block_focus_face, shader, view, projection);

  Matrix4x4 direction_dongle_model = identity();
  direction_dongle_model *= projection;
  direction_dongle_model *= translate(v3(0, 0, -1.0));
  direction_dongle_model *= scale(v3(0.04, 0.04, 0.04));
  direction_dongle_model *= rotate(v3(1, 0, 0) * interface->pitch);
  direction_dongle_model *= rotate(v3(0, 1, 0) * interface->yaw);

  glClear(GL_DEPTH_BUFFER_BIT);
  render(&interface->direction_dongle, shader, direction_dongle_model, identity(), identity());

  glDisable(GL_DEPTH_TEST);
  render(&interface->centre_point, shader, identity(), identity());
  glEnable(GL_DEPTH_TEST);
}

