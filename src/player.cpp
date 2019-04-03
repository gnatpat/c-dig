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
    V3 movement_direction = v3(0, 0, 0);
    if (isKeyDown(PLAYER_MOVE_FORWARD_KEY)) {
      movement_direction += player->facing;
    }
    if (isKeyDown(PLAYER_MOVE_BACKWARD_KEY)) {
      movement_direction -= player->facing;
    }
    if (isKeyDown(PLAYER_MOVE_LEFT_KEY)) {
      movement_direction += player_left;
    }
    if (isKeyDown(PLAYER_MOVE_RIGHT_KEY)) {
      movement_direction -= player_left;
    }
    player->position += normalise(movement_direction) * PLAYER_FLYING_SPEED * dt;
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

    V3 movement_direction = player->speed * dt;

    V3 current_pos = player->position;
    while (len(movement_direction) > 0.0001) {
      V3 next_whole_int = floor(current_pos) + 0.5 + copysign(v3(0.5, 0.5, 0.5), movement_direction);
      V3 dist = (next_whole_int - current_pos) / normalise(movement_direction);
      float step_dist = min(dist);
      if(step_dist == 0.0) {
        step_dist = 0.01;
      }
      V3 step = len(movement_direction) > step_dist ? normalise(movement_direction) * step_dist : movement_direction;
      step_dist = len(step);
      current_pos += step;
      movement_direction -= step;

      float block_height = getBlockHeightAtPoint(getBlockAt(world, toV3i(current_pos)).block_shape,
                                                 fractional_part(current_pos.x),
                                                 fractional_part(current_pos.z));
      if(fractional_part(current_pos.y) < block_height) {
        float y_diff = block_height - fractional_part(current_pos.y);
        V3 potential_endpoint = current_pos + v3(0, y_diff, 0);
        if(y_diff < 2 * step_dist && isPointAir(world, potential_endpoint+v3(0, 0.001, 0))) {
          current_pos.y = int(current_pos.y) + block_height;
          movement_direction.y = 0.0f;
          player->speed.y = 0.0f;
        } else {
          V3i block_diff = toV3i(current_pos) - toV3i(current_pos - step);
          movement_direction += movement_direction * -toV3(block_diff);
          current_pos -= step;
        }
      }
    }
    player->position = current_pos;
  }
}


