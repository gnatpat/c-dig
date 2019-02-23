void initPlayer(Player* player) {
  player->position = v3(0, 0, 0);
  player->pitch = 0.0f;
  player->yaw = 0.0f;
}


float PLAYER_SPEED = 50;
// TODO - at some point, these need to be user defined settings
float PLAYER_YAW_LOOK_SENSITIVITY = 0.01;
float PLAYER_PITCH_LOOK_SENSITIVITY = 0.01;
void updatePlayer(Player* player, float dt) {
  V2 mouseDiff = mouseDifference();
  player->yaw -= mouseDiff.x * PLAYER_YAW_LOOK_SENSITIVITY - 2 * M_PI;
  while(player->yaw > 2 * M_PI) {
    player->yaw -= 2 * M_PI;
  }
  float pitch = player->pitch + mouseDiff.y * PLAYER_PITCH_LOOK_SENSITIVITY;
  player->pitch = clamp(-M_PI/2, pitch, M_PI/2);

  Quaternion yaw_rotation = from_vector_and_angle(v3(0, 1, 0), player->yaw);
  V3 xz_facing = rotate(yaw_rotation, v3(0, 0, -1));
  V3 player_right = cross(v3(0, 1, 0), xz_facing);
  Quaternion pitch_rotation = from_vector_and_angle(player_right, player->pitch);
  player->facing = -rotate(pitch_rotation, xz_facing);

  V3 movement_direction = v3(0, 0, 0);
  if (isKeyDown(PLAYER_MOVE_FORWARD_KEY)) {
    movement_direction += player->facing;
  }
  if (isKeyDown(PLAYER_MOVE_BACKWARD_KEY)) {
    movement_direction -= player->facing;
  }
  if (isKeyDown(PLAYER_MOVE_LEFT_KEY)) {
    movement_direction -= player_right;
  }
  if (isKeyDown(PLAYER_MOVE_RIGHT_KEY)) {
    movement_direction += player_right;
  }
  player->position += normalise(movement_direction) * PLAYER_SPEED * dt;
}


