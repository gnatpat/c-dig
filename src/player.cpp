bool isPointInTriangle(Triangle t, V3 pos) {
  V3 axis0 = t.vertices[1] - t.vertices[0];
  V3 axis1 = t.vertices[2] - t.vertices[0];
  V3 relative_pos = pos - t.vertices[0];

  float axis0_dist_sq = dot(axis0, axis0);
  float axis1_dist_sq = dot(axis1, axis1);

  float axis_0_of_pos = dot(axis0, relative_pos);
  float axis_1_of_pos = dot(axis1, relative_pos);

  float crossover = dot(axis0, axis1);

  float inv_denom = 1 / (axis0_dist_sq * axis1_dist_sq - crossover * crossover);
  float u = (axis1_dist_sq * axis_0_of_pos - crossover * axis_1_of_pos) * inv_denom;
  float v = (axis0_dist_sq * axis_1_of_pos - crossover * axis_0_of_pos) * inv_denom;

  return u >= 0 && v >= 0 && (u + v) < 1;
}

Triangle toEspaceTriangle(Triangle t, V3 espace_conversion) {
    Triangle espace_triangle = t;
    espace_triangle.vertices[0] /= espace_conversion;
    espace_triangle.vertices[1] /= espace_conversion;
    espace_triangle.vertices[2] /= espace_conversion;
    espace_triangle.normal /= espace_conversion;
    espace_triangle.normal = normalise(espace_triangle.normal);

    return espace_triangle;
}

float signedDistanceFromPlane(V3 normal, V3 point_on_plane, V3 p) {
  return dot(p - point_on_plane, normal);
}

float signedDistanceFromTrianglePlane(Triangle t, V3 p) {
  return signedDistanceFromPlane(t.normal, t.vertices[0], p);
}

QuadraticSolutions solveQuadratic(float a, float b, float c) {
  QuadraticSolutions solutions;
  if (a == 0.0 && b == 0.0) {
    solutions.solution_exists = false;
    return solutions;
  }

  float determinant = b*b - 4.0f*a*c;

  if (determinant < 0.0f) {
    solutions.solution_exists = false;
    return solutions;
  }

  float sqrtD = sqrt(determinant);
  float r1 = (-b - sqrtD) / (2*a);
  float r2 = (-b + sqrtD) / (2*a);

  if (r1 > r2) {
    float temp = r2;
    r2 = r1;
    r1 = temp;
  }

  solutions.solution_exists = true;
  solutions.min = r1;
  solutions.max = r2;
  return solutions;
}

struct MaybeCollision {
  bool collided;
  float time;
  V3 collision_point;
};

MaybeCollision vertexCollision(V3 vertex, V3 sphere_pos, V3 velocity) {
  MaybeCollision collision;

  V3 distance = sphere_pos - vertex;
  float a = lenSq(velocity);
  float b = 2 * dot(distance, velocity);
  float c = lenSq(distance) - 1;

  QuadraticSolutions solutions = solveQuadratic(a, b, c);
  if (!solutions.solution_exists || solutions.max < 0.0f || solutions.min > 1.0f) {
    collision.collided = false;
    return collision;
  }

  collision.collided = true;
  collision.collision_point = vertex;

  if (solutions.min < 0.0) {
    collision.time = solutions.max;
    return collision;
  }
  collision.time = solutions.min;
  return collision;
}


MaybeCollision lineCollision(V3 line_start, V3 line_end, V3 sphere_pos, V3 velocity) {
  MaybeCollision collision;

  V3 line_direction = normalise(line_end - line_start);
  V3 u = velocity - dot(velocity, line_direction) * line_direction;
  V3 q = sphere_pos - dot(sphere_pos - line_start, line_direction) * line_direction;

  float a = lenSq(u);
  float b = 2 * dot(u, q);
  float c = lenSq(q) - 1;

  QuadraticSolutions solutions = solveQuadratic(a, b, c);
  if (!solutions.solution_exists || solutions.max < 0.0f || solutions.min > 1.0f) {
    collision.collided = false;
    return collision;
  }

  float solution;

  if (solutions.min < 0.0) {
    solution = solutions.max;
  } else {
    solution = solutions.min;
  }
  if(isnan(solution)) {
    printf("quadratic solver returned nan in linecollision.\n");
  }

  float line_length = len(line_end - line_start);
  float proportion_through_line = 
    (dot(velocity, line_direction) * solution + dot(sphere_pos - line_start, line_direction)) / line_length;

  if(proportion_through_line < 0.0 || proportion_through_line > 1.0) {
    collision.collided = false;
    return collision;
  }

  collision.collided = true;
  collision.time = solution;
  collision.collision_point = line_start + proportion_through_line * line_direction;
  return collision;
}

MaybeCollision earliestCollision(MaybeCollision r1, MaybeCollision r2) {
  if (!r2.collided || r2.time < 0 || r2.time > 1) {
    return r1;
  }
  if (!r1.collided || r1.time < 0 || r1.time > 1) {
    return r2;
  }
  if (r1.time < r2.time) {
    return r1;
  }
  return r2;
}

float VELOCITY_EPSILON = 0.000001;
V3 move(V3 position, V3 velocity, Triangle* triangles, int triangle_count) {
  printf("Moving from ");
  printV3(position);
  printf(" with velocity ");
  printV3(velocity);
  printf(".\n");
  MaybeCollision first_collision;
  first_collision.collided = false;
  first_collision.time = 2.0;

  for (int i = 0; i < triangle_count; i++) {
    Triangle triangle = triangles[i];

    float relative_velocity = dot(triangle.normal, velocity);
    float signed_distance = signedDistanceFromTrianglePlane(triangle, position);
    float t0, t1;
    if (relative_velocity == 0.0) {
      if (fabsf(signed_distance) < 1.0) {
        t0 = 0.0;
        t1 = 1.0;
      } else {
        continue;
      }
    } else {
      t0 = (1 - signed_distance) / (relative_velocity);
      t1 = (-1 - signed_distance) / (relative_velocity);
    }
    if (t0 > t1) {
      continue;
    }
    if (t0 > 1.0 || t1 < 0.0) {
      continue;
    }

    V3 plane_intersection_point = position - triangle.normal + t0 * velocity;
    if(isPointInTriangle(triangle, plane_intersection_point)) {
      MaybeCollision collision;
      collision.collided = true;
      collision.time = t0;
      collision.collision_point = plane_intersection_point;

      first_collision = earliestCollision(first_collision, collision);
      continue;
    }

    V3* vs = &triangle.vertices[0];

    first_collision = earliestCollision(first_collision, vertexCollision(vs[0], position, velocity));
    first_collision = earliestCollision(first_collision, vertexCollision(vs[1], position, velocity));
    first_collision = earliestCollision(first_collision, vertexCollision(vs[2], position, velocity));
    first_collision = earliestCollision(first_collision, lineCollision(vs[0], vs[1], position, velocity));
    first_collision = earliestCollision(first_collision, lineCollision(vs[0], vs[2], position, velocity));
    first_collision = earliestCollision(first_collision, lineCollision(vs[1], vs[2], position, velocity));
  }

  if (!first_collision.collided) {
    V3 new_position = position + velocity;
    printf("No collision.\n");
    return new_position;
  }
  printf("Collision at %f!\n", first_collision.time);

  V3 sphere_position_at_collision = position + first_collision.time * velocity;

  V3 sliding_plane_normal = normalise(sphere_position_at_collision - first_collision.collision_point);
  V3 remaining_velocity = (1-first_collision.time) * velocity;
  printV3(remaining_velocity);
  printf(" remaining before slide.\n");
  remaining_velocity += dot(remaining_velocity, sliding_plane_normal) * sliding_plane_normal;
  printV3(remaining_velocity);
  printf(" remaining after slide.\n");

  if(len(remaining_velocity) < VELOCITY_EPSILON) {
    return sphere_position_at_collision;
  }

  return move(sphere_position_at_collision, remaining_velocity, triangles, triangle_count);
}


void initPlayer(Player* player) {
  player->position = v3(LOADED_WORLD_SIZE*CHUNK_SIZE/2, LOADED_WORLD_SIZE*CHUNK_SIZE/2, LOADED_WORLD_SIZE*CHUNK_SIZE/2);
  player->pitch = 0.0f;
  player->yaw = 0.0f;
  player->speed = v3(0, 0, 0);
  initDebugTriangles(&player->collision_triangles, 100);
}


float PLAYER_ACCELERATION_SPEED = 200.0f;
float PLAYER_FRICTION = 0.96f;
float PLAYER_JUMP_SPEED = 20.0f;
float PLAYER_JUMP_TIME = 0.1f;
float PLAYER_FLYING_SPEED = 70.0f;

// TODO - at some point, these need to be user defined settings
float PLAYER_YAW_LOOK_SENSITIVITY = 0.01;
float PLAYER_PITCH_LOOK_SENSITIVITY = 0.01;
void updatePlayer(Player* player, float dt, LoadedWorld* world) {
  if(isKeyPressed(SWITCH_TO_FLYING_MODE_KEY)) {
    player->flying = !player->flying;
  }

  V2 mouse_diff = mouseDifference();
  player->yaw -= mouse_diff.x * PLAYER_YAW_LOOK_SENSITIVITY - 2 * M_PI;
  while(player->yaw > 2 * M_PI) {
    player->yaw -= 2 * M_PI;
  }
  float pitch = player->pitch + mouse_diff.y * PLAYER_PITCH_LOOK_SENSITIVITY;
  player->pitch = clamp(-M_PI/2, pitch, M_PI/2);

  Quaternion yaw_rotation = from_vector_and_angle(v3(0, 1, 0), player->yaw);
  V3 xz_facing = rotate(yaw_rotation, v3(0, 0, -1));
  V3 player_left = cross(v3(0, 1, 0), xz_facing);
  Quaternion pitch_rotation = from_vector_and_angle(player_left, player->pitch);
  player->facing = rotate(pitch_rotation, xz_facing);

  if(player->flying) {
    V3 velocity = v3(0, 0, 0);
    if (isKeyDown(PLAYER_MOVE_FORWARD_KEY)) {
      velocity += player->facing;
    }
    if (isKeyDown(PLAYER_MOVE_BACKWARD_KEY)) {
      velocity -= player->facing;
    }
    if (isKeyDown(PLAYER_MOVE_LEFT_KEY)) {
      velocity += player_left;
    }
    if (isKeyDown(PLAYER_MOVE_RIGHT_KEY)) {
      velocity -= player_left;
    }
    player->position += normalise(velocity) * PLAYER_FLYING_SPEED * dt;
  } else {
    V3 acceleration_direction = v3(0, 0, 0);
    if (isKeyDown(PLAYER_MOVE_FORWARD_KEY)) {
      acceleration_direction += xz_facing;
    }
    if (isKeyDown(PLAYER_MOVE_BACKWARD_KEY)) {
      acceleration_direction -= xz_facing;
    }
    if (isKeyDown(PLAYER_MOVE_LEFT_KEY)) {
      acceleration_direction += player_left;
    }
    if (isKeyDown(PLAYER_MOVE_RIGHT_KEY)) {
      acceleration_direction -= player_left;
    }

    acceleration_direction = normalise(acceleration_direction) * PLAYER_ACCELERATION_SPEED * dt;
    player->speed += acceleration_direction;
    player->speed.x *= pow(PLAYER_FRICTION, dt * 400);
    player->speed.z *= pow(PLAYER_FRICTION, dt * 400);

    V3 slightly_below_player = player->position - v3(0, PLAYER_HEIGHT/2 + 0.05f, 0);
    if (isPointAir(world, slightly_below_player)) {
      player->speed.y -= GRAVITY * dt;
      player->speed.y = fmaxf(-MAX_Y_SPEED, player->speed.y);
      if (isKeyDown(PLAYER_JUMP_KEY)) {
        if (player->jump_timer > 0.0f) {
          player->jump_timer -= dt;
          player->speed.y = PLAYER_JUMP_SPEED;
        }
      } else {
        player->jump_timer = 0.0;
      }
    } else {
      player->jump_timer = PLAYER_JUMP_TIME;
      if (isKeyDown(PLAYER_JUMP_KEY)) {
        player->speed.y = PLAYER_JUMP_SPEED;
      } else {
        player->speed.y = 0.0f;
      }
    }

    V3 velocity = player->speed * dt;
    if(lenSq(velocity) == 0.0) {
      return;
    }

    V3 position = player->position;

    // The code below was taken from the algorithm from http://www.peroxide.dk/papers/collision/collision.pdf.

    V3 espace_conversion = v3(0.5, PLAYER_HEIGHT/2, 0.5);
    Triangle triangles[500];
    int triangle_count = getMeshAroundPosition(&triangles[0],
                                               world,
                                               toV3i(position)-v3i(1, 2, 1),
                                               toV3i(position)+v3i(1, 2, 1));

    Triangle espace_triangles[500];
    printf("Checking %d triangles.\n", triangle_count);
    for (int i = 0; i < triangle_count; i++) {
      espace_triangles[i] = toEspaceTriangle(triangles[i], espace_conversion);
    }

    V3 espace_position = position / espace_conversion;
    V3 espace_velocity = velocity / espace_conversion;

    V3 espace_new_pos = move(espace_position, espace_velocity, &espace_triangles[0], triangle_count);

    V3 new_pos = espace_new_pos * espace_conversion;
    player->position = new_pos;
  }
}
