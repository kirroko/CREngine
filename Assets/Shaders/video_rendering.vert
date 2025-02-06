// pl_mpeg.vert
#ifdef GL_ES
precision mediump float;
#endif

// Attributes: vertex position and texture coordinate.
attribute vec4 a_position;
attribute vec2 a_texCoord;

// Pass the texture coordinate to the fragment shader.
varying vec2 v_texCoord;

void main() {
    gl_Position = a_position;
    v_texCoord = a_texCoord;
}
