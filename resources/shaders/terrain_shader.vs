#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aCol;
layout (location = 2) in vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 col;
out float visibility;

const float fog_density = 0.015;
const float fog_gradient = 5.0;

void main()
{
  vec4 cameraRelativePos = view * model * vec4(aPos, 1.0);
  gl_Position = projection * cameraRelativePos;
  // TODO: sun/moon rising/setting, other lighting bits
  col = aCol * (0.75 + dot(aNormal, normalize(vec3(2, -3, 1)) * 0.25));
  float distance_from_camera = length(cameraRelativePos.xyz);
  visibility = exp(-pow((distance_from_camera * fog_density), fog_gradient));
  visibility = clamp(visibility, 0.0, 1.0);
}       

