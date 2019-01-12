#version 330 core
out vec4 FragColor;
in vec3 pos;

void main()
{
    float red = pos.x >= 0.0 ? 1.0 : 0.0;
    float blue = pos.z >= 0.0 ? 1.0 : 0.0;
    FragColor = vec4(red, 0.0, blue, 1.0);
} 
