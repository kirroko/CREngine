#version 450 core 

in vec3 fragColor;
in vec2 TexCoord;

layout (location = 0) out vec4 FragColor;

uniform sampler2DArray videoTextures;  // Texture array containing video frames
uniform int frameIndex;  // Selected frame from CPU side

uniform sampler2D tex_y;
uniform sampler2D tex_cb;
uniform sampler2D tex_cr;

void main()
{
    float y = texture(tex_y, TexCoord).r;
    float cb = texture(tex_cb, TexCoord).r;
    float cr = texture(tex_cr, TexCoord).r;

    // BT.601 Limited-Range Conversion Matrix (column-major order)
    mat4 bt601 = mat4(
	    1.16438,  0.00000,  1.59603, -0.87079,
	    1.16438, -0.39176, -0.81297,  0.52959,
	    1.16438,  2.01723,  0.00000, -1.08139,
	    0, 0, 0, 1
    );

    FragColor = vec4(y, cb, cr, 1.0) * bt601;
}
