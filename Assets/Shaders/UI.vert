#version 450 core

layout (location = 0) in vec3 aPos;        // Position in screen space (e.g., in pixels)
layout (location = 1) in vec3 aColor;      // Button color
layout (location = 2) in vec2 aTexCoord;   // Texture coordinates
layout (location = 3) in int aTextureID;   // Texture ID to use (if any)

out vec3 fragColor;
out vec2 TexCoord;
flat out int TextureID;

uniform mat4 projection;  // For converting screen coordinates if necessary

void main()
{
    // Position directly in screen space
    gl_Position = projection * vec4(aPos, 0,0, 1.0);
    
    // Pass color and texture coordinates to fragment shader
    fragColor = aColor;
    TexCoord = aTexCoord;
    TextureID = aTextureID;
}
