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
  
CollisionResult collidesAtTime(V3 start_position, V3 velocity, LoadedWorld* world, V3 rectangle_radii, float time) {
  // TODO: think about replacing this with GJK (https://caseymuratori.com/blog_0003)
  V3 position = start_position + velocity * time;

  for (int corner_index = 0; corner_index < 8; corner_index++)  {
    V3 corner = position + EIGHT_CORNERS[corner_index] * rectangle_radii;
    if (isPointSolid(world, corner)) {
      CollisionResult result;
      result.collides = true;
      result.point_causing_collision = corner;
      result.player_in_block = true;
      return result;
    }
  }

  //V3i min_point = toV3i(position - rectangle_radii);
  //V3i max_point = toV3i(position + rectangle_radii);
  //for (int x = min_point.x; x <= max_point.x; x++) {
  //  for (int y = min_point.y; y <= max_point.y; y++) {
  //    for (int z = min_point.z; z <= max_point.z; z++) {
  //      BlockModel model = BLOCK_MODELS[getBlockAt(world, v3i(x, y, z)).block_shape];
  //      V3 block_pos = v3(x, y, z);
  //      for (int triangle_index = 0; triangle_index < model.triangle_count; triangle_index++) {
  //        for (int vertex_index = 0; vertex_index < 3; vertex_index++) {
  //          V3 mesh_vertex = model.mesh[triangle_index].vertices[vertex_index] + block_pos;
  //          if(isPointInRect(mesh_vertex, position, rectangle_radii)) {
  //            CollisionResult result;
  //            result.collides = true;
  //            result.point_causing_collision = mesh_vertex;
  //            result.player_in_block = false;
  //            return result;
  //          }
  //        }
  //      }
  //    }
  //  }
  //}
  CollisionResult result;
  result.collides = false;
  return result;
}

float MOVEMENT_EPSILON = 0.0001;
V3 move(V3 position, V3 velocity, LoadedWorld* world, V3 rectangle_radii, bool slide) {
  float t0 = 0.0;
  float t1 = 1.0;
  float time_checking = 1.0;
  CollisionResult latest_collision;
  latest_collision.collides = false;
  while ((t1 - t0) * len(velocity) > MOVEMENT_EPSILON) {
    CollisionResult result = collidesAtTime(position, velocity, world, rectangle_radii, time_checking);
    if (result.collides) {
      t1 = time_checking;
      latest_collision = result;
    } else {
      t0 = time_checking;
    }
    time_checking = (t1 + t0)/2;
  }
  V3 new_position = position + velocity * t0;
  printf("Moving from ");
  printV3(position);
  printf("\nWith velocity ");
  printV3(velocity);
  printf("\nEnded at ");
  printV3(new_position);
  printf("\nt0 is %.2f.\n", t0);
  if (slide && latest_collision.collides && latest_collision.player_in_block && t0 > 0.0) {
    V3 remaining_velocity = velocity * (1-t0);
    MaybeRayTraceResult ray_trace_result = blockRayTrace(world, latest_collision.point_causing_collision, -normalise(velocity), MOVEMENT_EPSILON);
    if (!ray_trace_result.hit) {
      ray_trace_result = blockRayTrace(world, latest_collision.point_causing_collision, normalise(velocity), MOVEMENT_EPSILON);
    }

    assert(ray_trace_result.hit);
    V3 collision_normal = ray_trace_result.hit_face.normal;

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
