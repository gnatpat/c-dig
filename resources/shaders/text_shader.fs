#version 330 core
out vec4 FragColor;

in vec2 uv;

uniform sampler2D fontTexture;

const float width = 0.5;
const float edge = 0.1;
void main()
{
    float distance = 1-texture(fontTexture, uv).a;
    float alpha = 1-smoothstep(width, width+edge, distance);
      
    FragColor = vec4(0.0, 0.0, 0.0, alpha);
} 
