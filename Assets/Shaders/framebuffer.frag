#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;

void main()
{
    FragColor = texture(screenTexture, vec2(TexCoords.x, 1.0 - TexCoords.y)); // Flip y
}