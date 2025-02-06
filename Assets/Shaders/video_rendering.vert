#version 450 core

layout (location = 0) in vec3 aPos;        // Vertex position
layout (location = 1) in vec3 aColor;      // Color
layout (location = 2) in vec2 aTexCoord;   // Texture coordinates
layout (location = 3) in int aFrameIndex;  // Frame index in texture array

out vec3 fragColor;
out vec2 TexCoord;
flat out int FrameIndex;

uniform mat4 projection;  // Projection matrix

void main()
{
    gl_Position = projection * vec4(aPos, 1.0);  // Transform vertex position
    fragColor = aColor;
    TexCoord = aTexCoord;
    FrameIndex = aFrameIndex;  // Pass frame index to fragment shader
}