#include "PreCompile.h"
#include "Renderer.h"

// Vertices coordinates
GLfloat vertices[] =
{	//     COORDINATES   /        COLORS      /   TexCoord  //
	-0.5f, -0.5f, 0.0f,    1.0f, 0.0f, 0.0f,	0.0f, 0.0f, // Lower left corner
	-0.5f,  0.5f, 0.0f,    0.0f, 1.0f, 0.0f,	0.0f, 1.0f, // Upper left corner
	 0.5f,  0.5f, 0.0f,    0.0f, 0.0f, 1.0f,	1.0f, 1.0f, // Upper right corner
	 0.5f, -0.5f, 0.0f,    1.0f, 1.0f, 1.0f,	1.0f, 0.0f  // Lower right corner
};

// Indices for vertices order
GLuint indices[] =
{
	0, 2, 1, // Upper triangle
	0, 3, 2 // Lower triangle
};

Renderer::Renderer()
{
	// Pointers to OpenGL objects are set to nullptr initially
	shaderProgram = nullptr;
	vao = nullptr;
	vbo = nullptr;
	ebo = nullptr;
	container = nullptr;
	use_texture = false;
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
	createWindow();

	// Sets up scene
	//setUpScene();

	// Load shaders
	setUpShaders();

	// Set up VAO, VBO, EBO
	setUpBuffers(vertices, sizeof(vertices), indices, sizeof(indices));

	// Load textures
	setUpTextures();
}

void Renderer::setUpScene()
{
	// Load shaders
	setUpShaders();

	// Set up VAO, VBO, EBO
	setUpBuffers(vertices, sizeof(vertices), indices, sizeof(indices));

	// Load textures
	setUpTextures();
}

void Renderer::setUpTextures()
{
	container = new Texture("../Assets/Textures/container.jpg", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGB, GL_UNSIGNED_BYTE);
	container->texUnit(*shaderProgram, "tex0", 0);
}

void Renderer::setUpShaders()
{
	//shaderProgram = new Shader("../Assets/Shaders/default.vert", "../Assets/Shaders/default.frag");
	shaderProgram = new Shader("../Assets/Shaders/default.vert", "../Assets/Shaders/default.frag");
}

void Renderer::setUpBuffers(GLfloat* vertices, size_t vertSize, GLuint* indices, size_t indexSize)
{
	// Initialize VAO, VBO, and EBO
	vao = new VAO();
	vbo = new VBO(vertices, vertSize);
	ebo = new EBO(indices, indexSize);

	// Bind the VAO to start setting it up
	vao->Bind();
	vbo->Bind();
	ebo->Bind();
	// Links VBO attributes such as coordinates and colors to VAO
	vao->LinkAttrib(*vbo, 0, 3, GL_FLOAT, 8 * sizeof(float), (void*)0);
	vao->LinkAttrib(*vbo, 1, 3, GL_FLOAT, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	vao->LinkAttrib(*vbo, 2, 2, GL_FLOAT, 8 * sizeof(float), (void*)(6 * sizeof(float)));

	// Unbind all to prevent accidentally modifying them
	vao->Unbind();
	vbo->Unbind();
	ebo->Unbind();
}

void Renderer::render()
{
	while (!glfwWindowShouldClose(window))
	{
		// Specify the color of the background
		glClearColor(0.07f, 0.13f, 0.17f, 1.f);
		// Clean the back buffer and assign the new color to it
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Tell OpenGL which Shader Program we want to use
		shaderProgram->Activate();
		shaderProgram->setBool("useTexture", use_texture ? 1 : 0);

		// Binds texture so that is appears in rendering
		if (use_texture)
			container->Bind();
		// Bind the VAO so OpenGL knows to use it
		vao->Bind();
		// Draw primitives, number of indices, datatype of indices, index of indices
		glDrawElements(GL_TRIANGLE_FAN, 102, GL_UNSIGNED_INT, 0);
		// Swap the back buffer with the front buffer
		glfwSwapBuffers(window);
		// Take care of all GLFW events
		glfwPollEvents();
	}
}

void Renderer::cleanUp()
{
	// Delete all the objects we've created
	vao->Delete();
	vbo->Delete();
	ebo->Delete();
	if (use_texture)
		container->Delete();
	shaderProgram->Delete();
}

void Renderer::addObjects(const GameObject& object)
{
	testObjects.push_back(object);
}

void Renderer::renderObjects()
{
	for (auto& obj : testObjects)
	{
		if (obj.type == ObjectType::Box)
		{
			//drawBox();
		}
	}
}

void Renderer::drawBox(GLfloat x, GLfloat y, GLfloat width, GLfloat height, GLboolean enable_texture)
{
	// Convert screen coordinates to normalized device coordinates (NDC)
	GLfloat new_x = (2.0f * x) / screen_width - 1.0f;
	GLfloat new_y = 1.0f - (2.0f * y) / screen_height;

	// Convert width and height from screen space to NDC scaling
	GLfloat new_width = (2.0f * width) / screen_width;
	GLfloat new_height = (2.0f * height) / screen_height;

	// Define the vertices for the box
	GLfloat vertices_box[] = {
		new_x, new_y, 0.0f,	0.0f, 1.0f, 0.0f,	0.0f, 1.0f,						// Top-left
		new_x, new_y - new_height, 0.0f,  1.0f, 0.0f, 0.0f,	0.0f, 0.0f,				// Bottom-left
		new_x + new_width, new_y - new_height, 0.0f, 1.0f, 1.0f, 1.0f,	1.0f, 0.0f,		// Bottom-right
		new_x + new_width, new_y, 0.0f, 0.0f, 0.0f, 1.0f,	1.0f, 1.0f				// Top-right
	};

	// Define indices to form two triangles
	GLuint indices_box[] = {
		0, 1, 2, // First triangle
		0, 2, 3  // Second triangle
	};

	// Set up shaders, buffers, and textures
	createWindow();
	setUpShaders();
	setUpBuffers(vertices_box, sizeof(vertices_box), indices_box, sizeof(indices_box));
	if (enable_texture)
	{
		setUpTextures();
		use_texture = true;
	}
	render();
	cleanUp();
}

void Renderer::drawCircle(GLfloat x, GLfloat y, GLfloat radius, GLboolean useTexture, GLint segments)
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
	for (int i = 0; i <= segments; ++i) {
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

	indices.push_back(1);

	// Set up shaders, buffers, and textures as usual
	createWindow();
	setUpShaders();
	setUpBuffers(vertices.data(), vertices.size() * sizeof(GLfloat), indices.data(), indices.size() * sizeof(GLuint));

	// Conditionally set up and bind the texture if the flag is true
	if (useTexture) {
		setUpTextures();  // Load and bind the texture if needed
		container->Bind(); // Bind the texture to be drawn
	}

	render();
	cleanUp();
}