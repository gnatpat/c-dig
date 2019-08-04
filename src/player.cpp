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

    if (!player->on_ground) {
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
    player->speed.y -= GRAVITY * dt;
    player->speed.y = fmaxf(-MAX_Y_SPEED, player->speed.y);

    V3 velocity = player->speed * dt;
    if(lenSq(velocity) == 0.0) {
      return;
    }
    V3 y_velocity = v3(0, velocity.y, 0);
    V3 xz_velocity = v3(velocity.x, 0, velocity.z);
    float xz_distance = len(xz_velocity);
    xz_velocity.y = xz_distance * sqrt(2);
    y_velocity.y -= xz_distance * sqrt(2);

    V3 position = player->position;

    // The general movement code was taken from the algorithm from http://www.peroxide.dk/papers/collision/collision.pdf
    V3 espace_conversion = v3(0.5, PLAYER_HEIGHT/2, 0.5);
    Triangle triangles[500];
    int triangle_count = getMeshAroundPosition(&triangles[0],
                                               world,
                                               toV3i(position)-v3i(1, 2, 1),
                                               toV3i(position)+v3i(1, 2, 1));

    Triangle espace_triangles[500];
    for (int i = 0; i < triangle_count; i++) {
      espace_triangles[i] = toEspaceTriangle(triangles[i], espace_conversion);
    }

    V3 espace_position = position / espace_conversion;
    V3 y_espace_velocity = y_velocity / espace_conversion;
    V3 xz_espace_velocity = xz_velocity / espace_conversion;

    MoveResult result = move(espace_position, xz_espace_velocity, &espace_triangles[0], triangle_count, true);
    player->latest_collision = fromEspaceCollision(result.collision, espace_conversion);

    result = move(result.end_pos, y_espace_velocity, &espace_triangles[0], triangle_count, false);
    V3 espace_new_pos = result.end_pos;

    V3 new_pos = espace_new_pos * espace_conversion;

#ifdef MOVEMENT_DEBUG
    printf("===================\nAttempting to move from ");
    printV3(player->position);
    printf(" to ");
    printV3(player->position + velocity);
    printf(".\n");
    printf("Checking %d triangles.\n", triangle_count);
    printf("Ended up at ");
    printV3(new_pos);
    printf(".\n");
#endif

    player->position = new_pos;
    player->on_ground = result.on_ground;

    player->focus = getFirstNonAirBlockPosition(world, player->position + v3(0, PLAYER_HEIGHT/2, 0), player->facing, 5.0);
    if(!player->focus.nothing && leftMouseClicked()) {
      putBlockAt(world, player->focus.value, { AIR });
    }
  }
}
