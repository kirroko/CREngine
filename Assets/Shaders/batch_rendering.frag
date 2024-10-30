#version 330 core

// Inputs from the vertex shader
in vec2 TexCoord;
in vec4 VertexColor;

// Texture sampler
uniform sampler2D uTexture;

// Output color
out vec4 FragColor;

void main() {
    // Sample the texture at the given coordinates
    vec4 texColor = texture(uTexture, TexCoord);

    // Combine the texture color with the vertex color
    FragColor = texColor * VertexColor;
}
