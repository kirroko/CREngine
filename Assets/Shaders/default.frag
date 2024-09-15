#version 330 core
// Outputs colors in RGBA
out vec4 FragColor;

// Inputs the color from the Vertex Shader
in vec4 color;

// Inputs the texture from the Vertex Shader
in vec2 texCoord;

uniform sampler2D tex0;
uniform sampler2D tex1;

void main()
{
   //FragColor =  mix(texture(tex0, texCoord), texture(tex1, texCoord), 0.2);
   FragColor = texture(tex0, texCoord);
}