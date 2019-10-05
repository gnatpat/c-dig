V3 EIGHT_CORNERS[8] = {v3(-1, -1, -1),
                       v3( 1, -1, -1),
                       v3(-1,  1, -1),
                       v3( 1,  1, -1),
                       v3(-1, -1,  1),
                       v3( 1, -1,  1),
                       v3(-1,  1,  1),
                       v3( 1,  1,  1)};

bool isPointInRect(V3 point, V3 rect_centre, V3 rectangle_radii) {
  V3 dist_from_rect_centre = abs(point - rect_centre);
  return (
      (dist_from_rect_centre.x) < rectangle_radii.x &&
      (dist_from_rect_centre.y) < rectangle_radii.y &&
      (dist_from_rect_centre.z) < rectangle_radii.z);
}

bool collidesAtTime(V3 start_position, V3 velocity, LoadedWorld* world, V3 rectangle_radii, float time) {
  // TODO: think about replacing this with GJK (https://caseymuratori.com/blog_0003)
  V3 position = start_position + velocity * time;

  for (int corner_index = 0; corner_index < 8; corner_index++)  {
    V3 corner = position + EIGHT_CORNERS[corner_index] * rectangle_radii;
    if (isPointSolid(world, corner)) {
      return true;
    }
  }

  V3i min_point = toV3i(position - rectangle_radii);
  V3i max_point = toV3i(position + rectangle_radii);
  for (int x = min_point.x; x <= max_point.x; x++) {
    for (int y = min_point.y; y <= max_point.y; y++) {
      for (int z = min_point.z; z <= max_point.z; z++) {
        printf("Checking block {%d, %d, %d}.\n", x, y, z);
        BlockModel model = BLOCK_MODELS[getBlockAt(world, v3i(x, y, z)).block_shape];
        V3 block_pos = v3(x, y, z);
        for (int triangle_index = 0; triangle_index < model.triangle_count; triangle_index++) {
          for (int vertex_index = 0; vertex_index < 3; vertex_index++) {
            if(isPointInRect(model.mesh[triangle_index].vertices[vertex_index] + block_pos, position, rectangle_radii)) {
              return true;
            }
          }
        }
      }
    }
  }
  return false;
}

V3 move(V3 position, V3 velocity, LoadedWorld* world, V3 rectangle_radii) {
  float t0 = 0.0;
  float t1 = 1.0;
  float time_checking = 1.0;
  while (t1 - t0 > 0.001) {
    bool collides_at_current_time = collidesAtTime(position, velocity, world, rectangle_radii, time_checking);
    if (collides_at_current_time) {
      t1 = time_checking;
    } else {
      t0 = time_checking;
    }
    time_checking = (t1 + t0)/2;
  }
  return position + velocity * t0;
}
