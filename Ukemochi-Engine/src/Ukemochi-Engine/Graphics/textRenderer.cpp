/* Start Header ************************************************************************/
/*!
\file       TextRenderer.cpp
\author     TAN Shun Zhi Tomy, t.shunzhitomy, 2301341, t.shunzhitomy@digipen.edu
\date       Nov 6, 2024
\brief      Implementation of the TextRenderer class for rendering text using FreeType and OpenGL.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header **************************************************************************/

#include "PreCompile.h"
#include "TextRenderer.h"
#include "Camera2D.h"

 /*!
  * @brief Constructs a TextRenderer and initializes FreeType and text buffers.
  * @param screenWidth Width of the screen.
  * @param screenHeight Height of the screen.
  */
TextRenderer::TextRenderer(GLuint screenWidth, GLuint screenHeight)
	: screenWidth(screenWidth), screenHeight(screenHeight)
{
	// Initialize FreeType library
	if (FT_Init_FreeType(&ft)) {
		std::cerr << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
		return;
	}
	// Setup text buffers
	initBuffers();

	// Load and compile text shader
	textShaderProgram = new Shader("../Assets/Shaders/text_rendering.vert", "../Assets/Shaders/text_rendering.frag");
}

/*!
 * @brief Destructor to release resources, font faces, and FreeType library.
 */
TextRenderer::~TextRenderer() {
	// Clean up resources
	releaseFaces();
	FT_Done_FreeType(ft);
	delete textShaderProgram;
}

/*!
 * @brief Initializes the VAO and VBO for text rendering.
 */
void TextRenderer::initBuffers()
{
	// Generate and bind the VAO
	glGenVertexArrays(1, &textVAO);
	glGenBuffers(1, &textVBO);
	glBindVertexArray(textVAO);

	// Create the VBO
	glBindBuffer(GL_ARRAY_BUFFER, textVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);

	// Unbind the buffer and VAO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

/*!
 * @brief Loads a font and its characters into memory.
 * @param fontName Name to associate with the loaded font.
 * @param font_path Path to the font file.
 */
void TextRenderer::loadTextFont(const std::string& fontName, const char* font_path)
{
	FT_Face face;

	// Loads Font Face from the font_path variable provided
	if (FT_New_Face(ft, font_path, 0, &face))
	{
		std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
		return;
	}

	// Sets the size of the loaded font, in this case the height of characters in the font face is set to 48
	FT_Set_Pixel_Sizes(face, 0, 48);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	std::map <GLchar, Character> Characters;
	loadCharacters(face, Characters);
	
	fontFaces[fontName] = face;
	fontCharacters[fontName] = Characters;

	if (activeFontName.empty())
		setActiveFont(fontName);
}

/*!
 * @brief Sets the active font for rendering.
 * @param font_name Name of the font to set as active.
 */
void TextRenderer::setActiveFont(const std::string& font_name)
{
	auto it = fontCharacters.find(font_name);
	if (it != fontCharacters.end()) {
		activeFace = fontFaces[font_name];
		activeFontName = font_name;
	}
	else {
		std::cerr << "ERROR::TextRenderer: Font not found: " << font_name << std::endl;
	}
}

/*!
 * @brief Loads character glyphs from a font face into a character map.
 * @param face The font face to load characters from.
 * @param characterMap Map to store the loaded characters.
 */
void TextRenderer::loadCharacters(FT_Face face, std::map<GLchar, Character>& characterMap) 
{
	for (GLubyte c = 0; c < 128; c++) {
		if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
			std::cerr << "ERROR::FREETYPE: Failed to load Glyph for character: " << c << std::endl;
			continue;
		}

		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width, face->glyph->bitmap.rows,
			0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		Character character = {
			texture,
			glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			static_cast<GLuint>(face->glyph->advance.x)
		};

		characterMap.insert(std::pair<GLchar, Character>(c, character));
	}
}

/*!
 * @brief Releases all loaded font faces from memory.
 */
void TextRenderer::releaseFaces()
{
	// Release each font face stored in the map
	for (auto& pair : fontFaces) {
		if (pair.second) {
			FT_Done_Face(pair.second);
		}
	}
	fontFaces.clear();
}

/*!
 * @brief Adds a text object for rendering.
 * @param id Identifier for the text object.
 * @param textObj The TextObject to add.
 */
void TextRenderer::addTextObject(const std::string& id, const TextObject& textObj) 
{
	textObjects[id] = textObj;
}

/*!
 * @brief Updates the text content of an existing text object.
 * @param id Identifier for the text object.
 * @param newText New text content.
 */
void TextRenderer::updateTextObject(const std::string& id, const std::string& newText) 
{
	if (textObjects.find(id) != textObjects.end())
		textObjects[id].text = newText;
}

/*!
 * @brief Updates the position of an existing text object.
 * @param id Identifier for the text object.
 * @param newPosition New position for the text.
 */
void TextRenderer::updateTextPosition(const std::string& id, glm::vec2 newPosition) 
{
	if (textObjects.find(id) != textObjects.end()) {
		textObjects[id].position = newPosition;
	}
}

/*!
 * @brief Renders all added text objects on the screen.
 */
void TextRenderer::renderAllText() 
{
	textShaderProgram->Activate();
	glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(screenWidth), 0.0f, static_cast<GLfloat>(screenHeight));
	textShaderProgram->setMat4("projection", projection);

	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(textVAO);

	for (const auto& textPair : textObjects) 
	{
		const TextObject& textObj = textPair.second;
		setActiveFont(textObj.fontName);

		const auto& Characters = fontCharacters[activeFontName];
		textShaderProgram->setVec3("textColor", textObj.color);

		float x = textObj.position.x;
		float y = textObj.position.y;
		float scale = textObj.scale;

		for (const auto& c : textObj.text) 
		{
			const Character& ch = Characters.at(c);

			GLfloat xpos = x + ch.Bearing.x * scale;
			GLfloat ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

			GLfloat w = ch.Size.x * scale;
			GLfloat h = ch.Size.y * scale;

			GLfloat vertices[6][4] = {
				{ xpos,     ypos + h, 0.0f, 0.0f },
				{ xpos,     ypos,     0.0f, 1.0f },
				{ xpos + w, ypos,     1.0f, 1.0f },
				{ xpos,     ypos + h, 0.0f, 0.0f },
				{ xpos + w, ypos,     1.0f, 1.0f },
				{ xpos + w, ypos + h, 1.0f, 0.0f }
			};

			glBindTexture(GL_TEXTURE_2D, ch.TextureID);
			glBindBuffer(GL_ARRAY_BUFFER, textVBO);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glDrawArrays(GL_TRIANGLES, 0, 6);
			x += (ch.Advance >> 6) * scale;
		}
	}

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}
