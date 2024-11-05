#version 450 core

// Positions/Coordinates
layout (location = 0) in vec3 aPos;
// Colors
layout (location = 1) in vec3 aColor;
// Texture Coordinates
layout (location = 2) in vec2 aTex;
// Texture ID
layout (location = 3) in int aTexID;  // Changed from vec2 to int

// Outputs the color for the Fragment Shader
out vec3 color;
// Outputs the texture coordinates to the fragment shader
out vec2 texCoord;
// Outputs the texture ID to the fragment shader
flat out int texID;  // Use 'flat' to prevent interpolation


uniform mat4 projection;
uniform mat4 view;

void main()
{
    // Outputs the positions/coordinates of all vertices
    gl_Position = projection * view *  vec4(aPos, 1.0);
    // Assigns the colors from the Vertex Data to "color"
    color = aColor;
    // Assigns the texture coordinates from the Vertex Data to "texCoord"
    texCoord = aTex;
    // Pass the texture ID to the fragment shader
    texID = aTexID;  // Passes integer texture ID without interpolation
}
