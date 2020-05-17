#version 330 core
uniform vec3 skyColour;

out vec4 FragColor;

in vec4 col;
in float visibility;

void main()
{
    FragColor = vec4(mix(skyColour, col.xyz, visibility), col.w);
} 
