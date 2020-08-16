const int EXTRA_INSIDE_POINTS = 5;

void initMinecartTrackModels(MinecartTracks* trackModels) {
  BasicRenderObject* straight = &trackModels->straight;
  initBasicRenderObject(straight, 2);

  V3 corner1 = v3(0.25, 0.01, 0.0);
  V3 corner2 = v3(0.75, 0.01, 0.0);
  V3 corner3 = v3(0.25, 0.01, 1.0);
  V3 corner4 = v3(0.75, 0.01, 1.0);
  straight->triangles[0].vertices[0] = corner1;
  straight->triangles[0].vertices[1] = corner4;
  straight->triangles[0].vertices[2] = corner2;
  straight->triangles[0].normal = v3(0, 1.0, 0);
  straight->triangles[0].colour = v3(0.5, 0.5, 0.5);
  straight->triangles[0].alpha = 1.0f;
  straight->triangles[1].vertices[0] = corner1;
  straight->triangles[1].vertices[1] = corner3;
  straight->triangles[1].vertices[2] = corner4;
  straight->triangles[1].normal = v3(0, 1.0, 0);
  straight->triangles[1].colour = v3(0.5, 0.5, 0.5);
  straight->triangles[1].alpha = 1.0f;
  straight->triangle_count = 2;
  markDirty(straight);

  BasicRenderObject* curve = &trackModels->curve;
  initBasicRenderObject(curve, EXTRA_INSIDE_POINTS * 2 + 3);

  V3 inside = v3(0.25, 0.01, 0.0);
  V3 outside = v3(0.75, 0.01, 0.0);
  float outside_angle = 0.0f;
  float inside_angle = 0.0f;
  int i = 0;
  for (i = 0; i < EXTRA_INSIDE_POINTS + 1; i++) {
    outside_angle = (M_PI / 2) * (i+1) / (EXTRA_INSIDE_POINTS + 2);
    V3 new_outside = v3(cos(outside_angle), 0, sin(outside_angle)) * 0.75;
    new_outside.y = 0.01;
    printV3(new_outside);
    printf("\nOutside angle: %.2f\n", outside_angle);
    curve->triangles[i*2].vertices[0] = outside;
    curve->triangles[i*2].vertices[1] = inside;
    curve->triangles[i*2].vertices[2] = new_outside;
    curve->triangles[i*2].normal = v3(0, 1.0, 0);
    curve->triangles[i*2].colour = v3(0.5, 0.5, 0.5);
    curve->triangles[i*2].alpha = 1.0f;
    outside = new_outside;

    inside_angle = (M_PI / 2) * (i+1) / (EXTRA_INSIDE_POINTS + 1);
    V3 new_inside = v3(cos(inside_angle), 0, sin(inside_angle)) * 0.25;
    new_inside.y = 0.01;
    printV3(new_inside);
    printf("\nInside angle: %.2f\n", inside_angle);
    curve->triangles[i*2 + 1].vertices[0] = inside;
    curve->triangles[i*2 + 1].vertices[1] = new_inside;
    curve->triangles[i*2 + 1].vertices[2] = outside;
    curve->triangles[i*2 + 1].normal = v3(0, 1.0, 0);
    curve->triangles[i*2 + 1].colour = v3(0.5, 0.5, 0.5);
    curve->triangles[i*2 + 1].alpha = 1.0f;
    inside = new_inside;
  }
  outside_angle = (M_PI / 2) * (i+1) / (EXTRA_INSIDE_POINTS + 2);
  V3 new_outside = v3(cos(outside_angle), 0.0, sin(outside_angle)) * 0.75;
  new_outside.y = 0.01;
  curve->triangles[i*2].vertices[0] = outside;
  curve->triangles[i*2].vertices[1] = inside;
  curve->triangles[i*2].vertices[2] = new_outside;
  curve->triangles[i*2].normal = v3(0, 1.0, 0);
  curve->triangles[i*2].colour = v3(0.5, 0.5, 0.5);
  curve->triangles[i*2].alpha = 1.0f;

  curve->triangle_count = EXTRA_INSIDE_POINTS * 2 + 3;
  markDirty(curve);
}
