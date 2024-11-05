#version 450 core

out vec4 FragColor;

uniform vec3 color;  // Color for debug outlines

void main() 
{
    FragColor = vec4(color, 1.0);
}
