#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 inCol;

out vec3 pos;
out vec3 col;

void main()
{
  gl_Position = projection * view * model * vec4(aPos, 1.0);
  pos = aPos;
  // TODO: sun/moon rising/setting, other lighting bits
  col = inCol;
}       

