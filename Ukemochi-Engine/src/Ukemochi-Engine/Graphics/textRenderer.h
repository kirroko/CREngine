#ifndef TEXT_RENDERER_CLASS_H
#define TEXT_RENDERER_CLASS_H
#include "ft2build.h"
#include FT_FREETYPE_H
#include <glad/glad.h>
#include <GLFW/glfw3.h>	
#include <../vendor/glm/glm/glm.hpp>
#include <map>
#include "shaderClass.h"
#include "Camera2D.h"

struct Character {
	GLuint TextureID;
	glm::ivec2 Size;
	glm::ivec2 Bearing;
	GLuint Advance;
};

class TextRenderer {
public:
	TextRenderer(GLuint screenWidth, GLuint screenHeight);
	~TextRenderer();

	void loadTextFont(const std::string& fontName, const char* font_path);
	void setActiveFont(const std::string& font_name);
	void TextRenderer::renderText(const std::string& fontName, const std::string& text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color);


private:
	FT_Library ft;
	FT_Face activeFace;
	std::string activeFontName;
	std::map<std::string, FT_Face> fontFaces;
	std::map<std::string, std::map<GLchar, Character>> fontCharacters;

	GLuint textVAO, textVBO;
	Shader* textShaderProgram;
	GLuint screenWidth, screenHeight;
	void initBuffers();
	void loadCharacters(FT_Face face, std::map<GLchar, Character>& characterMap);
	void releaseFaces();

};
#endif // !TEXTRENDERER_CLASS_H

