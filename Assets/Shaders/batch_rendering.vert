#version 330 core

// Input attributes from the vertex buffer
layout(location = 0) in vec3 aPos;      // Vertex position
layout(location = 1) in vec2 aTexCoord; // Texture coordinate
layout(location = 2) in vec4 aColor;    // Vertex color

// Uniforms for transformations
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

// Outputs to the fragment shader
out vec2 TexCoord;
out vec4 VertexColor;

void main() 
{
    // Calculate the transformed position
    gl_Position = projection * view * model * vec4(aPos, 1.0);

    // Pass texture coordinates and color to the fragment shader
    TexCoord = aTexCoord;
    VertexColor = aColor;
}