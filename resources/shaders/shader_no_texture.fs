#version 330 core
out vec4 FragColor;

in vec3 pos;
in vec3 col;
in vec3 normal;

void main()
{
    FragColor = vec4(col, 1.0) * (0.75 + dot(normal, vec3(0.577, -0.577, 0.577) * 0.25));
} 
