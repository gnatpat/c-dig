void initPlayer(Player* player) {
  player->position = v3(LOADED_WORLD_SIZE*CHUNK_SIZE/2, LOADED_WORLD_SIZE*CHUNK_SIZE/2, LOADED_WORLD_SIZE*CHUNK_SIZE/2);
  player->pitch = 0.0f;
  player->yaw = 0.0f;
  player->y_speed = 0.0f;
}


float PLAYER_SPEED = 10;
float PLAYER_FLYING_SPEED = 70;

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
    V3 movement_direction = v3(0, 0, 0);
    if (isKeyDown(PLAYER_MOVE_FORWARD_KEY)) {
      movement_direction += xz_facing;
    }
    if (isKeyDown(PLAYER_MOVE_BACKWARD_KEY)) {
      movement_direction -= xz_facing;
    }
    if (isKeyDown(PLAYER_MOVE_LEFT_KEY)) {
      movement_direction += player_left;
    }
    if (isKeyDown(PLAYER_MOVE_RIGHT_KEY)) {
      movement_direction -= player_left;
    }

    movement_direction = normalise(movement_direction) * PLAYER_SPEED * dt;

    V3 slightly_below_player = player->position - v3(0, -0.0001f, 0);
    if (isPointAir(world, slightly_below_player)) {
      player->y_speed -= GRAVITY * dt;
      player->y_speed = fmaxf(-MAX_Y_SPEED, player->y_speed);
    }

    float y_direction = player->y_speed * dt;
    movement_direction.y = y_direction;

    V3 current_pos = player->position;
    do {
      V3 step = lenSq(movement_direction) > 1 ? normalise(movement_direction) : movement_direction;
      current_pos += movement_direction;
      movement_direction -= step;

      float block_height = getBlockHeightAtPoint(getBlockAt(world, toV3i(current_pos)).block_shape,
                                                 fractional_part(current_pos.x),
                                                 fractional_part(current_pos.z));
      if(fractional_part(current_pos.y) < block_height) {
        current_pos.y = int(current_pos.y) + block_height;
        movement_direction.y = 0.0f;
        player->y_speed = 0.0f;
      }
    } while (len(movement_direction) > 0.000001);
    player->position = current_pos;
  }
  /*float distance_left = PLAYER_SPEED;
  while(distance_left >= 0) {
    

  }*/
}


