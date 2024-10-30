#version 330 core

// Inputs from the vertex shader
in vec2 texCoord;
in vec3 VertexColor;

// Texture sampler
uniform sampler2D tex0;

// Output color
out vec4 FragColor;
uniform bool useTexture;
uniform vec3 objectColor;
void main() 
{
    if(useTexture)
	{
		vec4 texColor = texture(tex0, texCoord);
    	if (texColor.a < 0.1) discard;  // Optional: Discard pixels with low alpha
    	FragColor = texColor;
	}
	else
	FragColor = vec4(objectColor, 1.0f);
}
