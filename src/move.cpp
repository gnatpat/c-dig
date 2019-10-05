V3 EIGHT_CORNERS[8] = {v3(-1, -1, -1),
                       v3( 1, -1, -1),
                       v3(-1,  1, -1),
                       v3( 1,  1, -1),
                       v3(-1, -1,  1),
                       v3( 1, -1,  1),
                       v3(-1,  1,  1),
                       v3( 1,  1,  1)};

bool collidesAtTime(V3 start_position, V3 velocity, LoadedWorld* world, V3 rectangle_radii, float time) {
  V3 position = start_position + velocity * time;

  for (int corner_index = 0; corner_index < 8; corner_index++)  {
    V3 corner = position + EIGHT_CORNERS[corner_index] * rectangle_radii;
    if (isPointSolid(world, corner)) {
      return true;
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
