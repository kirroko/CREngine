#version 330 core

// Outputs colors in RGBA
out vec4 FragColor;


// Inputs the color from the Vertex Shader
in vec3 color;
// Inputs the texture coordinates from the Vertex Shader
in vec2 texCoord;

// Gets the Texture Unit from the main function
uniform sampler2D tex0;

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