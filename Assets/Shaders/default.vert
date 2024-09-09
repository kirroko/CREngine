#version 330 core

// Positions/Coordinates
layout (location = 0) in vec3 aPos;
// Colors
layout (location = 1) in vec3 aCol;
// Texture
layout (location = 2) in vec2 aTex; 

// Outputs the color for the Fragment Shader
out vec4 color;

// Outputs the texture for the Fragment Shader
out vec2 texCoord;

//uniform mat4 transform;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
void main()
{
   // Outputs the positions/coordinates of all vertices
   //gl_Position = transform * vec4(aPos.x, aPos.y, aPos.z, 1.0);

   // note that we read the multiplication from right to left
    gl_Position = projection * view * model * vec4(aPos, 1.0);
   
   color =  vec4(aCol, 1.0);
   texCoord = aTex;
}
