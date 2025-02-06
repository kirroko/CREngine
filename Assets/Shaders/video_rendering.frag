// pl_mpeg.frag
#ifdef GL_ES
precision mediump float;
#endif

// The texture coordinate passed from the vertex shader.
varying vec2 v_texCoord;

// Sampler uniforms for each plane (Y, Cb, Cr).
uniform sampler2D texY;
uniform sampler2D texCb;
uniform sampler2D texCr;

void main() {
    // Sample the Y, Cb, and Cr components.
    // It is assumed that the textures store the data in the red channel.
    float y  = texture2D(texY,  v_texCoord).r;
    float cb = texture2D(texCb, v_texCoord).r;
    float cr = texture2D(texCr, v_texCoord).r;
    
    // The BT.601 conversion matrix.
    // Note: GLSL matrices are specified in column‐major order.
    // When multiplying a row vector by this matrix (as in the next line),
    // the values combine as intended for BT.601 conversion.
    mat4 bt601 = mat4(
        1.16438,  0.00000,  1.59603, -0.87079,
        1.16438, -0.39176, -0.81297,  0.52959,
        1.16438,  2.01723,  0.00000, -1.08139,
        0.00000,  0.00000,  0.00000,  1.00000
    );
    
    // Construct a vector from the Y, Cb, Cr components (with 1.0 for the offset)
    // and multiply by the conversion matrix to get the final RGB color.
    gl_FragColor = vec4(y, cb, cr, 1.0) * bt601;
}
