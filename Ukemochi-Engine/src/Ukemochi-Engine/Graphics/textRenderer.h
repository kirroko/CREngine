#ifndef TEXTRENDERER_CLASS_H
#define TEXTRENDERER_CLASS_H
#include "ft2build.h"
#include FT_FREETYPE_H
#include <glad/glad.h>
#include <GLFW/glfw3.h>	
#include <../vendor/glm/glm/glm.hpp>

#include <map>
#include "shaderClass.h"

struct Character {
	GLuint TextureId;
	glm::ivec2 Size;
	glm::ivec2 Bearing;
	GLuint Advance;
};

class TextRenderer {
public:
	TextRenderer();
	TextRenderer(GLint height);
	
	bool 

};
#endif // !TEXTRENDERER_CLASS_H

