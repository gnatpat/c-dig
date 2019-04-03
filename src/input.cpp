// Input is one of those things were I just feel like it should be global, you know? I *could* encapsulate all this
// in an Input stuct and pass it around, but at some point I don't really understand why. Readonly globals that really
// are globals don't seem too bad. Maybe?

static bool keys_pressed[INPUT_KEY_COUNT];
static bool keys_down[INPUT_KEY_COUNT];
static bool keys_released[INPUT_KEY_COUNT];
static int key_mappings[INPUT_KEY_COUNT];

static V2 mouse_d;
static V2 mouse_pos;


bool isKeyDown(InputKey key) {
  return keys_down[key];
}

bool isKeyPressed(InputKey key) {
  return keys_pressed[key];
}

bool isKeyReleased(InputKey key) {
  return keys_released[key];
}

V2 mouseDifference() {
  return mouse_d;
}

void initInputKeyMappings() {
  key_mappings[PLAYER_MOVE_FORWARD_KEY] = GLFW_KEY_W;
  key_mappings[PLAYER_MOVE_BACKWARD_KEY] = GLFW_KEY_S;
  key_mappings[PLAYER_MOVE_LEFT_KEY] = GLFW_KEY_A;
  key_mappings[PLAYER_MOVE_RIGHT_KEY] = GLFW_KEY_D;
  key_mappings[PLAYER_JUMP_KEY] = GLFW_KEY_SPACE;
  key_mappings[SWITCH_TO_BLOCK_VIEWER_KEY] = GLFW_KEY_B;
  key_mappings[SWITCH_TO_FLYING_MODE_KEY] = GLFW_KEY_F;
  key_mappings[SWITCH_TO_DEBUG_MODE_KEY] = GLFW_KEY_TAB;
  key_mappings[QUIT_KEY] = GLFW_KEY_Q;
}

void initInput() {
  initInputKeyMappings();
  // I don't think this actually needs to happen
  /*for(int key = 0; key < INPUT_KEY_COUNT; key++) {
    keys_pressed[key] = false;
    keys_released[key] = false;
    keys_down[key] = false;
  }*/
}

void updateInput() {
  for(int key = 0; key < INPUT_KEY_COUNT; key++) {
    keys_pressed[key] = false;
    keys_released[key] = false;
  }
  mouse_d.x = 0.0f;
  mouse_d.y = 0.0f;
}


void keyCallback(GLFWwindow* window, int keycode, int scancode, int action, int mods) {
  // It would be more efficient to do the lookup from scancode -> InputKey, but I don't have a hashmap right now and
  // if this ever becomes a performance issue I will eat my hat.
  for(int key = 0; key < INPUT_KEY_COUNT; key++) {
    // TODO: deal with GLFW_UNKNOWN and use scancode
    if (keycode != key_mappings[key]) {
      continue;
    }
    if (action == GLFW_PRESS) {
      keys_pressed[key] = true;
      keys_down[key] = true;
    } else if (action == GLFW_RELEASE) {
      keys_released[key] = true;
      keys_down[key] = false;
    }
    return;
  }
}

void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos) {
  V2 old_pos = mouse_pos;
  mouse_pos = v2(xpos, ypos);
  mouse_d = mouse_pos - old_pos;
}
