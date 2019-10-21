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


struct CollisionResult {
  bool collides;
  V3 point_causing_collision;
  bool player_in_block;
};

float MOVEMENT_EPSILON = 0.0001;
V3 move(V3 position, V3 velocity, LoadedWorld* world, V3 rectangle_radii, bool slide) {
  V3 normalised_velocity = normalise(velocity);
  float velocity_distance = len(velocity);

  MaybeRayTraceResult closest_result;
  closest_result.hit = false;
  closest_result.distance = velocity_distance;

  for (int corner_index = 0; corner_index < 8; corner_index++)  {
    V3 corner = position + EIGHT_CORNERS[corner_index] * rectangle_radii;
    MaybeRayTraceResult ray_trace_result = blockRayTrace(world, corner, normalised_velocity, velocity_distance, false);
    closest_result = getClosestResult(closest_result, ray_trace_result);
  }

  V3 new_position = position + normalised_velocity * closest_result.distance;
  V3 remaining_velocity = normalised_velocity * (velocity_distance - closest_result.distance);

  //printf("Moving from ");
  //printV3(position);
  //printf("\nWith velocity ");
  //printV3(velocity);
  //printf("\nEnded at ");
  //printV3(new_position);
  //printf("\n");
  //printf("Closest result distance: %.7f\n", closest_result.distance);
  //printf("Closest result hit pos ");
  //printV3(closest_result.hit_position);
  //printf("\n");

  if(len(remaining_velocity) < MOVEMENT_EPSILON) {
    return new_position;
  }

  if (slide && false) {
    V3 collision_normal = closest_result.hit_face.normal;

    V3 new_velocity = remaining_velocity - dot(remaining_velocity, collision_normal) * collision_normal;
    
    printf("Moving: ");
    printV3(velocity);
    printf("\nCollision normal: ");
    printV3(collision_normal);
    printf("\nNew moving: ");
    printV3(new_velocity);
    printf("\n");
    return move(new_position, new_velocity, world, rectangle_radii, slide);
  }

  return new_position;
}
