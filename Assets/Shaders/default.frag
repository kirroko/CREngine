#version 450 core

out vec4 FragColor;

in vec3 color;
in vec2 texCoord;
flat in int texID;
uniform sampler2D textures[32];

void main()
{

   vec4 textureColor = texture(textures[texID], texCoord);

   // Multiply by color only if you want a tint; otherwise, use just the texture color
   FragColor = textureColor * vec4(color, 1.0);
   
}
