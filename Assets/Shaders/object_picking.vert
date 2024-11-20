#version 450 core
layout (location = 0) in vec3 aPos; // Vertex position

uniform mat4 model; // Object transformation
uniform mat4 view;  // Camera transformation
uniform mat4 projection; // Projection transformation

void main() 
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
