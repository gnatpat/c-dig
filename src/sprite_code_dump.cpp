// THe sprite code needs clear up and I'm not using it right now. Here it will live until i come back to it.

void setup() {
  GLuint shader_program = compileShaderOrDie("resources/shaders/shader.vs", "resources/shaders/shader.fs");
  GLuint sprite_textures = loadTexture("resources/sprites.png");
  GLuint sprite_vaos[] = { genTexturedGroundedSprite(0) };

  Sprite sprites[] = { { { 1.0, 3.0, 1.0 }, 0 } };
}

void render() {
  glUseProgram(shader_program);
  transformLocation = glGetUniformLocation(shader_program, "view");
  glUniformMatrix4fv(transformLocation, 1, GL_TRUE, (float*)&view);
  transformLocation = glGetUniformLocation(shader_program, "projection");
  glUniformMatrix4fv(transformLocation, 1, GL_TRUE, (float*)&projection);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, sprite_textures);
  for (unsigned int i = 0; i < (sizeof(sprites) / sizeof(Sprite)); i++) {
    Sprite s = sprites[i];
    Matrix4x4 model = identity();
    model *= translate(s.position);
    model *= rotate(v3(0, 1, 0) * -y_rot);
    transformLocation = glGetUniformLocation(shader_program, "model");
    glUniformMatrix4fv(transformLocation, 1, GL_TRUE, (float*)&model);
    glBindVertexArray(sprite_vaos[s.index]);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
  }
}
