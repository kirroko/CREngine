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
};

Renderer::~Renderer()
{
	cleanUp();
}

void Renderer::init()
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

	// Sets up scene
	setUpScene();
}

void Renderer::setUpScene()
{
	// Load shaders
	setUpShaders();

	// Set up VAO, VBO, EBO
	setUpBuffers();

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

void Renderer::setUpBuffers()
{
	// Initialize VAO, VBO, and EBO
	vao = new VAO();
	vbo = new VBO(vertices, sizeof(vertices));
	ebo = new EBO(indices, sizeof(indices));

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

		// Assigns a value to the uniform; NOTE: Must always be done after activating the Shader Program
		shaderProgram->setFloat("scale", 0.5);
		// Binds texture so that is appears in rendering
		container->Bind();
		// Bind the VAO so OpenGL knows to use it
		vao->Bind();
		// Draw primitives, number of indices, datatype of indices, index of indices
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
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
	container->Delete();
	shaderProgram->Delete();
}

void Renderer::drawBox(GLfloat x, GLfloat y, GLfloat width, GLfloat height)
{
	// Define the vertices for the box
	GLfloat vertices[] = {
		x, y, 0.0f,                // Bottom-left
		x, y + height, 0.0f,       // Top-left
		x + width, y + height, 0.0f, // Top-right
		x + width, y, 0.0f         // Bottom-right
	};

	// Define indices to form two triangles
	GLuint indices[] = {
		0, 1, 2, // First triangle
		0, 2, 3  // Second triangle
	};

	setUpScene();

	// Draw the box
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	// Unbind everything
	vao->Unbind();
	vbo->Unbind();
	ebo->Unbind();
}
