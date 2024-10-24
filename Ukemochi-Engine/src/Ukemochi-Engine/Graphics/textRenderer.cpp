#include "PreCompile.h"
#include "TextRenderer.h"

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

TextRenderer::~TextRenderer() {
	// Clean up resources
	releaseFaces();
	FT_Done_FreeType(ft);
	delete textShaderProgram;
}

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

void TextRenderer::renderText(const std::string& fontName, const std::string& text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color) 
{
	// Set the font based on the given font name
	setActiveFont(fontName);

	if (activeFontName.empty() || fontCharacters.find(activeFontName) == fontCharacters.end()) {
		std::cerr << "ERROR::TextRenderer: No active font set or font not loaded." << std::endl;
		return;
	}

	// Get the active font's character map
	const std::map<GLchar, Character>& Characters = fontCharacters[activeFontName];

	textShaderProgram->Activate();
	glUniform3f(glGetUniformLocation(textShaderProgram->ID, "textColor"), color.x, color.y, color.z);
	glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(screenWidth), 0.0f, static_cast<GLfloat>(screenHeight));
	glUniformMatrix4fv(glGetUniformLocation(textShaderProgram->ID, "projection"), 1, GL_FALSE, &projection[0][0]);

	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(textVAO);

	for (const auto& c : text) {
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

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

