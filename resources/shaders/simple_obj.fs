#version 330 core
uniform vec3 skyColour;

out vec4 FragColor;

in vec3 col;
in float visibility;

void main()
{
    FragColor = vec4(mix(skyColour, col, visibility), 1.0);
} 
