GLuint compileShaderFile(const char* path, const GLenum shaderType) {
  FILE *fp = fopen(path, "r");
  if (fp == NULL) {
    printf("Couldn't open %s.\n", path);
  }
  GLuint shader = glCreateShader(shaderType);

  int len = getFileSize(fp);
  char* shader_text = (char*)malloc(len * sizeof(char));
  size_t size_read = fread(shader_text, sizeof(char), len, fp);
  assert(size_read == (size_t)len);

  glShaderSource(shader, 1, &shader_text, &len);
  free(shader_text);

  glCompileShader(shader);

  int success;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (success == GL_FALSE) {
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
    char* info_log = (char*)malloc(len * sizeof(char));
    glGetShaderInfoLog(shader, len, NULL, info_log);
    printf("Shader compilation error. %s.\n%s\n", path, info_log);
    free(info_log);
    return 0;
  }

  return shader;
}

GLuint compileShader(const char* vertex_path, const char* fragment_path) {
  GLuint vertex_shader = compileShaderFile(vertex_path, GL_VERTEX_SHADER);
  if (vertex_shader == 0) {
    return 0;
  }
  GLuint fragment_shader = compileShaderFile(fragment_path, GL_FRAGMENT_SHADER);
  if (fragment_shader == 0) {
    return 0;
  }

  GLuint shader = glCreateProgram();
  glAttachShader(shader, vertex_shader);
  glAttachShader(shader, fragment_shader);
  glLinkProgram(shader);

  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);

  int success;
  glGetProgramiv(shader, GL_LINK_STATUS, &success);
  if (success == GL_FALSE) {
    int len;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
    char* info_log = (char*)malloc(len * sizeof(char));
    glGetProgramInfoLog(shader, len, NULL, info_log);
    printf("Shader linker error.\n%s\n", info_log);
    free(info_log);
    return 0;
  }

  return shader;
}

GLuint compileShaderOrDie(const char* vertex_path, const char* fragment_path) {
  GLuint shader = compileShader(vertex_path, fragment_path);
  if (shader == 0) {
    exit(1);
  }
  return shader;
}
