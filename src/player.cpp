bool isPointInTriangle(Triangle t, V3 pos) {
  V3 axis0 = t.vertices[1] - t.vertices[0];
  V3 axis1 = t.vertices[2] - t.vertices[0];
  V3 relative_pos = pos - t.vertices[0];
  if(dot(relative_pos, t.normal) != 0.0) {
    return false;
  }

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


float signedDistance(Triangle t, V3 p) {
  return dot(t.normal, p) - (t.normal.x * t.vertices[0].x +
                             t.normal.y * t.vertices[0].y +
                             t.normal.z * t.vertices[0].z);
}


void initPlayer(Player* player) {
  player->position = v3(LOADED_WORLD_SIZE*CHUNK_SIZE/2, LOADED_WORLD_SIZE*CHUNK_SIZE/2, LOADED_WORLD_SIZE*CHUNK_SIZE/2);
  player->pitch = 0.0f;
  player->yaw = 0.0f;
  player->speed = v3(0, 0, 0);
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
    V3 movement_velocity = v3(0, 0, 0);
    if (isKeyDown(PLAYER_MOVE_FORWARD_KEY)) {
      movement_velocity += player->facing;
    }
    if (isKeyDown(PLAYER_MOVE_BACKWARD_KEY)) {
      movement_velocity -= player->facing;
    }
    if (isKeyDown(PLAYER_MOVE_LEFT_KEY)) {
      movement_velocity += player_left;
    }
    if (isKeyDown(PLAYER_MOVE_RIGHT_KEY)) {
      movement_velocity -= player_left;
    }
    player->position += normalise(movement_velocity) * PLAYER_FLYING_SPEED * dt;
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

    V3 slightly_below_player = player->position - v3(0, 0.0001f, 0);
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

    V3 movement_velocity = player->speed * dt;

    V3 current_pos = player->position + movement_velocity;

    // The code below was taken from the algorithm from http://www.peroxide.dk/papers/collision/collision.pdf.

    Triangle triangles[500];
    int triangle_count = getMeshAroundPosition(&triangles[0],
                                               world,
                                               toV3i(current_pos)-v3i(1, 1, 1),
                                               toV3i(current_pos)+v3i(1, 2, 1));

    V3 espace_movement_velocity = movement_velocity / v3(1, PLAYER_HEIGHT, 1);
    V3 espace_current_pos = current_pos / v3(1, PLAYER_HEIGHT, 1);
    bool collision = false;
    float earliest_t0 = 1.0f;
    for (int i = 0; i < triangle_count; i++) {
      Triangle espace_triangle = triangles[i];
      espace_triangle.vertices[0].y /= PLAYER_HEIGHT;
      espace_triangle.vertices[1].y /= PLAYER_HEIGHT;
      espace_triangle.vertices[2].y /= PLAYER_HEIGHT;
      espace_triangle.normal.y /= PLAYER_HEIGHT;
      espace_triangle.normal = normalise(espace_triangle.normal);

      float relative_velocity = dot(espace_triangle.normal, espace_movement_velocity);
      float signed_distance = signedDistance(espace_triangle, espace_current_pos);
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
      if ((t0 < 0.0 || t0 > 1.0) && (t1 < 0.0 || t1 > 1.0)) {
        continue;
      }

      if (t0 < earliest_t0) {
        if(t0 == 0.0) {
          printf("Beginning intersection?\n");
        }
        V3 plane_intersection_point = espace_current_pos - espace_triangle.normal + t0 * espace_movement_velocity;
        if(isPointInTriangle(espace_triangle, plane_intersection_point)) {
          collision = true;
          earliest_t0 = t0;
        }
      }
    }
    if (collision) {
      current_pos += earliest_t0 * movement_velocity;
    }
    player->position = current_pos;
    printV3(player->position);
  }
}
