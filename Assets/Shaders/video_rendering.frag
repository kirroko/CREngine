#version 450 core

in vec3 fragColor;
in vec2 TexCoord;
flat in int FrameIndex;   // The selected frame from the texture array

out vec4 FragColor;

uniform sampler2DArray videoTextures;  // Texture array containing video frames

void main()
{
    // Sample the correct frame from the texture array
    vec4 texColor = texture(videoTextures, vec3(TexCoord, FrameIndex));

    // Final output color
    FragColor = texColor * vec4(fragColor, 1.0);
}