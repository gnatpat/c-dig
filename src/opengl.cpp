void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
  SCREEN_WIDTH = width;
  SCREEN_HEIGHT = height;
  printf("%dx%d\n", SCREEN_WIDTH, SCREEN_HEIGHT);
  glViewport(0, 0, width, height);
}

bool initOpenGLAndCreateWindow(GLFWwindow** window) {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  *window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
  if (*window == NULL)
  {
      printf("Failed to create GLFW window");
      glfwTerminate();
      return false;
  }
  glfwMakeContextCurrent(*window);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
  {
      printf("Failed to initialize GLAD");
      return false;
  }    

  glViewport(0, 0, 800, 600);
  glfwSetFramebufferSizeCallback(*window, framebufferSizeCallback);
  glfwSwapInterval(0);
  
  glEnable(GL_DEPTH_TEST);
  //glEnable(GL_CULL_FACE);

  glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

  printf("%s\n", glGetString(GL_VERSION));

  return true;
}
