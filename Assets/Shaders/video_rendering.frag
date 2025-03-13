#version 450 core 

in vec3 fragColor;
in vec2 TexCoord;

layout (location = 0) out vec4 FragColor;

uniform sampler2DArray videoTextures;  // Texture array containing video frames
uniform int frameIndex;  // Selected frame from CPU side

void main()
{
    // Sample the correct frame from the texture array
    vec4 texColor = texture(videoTextures, vec3(TexCoord, float(frameIndex)));

    // Output final color
    FragColor = texColor * vec4(fragColor, 1.0);
}
