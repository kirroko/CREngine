#version 450 core 

layout (location = 0) in vec3 aPos;        // Vertex position
layout (location = 1) in vec3 aColor;      // Color
layout (location = 2) in vec2 aTexCoord;   // Texture coordinates

out vec3 fragColor;
out vec2 TexCoord;

uniform mat4 mvp;  // Projection matrix

void main()
{
    gl_Position = mvp * vec4(aPos, 1.0);  // Transform vertex position
    fragColor = aColor;
    TexCoord = aTexCoord;
}
