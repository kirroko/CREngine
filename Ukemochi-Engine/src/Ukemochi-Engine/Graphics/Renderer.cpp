#include "PreCompile.h"
#include "Renderer.h"

Renderer::Renderer()
{
	// Pointers to OpenGL objects are set to nullptr initially
	shaderProgram = nullptr;
	VAO* vaos = nullptr;
	VBO* vbos = nullptr;
	EBO* ebos = nullptr;
};

Renderer::~Renderer()
{
	cleanUp();
}

void Renderer::createWindow()
{
	// Initialize GLFW
	glfwInit();

	// Tell GLFW what version of OpenGL we are using
	// In this case we are using OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Tell GLFW we are using the CORE profile
	// So that means we only have the modern functions
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create a GLFWwindow object of 1600 by 900 pixels
	window = glfwCreateWindow(1600, 900, "Cozy Racoons", NULL, NULL);
	// Error check if the window fails to create
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
	}
	// Introduce the window into the current context
	glfwMakeContextCurrent(window);

	//Load GLAD so it configures OpenGL
	gladLoadGL();
	// Specify the viewport of OpenGL in the Window
	// In this case the viewport goes from x = 0, y = 0, to x = 800, y = 800
	glViewport(0, 0, 1600, 900);
}

void Renderer::init()
{
	// Create window
	//createWindow();

	// Load shaders
	setUpShaders();

}

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

void Renderer::setUpShaders()
{
	shaderProgram = new Shader("../Assets/Shaders/default.vert", "../Assets/Shaders/default.frag");
	//shaderProgram = new Shader("default.vert", "default.frag");
}

void Renderer::setUpBuffers(GLfloat* vertices, size_t vertSize, GLuint* indices, size_t indexSize)
{
	// Create new VAO, VBO, and EBO for each object
	VAO* vao = new VAO();
	VBO* vbo = new VBO(vertices, vertSize);
	EBO* ebo = new EBO(indices, indexSize);

	// Bind the VAO and set up the attributes
	vao->Bind();
	vbo->Bind();
	ebo->Bind();
	vao->LinkAttrib(*vbo, 0, 3, GL_FLOAT, 8 * sizeof(float), (void*)0);
	vao->LinkAttrib(*vbo, 1, 3, GL_FLOAT, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	vao->LinkAttrib(*vbo, 2, 2, GL_FLOAT, 8 * sizeof(float), (void*)(6 * sizeof(float)));

	// Unbind after setting
	vao->Unbind();
	vbo->Unbind();
	ebo->Unbind();

	// Store the buffers in the list
	vaos.push_back(vao);
	vbos.push_back(vbo);
	ebos.push_back(ebo);
}

void Renderer::render()
{

		// Specify the color of the background
		glClearColor(0.07f, 0.13f, 0.17f, 1.f);
		// Clean the back buffer and assign the new color to it
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Tell OpenGL which Shader Program we want to use
		shaderProgram->Activate();
		//shaderProgram->setBool("useTexture", use_texture ? 1 : 0);

		// Loop over each VAO and draw objects
		for (size_t i = 0; i < vaos.size(); ++i)
		{
			vaos[i]->Bind();

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
		}
		//// Swap the back buffer with the front buffer
		//glfwSwapBuffers(window);
		//// Take care of all GLFW events
		//glfwPollEvents();
	
}

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
		if (textures[i]) {
			textures[i]->Delete();  // Delete the OpenGL texture
			delete textures[i];     // Deallocate memory for the texture object
		}
	}

	// Clear the textures vector
	textures.clear();

	// Clear the textures_enabled vector as well
	textures_enabled.clear();

	// Delete the shader program
	if (shaderProgram) {
		shaderProgram->Delete();
		delete shaderProgram;  // Deallocate memory
		shaderProgram = nullptr;
	}
}


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
		new_x - half_width, new_y + half_height, 0.0f,  0.0f, 1.0f, 0.0f,   0.0f, 1.0f,   // Top-left
		new_x - half_width, new_y - half_height, 0.0f,  1.0f, 0.0f, 0.0f,   0.0f, 0.0f,   // Bottom-left
		new_x + half_width, new_y - half_height, 0.0f,  1.0f, 1.0f, 1.0f,   1.0f, 0.0f,   // Bottom-right
		new_x + half_width, new_y + half_height, 0.0f,  0.0f, 0.0f, 1.0f,   1.0f, 1.0f    // Top-right
	};
	// Define indices to form two triangles 
	GLuint indices_box[] = {
	0, 1, 2, // First triangle  
	0, 2, 3  // Second triangle
	};

	// Set up buffers
	setUpBuffers(vertices_box, sizeof(vertices_box), indices_box, sizeof(indices_box));

	// Store the number of indices for this box (6 indices: two triangles)
	indices_count.push_back(6);  // We have 6 indices for a box (two triangles)

	setUpTextures(texturePath);
}


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


	setUpTextures(texturePath);
}