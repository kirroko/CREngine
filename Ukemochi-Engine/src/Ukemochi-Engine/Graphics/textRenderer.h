/* Start Header ************************************************************************/
/*!
\file       TextRenderer.h
\author     TAN Shun Zhi Tomy, t.shunzhitomy, 2301341, t.shunzhitomy@digipen.edu
\date       Nov 6, 2024
\brief      Header file for the TextRenderer class, which provides functionality for rendering text using FreeType and OpenGL.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header **************************************************************************/

#ifndef TEXT_RENDERER_CLASS_H
#define TEXT_RENDERER_CLASS_H
#include "ft2build.h"
#include FT_FREETYPE_H
#include <glad/glad.h>
#include <GLFW/glfw3.h>	
#include <../vendor/glm/glm/glm.hpp>
#include <map>
#include "shaderClass.h"

 /*!
  * @struct Character
  * @brief Represents a single character glyph with texture information for text rendering.
  */
struct Character {
	GLuint TextureID; // ID handle of the glyph texture
	glm::ivec2 Size; // Size of the glyph
	glm::ivec2 Bearing; // Offset from baseline to left/top of glyph
	GLuint Advance; // Offset to advance to the next glyph
};

/*!
 * @class TextObject
 * @brief Represents a text object with content, position, color, scale, and font information.
 */
class TextObject {
public:
	std::string text; // Text content
	glm::vec2 position; // Position of the text on screen
	glm::vec3 color; // Color of the text
	float scale; // Scale of the text
	std::string fontName; // Font used for the text

	/*!
	 * @brief Default constructor.
	 */
	TextObject() = default;
	/*!
	 * @brief Constructs a TextObject with specified properties.
	 * @param text The text content.
	 * @param pos Position on the screen.
	 * @param scale Scale of the text.
	 * @param color Color of the text.
	 * @param fontName Name of the font to use.
	 */
	TextObject(const std::string& text, glm::vec2 pos, float scale, glm::vec3 color, const std::string& fontName)
		: text(text), position(pos), color(color), scale(scale), fontName(fontName) {}
};

/*!
 * @class TextRenderer
 * @brief A class responsible for rendering text on the screen using FreeType and OpenGL.
 */
class TextRenderer {
public:
	/*!
	 * @brief Constructs a TextRenderer with specified screen dimensions.
	 * @param screenWidth Width of the screen.
	 * @param screenHeight Height of the screen.
	 */
	TextRenderer(GLuint screenWidth, GLuint screenHeight);
	/*!
	 * @brief Destructor for cleaning up resources used by the TextRenderer.
	 */
	~TextRenderer();

	/*!
	 * @brief Loads a font from a specified file path.
	 * @param fontName Name to associate with the font.
	 * @param font_path Path to the font file.
	 */
	void loadTextFont(const std::string& fontName, const char* font_path);
	/*!
	 * @brief Sets the active font for rendering text.
	 * @param font_name Name of the font to set as active.
	 */
	void setActiveFont(const std::string& font_name);
	/*!
	* @brief Adds a text object for rendering.
	* @param id Identifier for the text object.
	* @param textObj The TextObject to add.
	*/
	void addTextObject(const std::string& id, const TextObject& textObj);
	/*!
	 * @brief Updates the content of an existing text object.
	 * @param id Identifier for the text object.
	 * @param newText New text content.
	 */
	void updateTextObject(const std::string& id, const std::string& newText);
	void updateTextColor(const std::string& id, const glm::vec3& color);
	/*!
	 * @brief Updates the position of an existing text object.
	 * @param id Identifier for the text object.
	 * @param newPosition New position for the text.
	 */
	void updateTextPosition(const std::string& id, glm::vec2 newPosition);
	/*!
	 * @brief Renders all added text objects on the screen.
	 */
	void renderAllText();

	/*!
	 * @brief Calculates the pixel width of a string of text.
	 * @param text The text to measure.
	 * @param scale The scale of the text.
	 * @param fontName The font to use for measuring.
	 * @return The width of the text in pixels.
	 */
	float getTextWidth(const std::string& text, float scale, const std::string& fontName);

	float getTextHeight(const std::string& text, float scale, const std::string& fontName);

private:
	FT_Library ft; // FreeType library instance
	FT_Face activeFace = nullptr; // Current active font face
	std::string activeFontName; // Name of the current active font
	std::map<std::string, FT_Face> fontFaces; // Map of loaded font faces
	std::map<std::string, std::map<GLchar, Character>> fontCharacters; // Map of characters for each font

	std::map<std::string, TextObject> textObjects; // Map of text objects to render

	GLuint textVAO{};
	GLuint textVBO{}; // VAO and VBO for text rendering
	Shader* textShaderProgram = nullptr; // Shader Program for text rendering
	GLuint screenWidth, screenHeight; // Screen dimensions

	/*!
	 * @brief Initializes VAO and VBO for text rendering.
	 */
	void initBuffers();
	/*!
	 * @brief Loads character glyphs for a given font face into a character map.
	 * @param face The font face to load characters from.
	 * @param characterMap Map to store the character glyphs.
	 */
	void loadCharacters(FT_Face face, std::map<GLchar, Character>& characterMap);
	/*!
	 * @brief Releases all loaded font faces.
	 */
	void releaseFaces();

};
#endif // !TEXTRENDERER_CLASS_H

