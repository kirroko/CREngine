#version 450 core
out vec4 FragColor;

in vec2 TexCoords; // Interpolated texture coordinates

uniform sampler2D framebufferTexture; // The picking framebuffer texture

void main()
{
   FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}