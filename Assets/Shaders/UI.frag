#version 450 core

in vec3 fragColor;
in vec2 TexCoord;
flat in int TextureID;

out vec4 FragColor;

uniform sampler2D textures[32];  // Array of textures, max 32

void main()
{
    vec4 color = vec4(fragColor, 1.0);  // Default to solid color

    // If TextureID is valid, sample the texture
    if (TextureID >= 0)
    {
        vec4 texColor = texture(textures[TextureID], TexCoord);
        color = mix(color, texColor, texColor.a);  // Mix based on texture alpha
    }

    FragColor = color;  // Output final color
}
