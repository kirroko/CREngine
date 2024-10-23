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
	~TextRenderer();

	void loadTextFont(const char* font_path);
	void renderText(std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color);

private:
	FT_Library ft;
	FT_Face face;
	std::unordered_map<char, Character> Characters;
	GLuint textVAO, textVBO;
	Shader* textShaderProgram;
	void initBuffers();

	

};
#endif // !TEXTRENDERER_CLASS_H

