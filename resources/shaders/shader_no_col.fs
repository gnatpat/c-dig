#version 330 core
out vec4 FragColor;

in vec3 pos;
in vec3 col;

void main()
{
    FragColor = vec4(col, 0.2);
} 
