#version 450 core

in vec3 fragColor;
in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D ourTexture;
uniform float alpha;

void main()
{
	FragColor = texture(ourTexture, TexCoord) * vec4(fragColor, alpha);
}
