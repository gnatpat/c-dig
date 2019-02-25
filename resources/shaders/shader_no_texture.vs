#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aCol;
layout (location = 2) in vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 pos;
out vec3 col;

void main()
{
  gl_Position = projection * view * model * vec4(aPos, 1.0);
  pos = aPos;
  // TODO: sun/moon rising/setting, other lighting bits
  col = aCol * (0.75 + dot(aNormal, normalize(vec3(2, -3, 1)) * 0.25));
}       

