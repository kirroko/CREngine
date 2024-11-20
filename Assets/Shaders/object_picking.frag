#version 450 core
out vec4 FragColor;

uniform vec3 idColor; // Encoded object ID color

void main() 
{
    FragColor = vec4(idColor, 1.0); // Write unique ID color
}
