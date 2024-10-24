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

class TextObject {
public:
	std::string text;
	glm::vec2 position;
	glm::vec3 color;
	float scale;
	std::string fontName;

	TextObject() = default;
	TextObject(const std::string& text, glm::vec2 pos, float scale, glm::vec3 color, const std::string& fontName)
		: text(text), position(pos), color(color), scale(scale), fontName(fontName) {}
};

class TextRenderer {
public:
	TextRenderer(GLuint screenWidth, GLuint screenHeight);
	~TextRenderer();

	void loadTextFont(const std::string& fontName, const char* font_path);
	void setActiveFont(const std::string& font_name);
	void addTextObject(const std::string& id, const TextObject& textObj);
	void updateTextObject(const std::string& id, const std::string& newText);
	void updateTextPosition(const std::string& id, glm::vec2 newPosition);
	void renderAllText();


private:
	FT_Library ft;
	FT_Face activeFace;
	std::string activeFontName;
	std::map<std::string, FT_Face> fontFaces;
	std::map<std::string, std::map<GLchar, Character>> fontCharacters;

	std::map<std::string, TextObject> textObjects;

	GLuint textVAO, textVBO;
	Shader* textShaderProgram;
	GLuint screenWidth, screenHeight;
	void initBuffers();
	void loadCharacters(FT_Face face, std::map<GLchar, Character>& characterMap);
	void releaseFaces();

};
#endif // !TEXTRENDERER_CLASS_H

