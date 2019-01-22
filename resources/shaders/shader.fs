#version 330 core
out vec4 FragColor;
in vec2 uv;

uniform sampler2D ourTexture;

void main()
{
    vec4 texColor = texture(ourTexture, uv);
    if (texColor.a == 0.0) {
      discard;
    }
    FragColor = texColor;
} 
