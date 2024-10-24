#include "PreCompile.h"
#include "TextRenderer.h"

TextRenderer::TextRenderer(const char* fontPath, GLuint screenWidth, GLuint screenHeight)
	: screenWidth(1600.f), screenHeight(900.f)
{
	// Initialize FreeType library
	if (FT_Init_FreeType(&ft)) {
		std::cerr << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
		return;
	}
	// Setup text buffers
	initBuffers();

	loadTextFont(fontPath);
	// Load and compile text shader
	textShaderProgram = new Shader("../Assets/Shaders/text_rendering.vert", "../Assets/Shaders/text_rendering.frag");
}

TextRenderer::~TextRenderer() {
	// Clean up resources
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


void TextRenderer::loadTextFont(const char* font_path)
{
	// Initialises the FreeType Library
	if (FT_Init_FreeType(&ft))
	{
		// Throws error if FreeType library cant be initialised
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
		return;
	}

	// Loads Font Face from the font_path variable provided
	if (FT_New_Face(ft, font_path, 0, &face))
	{
		std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
		return;
	}

	// Sets the size of the loaded font, in this case the height of characters in the font face is set to 48
	FT_Set_Pixel_Sizes(face, 0, 48);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// Loops through the 128 ASCII characters
	for (unsigned char c = 0; c < 128; c++)
	{
		// Loads the glyph for the current character and renders it as a bitmap
		// FT_LOAD_RENDER tells FreeType to render the loaded glyph into a bitmap that can be accessed via face->glyph->bitmap.
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			std::cout << "ERROR::FREETYPE: Failed to load glyph" << std::endl;
		}

		// Generate a texture
		unsigned int texture;
		glGenTextures(1, &texture);
		// Bind a texture
		glBindTexture(GL_TEXTURE_2D, texture);
		// Creates a 2D texture using glpyh's bitmap information
		glTexImage2D(
			GL_TEXTURE_2D, // target texture
			0, // mipmap level, 0 is the base
			GL_RED, // Specifies that the texture will only have one color
			face->glyph->bitmap.width, // Dimensions of the glyph's bitmap
			face->glyph->bitmap.rows, // Dimensions of the glyph's bitmap
			0, // Border width which must be zero
			GL_RED, // Format of the pixel data being uploaded
			GL_UNSIGNED_BYTE, // Specifies that each pixel in the bitmap is represented by an unsigned byte
			face->glyph->bitmap.buffer // Pointer to the bitmap's pixel data
		);
		// GL_CLAMP_TO_EDGE wrapping mode ensures that sampling beyond the texture edges will use the edge color (useful for text rendering to avoid seams).
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		// GL_LINEAR specifies how the texture should be sampled, smoothing it during scaling.
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		Character character = {
			texture,
			glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			static_cast<GLuint>(face->glyph->advance.x)
		};

		Characters.insert(std::pair<char, Character>(c, character));
	}
	glBindTexture(GL_TEXTURE_2D, 0);

	// Destroy FreeTypes once things are done
	FT_Done_Face(face);
	FT_Done_FreeType(ft);
	
}

void TextRenderer::renderText(std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color)
{
	// Activate text rendering shader
	textShaderProgram->Activate();

	// Get the camera's projection matrix
	auto& camera = Ukemochi::ECS::GetInstance().GetSystem<Camera>();
	glm::mat4 projection = camera->getCameraProjectionMatrix();
	textShaderProgram->setMat4("projection", projection);

	// Set text color
	textShaderProgram->setVec3("textColor", color.x, color.y, color.z);

	// Activate texture unit
	glActiveTexture(GL_TEXTURE0);
	// Bind the VAO to textVAO which stores the vertex attributes configurations for text rendering
	glBindVertexArray(textVAO);

	// Loops through each character in the text string to render them individually
	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++)
	{
		// Retrieves the pre-loaded Character structure for the current character *c from the Characters map
		Character ch = Characters[*c];

		// Computes the position (xpos, ypos) where the current character should be drawn
		// Bearing is an offset that adjusts the position of the character relative to the cursor
		GLfloat xpos = x + ch.Bearing.x * scale;
		GLfloat ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

		// Computes the width (w) and height (h) of the character, scaled by a scale factor to adjust the size of the text
		GLfloat w = ch.Size.x * scale;
		GLfloat h = ch.Size.y * scale;

		// Vertices of the character quad
		GLfloat vertices[6][4] = {
			{ xpos,     ypos + h,   0.0f, 0.0f },
			{ xpos,     ypos,       0.0f, 1.0f },
			{ xpos + w, ypos,       1.0f, 1.0f },

			{ xpos,     ypos + h,   0.0f, 0.0f },
			{ xpos + w, ypos,       1.0f, 1.0f },
			{ xpos + w, ypos + h,   1.0f, 0.0f }
		};

		// Bind character texture
		glBindTexture(GL_TEXTURE_2D, ch.TextureId);
		glBindBuffer(GL_ARRAY_BUFFER, textVBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// Draw quad
		glDrawArrays(GL_TRIANGLES, 0, 6);

		// Advances the cursor position for the next character. The Advance value indicates how far to move the cursor in the x-direction. 
		// It is shifted right by 6 bits (>> 6) because FreeType stores the advance width in 1/64th pixels
		x += (ch.Advance >> 6) * scale;
	}

	// Unbinds the vertex array object and texture
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}
