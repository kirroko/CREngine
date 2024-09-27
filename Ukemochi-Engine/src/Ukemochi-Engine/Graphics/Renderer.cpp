/*!
 * @file    Renderer.cpp
 * @brief   This file contains the implementation of the Renderer class responsible for 
			handling OpenGL rendering, including setting up shaders, buffers, textures, 
			and rendering 2D objects like boxes and circles.
 * @author  t.shunzhitomy@digipen.edu
 * @date    25/09/2024
 */
#include "PreCompile.h"
#include "Renderer.h"

/*!
 * @brief Constructor for the Renderer class.
 * Initializes pointers to OpenGL objects (e.g., shaderProgram, VAOs, VBOs, EBOs) to nullptr.
 */
Renderer::Renderer()
{
	// Pointers to OpenGL objects are set to nullptr initially
	shaderProgram = nullptr;
	VAO* vaos = nullptr;
	VBO* vbos = nullptr;
	EBO* ebos = nullptr;
};

/*!
 * @brief Destructor for the Renderer class.
 * Calls the cleanUp() method to release all allocated OpenGL resources.
 */
Renderer::~Renderer()
{
	cleanUp();
}

/*!
 * @brief Initializes the renderer, including loading shaders.
 * This function should be called once at the beginning to set up necessary resources.
 */
void Renderer::init()
{
	// Load shaders
	setUpShaders();
}

/*!
 * @brief Loads and sets up a texture based on the given file path.
 * Supports PNG (GL_RGBA) and JPG (GL_RGB) formats.
 * @param texturePath The file path to the texture to be loaded.
 */
void Renderer::setUpTextures(const std::string& texturePath)
{
	// If a texture file path is provided, load and store the texture
	if (!texturePath.empty())
	{
		// Extract the file extension and determine format (GL_RGBA for PNG, GL_RGB for JPG)
		std::string extension = texturePath.substr(texturePath.find_last_of(".") + 1);
		std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

		GLenum format;
		if (extension == "png")
			format = GL_RGBA;  // PNG files have transparency
		else if (extension == "jpg" || extension == "jpeg")
			format = GL_RGB;   // JPG files do not have transparency
		else
		{
			std::cerr << "Warning: Unrecognized image format. Defaulting to GL_RGB." << std::endl;
			format = GL_RGB;
		}

		// Load and store the texture with the determined format
		Texture* texture = new Texture(texturePath.c_str(), GL_TEXTURE_2D, GL_TEXTURE0, format, GL_UNSIGNED_BYTE);
		textures.push_back(texture);
		textures_enabled.push_back(true);
		texture->texUnit(*shaderProgram, "tex0", 0);
	}
	else
	{
		// No texture provided
		textures.push_back(nullptr);  // No texture for this object
		textures_enabled.push_back(false);
	}
}

/*!
 * @brief Loads and sets up the shaders to be used for rendering.
 * Currently loads vertex and fragment shaders from the specified paths.
 */
void Renderer::setUpShaders()
{
	shaderProgram = new Shader("../Assets/Shaders/default.vert", "../Assets/Shaders/default.frag");
}

/*!
 * @brief Sets up the Vertex Array Object (VAO), Vertex Buffer Object (VBO), and Element Buffer Object (EBO) for the given vertices and indices.
 * This function prepares the OpenGL buffers for rendering the object.
 * @param vertices Pointer to the vertex data.
 * @param vertSize The size of the vertex data in bytes.
 * @param indices Pointer to the index data.
 * @param indexSize The size of the index data in bytes.
 */
void Renderer::setUpBuffers(GLfloat* vertices, size_t vertSize, GLuint* indices, size_t indexSize)
{
	// Create new VAO, VBO, and EBO for each object
	VAO* vao = new VAO();
	VBO* vbo = new VBO(vertices, vertSize);
	EBO* ebo = new EBO(indices, indexSize);

	vao->Bind();
	vbo->Bind();
	ebo->Bind();

	vao->LinkAttrib(*vbo, 0, 3, GL_FLOAT, 8 * sizeof(float), (void*)0);
	vao->LinkAttrib(*vbo, 1, 3, GL_FLOAT, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	vao->LinkAttrib(*vbo, 2, 2, GL_FLOAT, 8 * sizeof(float), (void*)(6 * sizeof(float)));

	vao->Unbind();
	vbo->Unbind();
	ebo->Unbind();

	// Store the buffers in the list
	vaos.push_back(vao);
	vbos.push_back(vbo);
	ebos.push_back(ebo);
}

/*!
 * @brief Renders all the objects (boxes and circles) using the set up VAOs, VBOs, EBOs, and textures.
 */
void Renderer::render()
{
	// Specify the color of the background
	glClearColor(0.07f, 0.13f, 0.17f, 1.f);
	// Clean the back buffer and assign the new color to it
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	drawBox(800.f, 450.f, 100.f, 100.f, "");

	// Loop over each VAO and draw objects
	for (size_t i = 0; i < vaos.size(); ++i)
	{
		vaos[i]->Bind();

		// Activate the shader program
		shaderProgram->Activate();

		// Create the model matrix
		glm::mat4 model = glm::mat4(1.0f);  // Identity matrix

		// Apply scaling if enabled
		if (scale_enabled)
		{
			model = glm::scale(model, glm::vec3(scale_factor, scale_factor, 1.0f));  // Apply scaling
		}

		// Apply rotation if enabled
		if (rotation_enabled)
		{
			// Apply the fixed rotation speed
			rotation_angle += rotation_speed;  // Fixed speed of 0.01f radians

			// Cap the rotation angle between 0 and 360 degrees
			if (rotation_angle >= 360.0f)
				rotation_angle -= 360.0f;

			// Apply rotation to the model matrix
			model = glm::rotate(model, glm::radians(rotation_angle), glm::vec3(0.0f, 0.0f, 1.0f));
		}

		// Send the model matrix to the shader
		shaderProgram->setMat4("model", model);

		// Binds texture so that is appears in rendering
		if (textures_enabled[i])
		{
			textures[i]->Bind();
			shaderProgram->setBool("useTexture", true);
		}
		else
		{
			shaderProgram->setBool("useTexture", false);
		}

		glDrawElements(GL_TRIANGLE_FAN, indices_count[i], GL_UNSIGNED_INT, 0);

		// If debug mode is enabled, draw wireframe outlines
		if (debug_mode_enabled) {
			shaderProgram->setBool("debug", true);
			drawBoxOutline(800.f, 450.f, 100.f, 100.f);
		}
		
		vaos[i]->Unbind();

	}

}

/*!
 * @brief Cleans up and releases all OpenGL resources (VAOs, VBOs, EBOs, textures, shaders).
 */
void Renderer::cleanUp()
{
	// Delete all VAOs, VBOs, and EBOs
	for (size_t i = 0; i < vaos.size(); ++i)
	{
		vaos[i]->Delete();
		delete vaos[i]; // Deallocate memory
	}
	for (size_t i = 0; i < vbos.size(); ++i)
	{
		vbos[i]->Delete();
		delete vbos[i]; // Deallocate memory
	}
	for (size_t i = 0; i < ebos.size(); ++i)
	{
		ebos[i]->Delete();
		delete ebos[i]; // Deallocate memory
	}

	// Clear the vectors after deleting the objects
	vaos.clear();
	vbos.clear();
	ebos.clear();

	// Delete all textures
	for (size_t i = 0; i < textures.size(); ++i)
	{
		if (textures[i]) 
		{
			textures[i]->Delete();  // Delete the OpenGL texture
			delete textures[i];     // Deallocate memory for the texture object
		}
	}

	// Clear the textures vector
	textures.clear();

	// Clear the textures_enabled vector as well
	textures_enabled.clear();

	// Delete the shader program
	if (shaderProgram) 
	{
		shaderProgram->Delete();
		delete shaderProgram;  // Deallocate memory
		shaderProgram = nullptr;
	}
}

/*!
 * @brief Draws a 2D box with the given position, dimensions, and texture, 
		  starting position is the top left of screen. It starts from the 
		  center of the box.
 * @param x The x-coordinate of the center of the box (in screen space).
 * @param y The y-coordinate of the center of the box (in screen space).
 * @param width The width of the box (in screen space).
 * @param height The height of the box (in screen space).
 * @param texturePath The file path to the texture for the box.
 */
void Renderer::drawBox(GLfloat x, GLfloat y, GLfloat width, GLfloat height, const std::string& texturePath)
{
	// Convert screen coordinates to normalized device coordinates (NDC) 
	GLfloat new_x = (2.0f * x) / screen_width - 1.0f;
	GLfloat new_y = 1.0f - (2.0f * y) / screen_height;
	// Convert width and height from screen space to NDC scaling 
	GLfloat new_width = (2.0f * width) / screen_width;
	GLfloat new_height = (2.0f * height) / screen_height;

	// Adjust vertices so that the position (x, y) represents the center of the box
	GLfloat half_width = new_width / 2.0f;
	GLfloat half_height = new_height / 2.0f;

	// Define the vertices for the box, centered around (new_x, new_y)
	GLfloat vertices_box[] = {
		new_x - half_width, new_y + half_height, 0.0f,  0.0f, 0.0f, 0.0f,   0.0f, 1.0f,   // Top-left
		new_x - half_width, new_y - half_height, 0.0f,  0.0f, 0.0f, 0.0f,   0.0f, 0.0f,   // Bottom-left
		new_x + half_width, new_y - half_height, 0.0f,  0.0f, 0.0f, 0.0f,   1.0f, 0.0f,   // Bottom-right
		new_x + half_width, new_y + half_height, 0.0f,  0.0f, 0.0f, 0.0f,   1.0f, 1.0f    // Top-right
	};
	// Define indices to form two triangles 
	GLuint indices_box[] = {
	0, 1, 2, // First triangle  
	0, 2, 3  // Second triangle
	};

	// ** Set the object color for the filled box (e.g., white) **
	shaderProgram->setVec3("objectColor", glm::vec3(1.0f, 1.0f, 1.0f));  // Set uniform to white

	// Set up buffers
	setUpBuffers(vertices_box, sizeof(vertices_box), indices_box, sizeof(indices_box));

	// Store the number of indices for this box (6 indices: two triangles)
	indices_count.push_back(6);  // We have 6 indices for a box (two triangles)

	// Set up the texture for the box
	setUpTextures(texturePath);
}

/*!
 * @brief Draws a 2D circle with the given position, radius, and texture, 
		  starting position is the top left of screen. It starts from the 
		  center of the box.
 * @param x The x-coordinate of the center of the circle (in screen space).
 * @param y The y-coordinate of the center of the circle (in screen space).
 * @param radius The radius of the circle (in screen space).
 * @param texturePath The file path to the texture for the circle.
 * @param segments The number of segments to use for rendering the circle (higher numbers create smoother circles), 
		  by default it is set to 1000.
 */
void Renderer::drawCircle(GLfloat x, GLfloat y, GLfloat radius, const std::string& texturePath, GLint segments)
{
	// Convert screen coordinates to normalized device coordinates (NDC)
	GLfloat new_x = (2.0f * x) / screen_width - 1.0f;
	GLfloat new_y = 1.0f - (2.0f * y) / screen_height;
	GLfloat z = 0.0f;
	// Convert radius from screen space to NDC scaling
	GLfloat new_radius_x = (2.0f * radius) / screen_width;
	GLfloat new_radius_y = (2.0f * radius) / screen_height;

	// Arrays for vertices and indices
	std::vector<GLfloat> vertices;
	std::vector<GLuint> indices;

	// Add center vertex
	vertices.push_back(new_x);
	vertices.push_back(new_y);
	vertices.push_back(z);
	vertices.push_back(1.0f);   // Color (r)
	vertices.push_back(1.0f);   // Color (g)
	vertices.push_back(1.0f);   // Color (b)
	vertices.push_back(0.5f);   // Texture coordinate (s) - center of the texture
	vertices.push_back(0.5f);   // Texture coordinate (t)


	// Generate vertices around the circle
	for (int i = 0; i <= segments; ++i)
	{
		GLfloat angle = i * 2.0f * 3.1415926f / segments;
		GLfloat dx = cosf(angle) * new_radius_x;
		GLfloat dy = sinf(angle) * new_radius_y;
		vertices.push_back(new_x + dx);
		vertices.push_back(new_y + dy);
		vertices.push_back(z);

		// Color
		vertices.push_back(1.0f);  // Color r
		vertices.push_back(1.0f);  // Color g
		vertices.push_back(1.0f);  // Color b

		// Texture coordinates
		vertices.push_back(0.5f + cosf(angle) * 0.5f);  // s (normalized)
		vertices.push_back(0.5f + sinf(angle) * 0.5f);  // t (normalized)

		// Index the vertices for triangle fan
		indices.push_back(i + 1);
	}

	// Store the number of indices for this object
	indices_count.push_back(indices.size());

	// Set up buffers
	setUpBuffers(vertices.data(), vertices.size() * sizeof(GLfloat), indices.data(), indices.size() * sizeof(GLuint));

	// Set up the texture for the circle
	setUpTextures(texturePath);
}

void Renderer::ToggleInputsForScale()
{
	scale_enabled = !scale_enabled;
	// Adjust scale factor when toggled
	if (scale_enabled)
	scale_factor = 0.5f;
	else
	scale_factor = 1.0f;
}

void Renderer::ToggleInputsForRotation()
{
	rotation_enabled = !rotation_enabled;
	if (!rotation_enabled)
	{
		rotation_angle = 0.f;
	}
}


void Renderer::drawBoxOutline(GLfloat x, GLfloat y, GLfloat width, GLfloat height) {
	// Convert screen coordinates to normalized device coordinates (NDC)
	GLfloat new_x = (2.0f * x) / screen_width - 1.0f;
	GLfloat new_y = 1.0f - (2.0f * y) / screen_height;
	GLfloat new_width = (2.0f * width) / screen_width;
	GLfloat new_height = (2.0f * height) / screen_height;

	GLfloat half_width = new_width / 2.0f;
	GLfloat half_height = new_height / 2.0f;

	// Define the vertices for the box outline (positions only)
	GLfloat vertices_box[] = {
		new_x - half_width, new_y + half_height, 0.0f,  // Top-left
		new_x - half_width, new_y - half_height, 0.0f,  // Bottom-left
		new_x + half_width, new_y - half_height, 0.0f,  // Bottom-right
		new_x + half_width, new_y + half_height, 0.0f   // Top-right
	};

	// Activate the shader program
	shaderProgram->Activate();

	// ** Set the wireframe color to red **
	shaderProgram->setVec3("objectColor", glm::vec3(1.0f, 0.0f, 0.0f));  // Set uniform to red

	// Set to wireframe mode and increase line width for visibility
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glLineWidth(2.0f);  // Make the wireframe lines thicker

	// Enable the vertex attribute for position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), vertices_box);

	// Draw the outline using GL_LINE_LOOP
	glDrawArrays(GL_LINE_LOOP, 0, 4);

	// Disable the vertex array and reset to fill mode
	glDisableVertexAttribArray(0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// Reset line width back to default
	glLineWidth(1.0f);
}