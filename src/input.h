bool isKeyDown(InputKey key);
bool isKeyPressed(InputKey key);
bool isKeyReleased(InputKey key);
V2 mouseDifference();
void initInput();
void updateInput();
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos);
