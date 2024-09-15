#include "Renderer.h"

GLfloat vertices_test[] = {
//   COORDINATES            / COLORS            / TexCoord  //
    -0.5f, -0.5f, 0.0f,     1.0f, 0.0f, 0.0f,	0.0f, 0.0f, // Lower left corner
    -0.5f,  0.5f, 0.0f,     0.0f, 1.0f, 0.0f,	0.0f, 1.0f, // Upper left corner
     0.5f,  0.5f, 0.0f,     0.0f, 0.0f, 1.0f,	1.0f, 1.0f, // Upper right corner
     0.5f, -0.5f, 0.0f,     1.0f, 1.0f, 1.0f,	1.0f, 0.0f  // Lower right corner
};

GLfloat backgroundVertices[] = {
    // Positions        // Texture Coords
    -1.0f,  1.0f, 0.0f,  0.0f, 1.0f,   // Top-left
    -1.0f, -1.0f, 0.0f,  0.0f, 0.0f,   // Bottom-left
     1.0f, -1.0f, 0.0f,  1.0f, 0.0f,   // Bottom-right
     1.0f,  1.0f, 0.0f,  1.0f, 1.0f    // Top-right
};

GLuint backgroundIndices[] = {
    0, 1, 2,  // First triangle
    0, 2, 3   // Second triangle
};


glm::vec3 cubePositions_test[] = {
    glm::vec3(0.0f,  0.0f,  0.0f)
};

// Indices for vertices order
GLuint indices_test[] = {  // note that we start from 0!
    0, 2, 1, // Upper triangle
    0, 3, 2 // Lower triangle
};

//void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = Renderer::screen_width / 2.0f;
float lastY = Renderer::screen_height / 2.0f;
float firstMouse = true;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

Renderer::Renderer()
{
    // Pointers to OpenGL objects are set to nullptr initially
    shaderProgram = nullptr;
    vao = nullptr;
    vbo = nullptr;
    ebo = nullptr;
    container = nullptr;
    moon_floor = nullptr;
};

Renderer::~Renderer()
{
    cleanUp();
}

void Renderer::init()
{
    glfwInit(); // Initialise glfw

    // Set the version of OpenGL I am using, in this case version 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // Set the number before the decimal (3.), in this case 3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); // Set the number after the decimal (.3), in this case 3
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // Setting profile is like a package of functions, CORE(Modern OpenGL functions), 
    // COMPATIBILITY(both Modern and Outdated OpenGL functions) 

    // Creates Window object of 1600 by 900 pixels, naming it "LearnOpenGl"
    window = glfwCreateWindow(screen_width, screen_height, "LearnOpenGL", NULL, NULL);
    // Error checking for window
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
    }
    glfwMakeContextCurrent(window); // GLFW not the brightest, after creating the window, I need to tell it to use the window

    // Sets a function to handle mouse movement events
    //glfwSetCursorPosCallback(window, mouse_callback);
   
    // Sets a function to handle mouse scroll events
    glfwSetScrollCallback(window, scroll_callback);
    // tell GLFW to capture the mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    gladLoadGL();

    glViewport(0, 0, screen_width, screen_height); // starting x pos, starting y pos, width, height

    glEnable(GL_DEPTH_TEST);

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

void Renderer::setUpShaders() 
{
    shaderProgram = new Shader("../Assets/Shaders/default.vert", "../Assets/Shaders/default.frag");
}

void Renderer::setUpBuffers() 
{
    // Initialize VAO, VBO, and EBO
    vao = new VAO();
    vbo = new VBO(vertices_test, sizeof(vertices_test));
    ebo = new EBO(indices_test, sizeof(indices_test));

    // Bind the VAO to start setting it up
    vao->Bind();
    vbo->Bind();
    ebo->Bind();
    // Links VBO attributes such as coordinates and colors to VAO
    vao->LinkAttrib(*vbo, 0, 3, GL_FLOAT, 8 * sizeof(float), (void*)0);
    vao->LinkAttrib(*vbo, 1, 3, GL_FLOAT, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    vao->LinkAttrib(*vbo, 2, 2, GL_FLOAT, 8 * sizeof(float), (void*)(6 * sizeof(float)));


}

void Renderer::setUpTextures()
{
    moon_floor = new Texture("../Assets/Textures/Moon Floor.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
    moon_floor->texUnit(*shaderProgram, "tex0", 0);

    container = new Texture("../Assets/Textures/container.jpg", GL_TEXTURE_2D, GL_TEXTURE1, GL_RGB, GL_UNSIGNED_BYTE);
    container->texUnit(*shaderProgram, "tex1", 1);

}

void Renderer::render()
{

    // Main Rendering Loop
    while (!glfwWindowShouldClose(window)) // This condition will only happen when I press the close button or if another function tells the window to close
    {

        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        camera.processInput(window, deltaTime);

        // Specify the color of the background
        glClearColor(0.07f, 0.13f, 0.17f, 1.f);
        // Clean the back buffer and assign the new color to it
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Tell OpenGL which Shader Program we want to use
        shaderProgram->Activate();

        // pass projection matrix to shader (note that in this case it could change every frame)
        glm::mat4 projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 100.0f);
        shaderProgram->setMat4("projection", projection);

        // camera/view transformation
        glm::mat4 view = camera.GetViewMatrix();
        shaderProgram->setMat4("view", view);
        
        // Bind the VAO so OpenGL knows to use it
        vao->Bind();
        container->Bind();
        moon_floor->Bind();
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, cubePositions_test[0]);
        if (camera.rotate_state)
            model = glm::rotate(model, (GLfloat)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));
        if (camera.scale_state)
            model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
        shaderProgram->setMat4("model", model);

        //glDrawArrays(GL_TRIANGLES, 0, 6);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);

        glfwPollEvents(); // Handles all pending events, updates the window and input state, and keeps the application responsive to user inputs and window events
        // For example, without glfwPollEvents, the code would not know that i close the application
    }
}

void Renderer::cleanUp() {
    // Clean up resources like VAO, VBO, shader program, etc.
    // Check if each pointer is valid (non-null) before trying to delete them.
    if (vao) 
    {
        vao->Delete();    // Clean up VAO OpenGL resource
        delete vao;       // Free the memory for the VAO object
        vao = nullptr;    // Set to nullptr to avoid dangling pointers
    }
    if (vbo) 
    {
        vbo->Delete();    // Clean up VBO OpenGL resource
        delete vbo;       // Free the memory for the VBO object
        vbo = nullptr;
    }
    if (ebo) 
    {
        ebo->Delete();    // Clean up EBO OpenGL resource
        delete ebo;       // Free the memory for the EBO object
        ebo = nullptr;
    }
    if (shaderProgram) 
    {
        shaderProgram->Delete();  // Clean up shader program resource
        delete shaderProgram;     // Free the memory for the shader object
        shaderProgram = nullptr;
    }
    if (container) 
    {
        container->Delete();  // Clean up texture resource
        delete container;     // Free the memory for the container texture
        container = nullptr;
    }
    if (moon_floor)
    {
        moon_floor->Delete();      // Clean up texture resource
        delete moon_floor;         // Free the memory for the smile texture
        moon_floor = nullptr;
    }


    glfwDestroyWindow(window);
    glfwTerminate();
}

//// glfw: whenever the mouse moves, this callback is called
//// -------------------------------------------------------
//void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
//{
//    float xpos = static_cast<float>(xposIn);
//    float ypos = static_cast<float>(yposIn);
//
//    if (firstMouse)
//    {
//        lastX = xpos;
//        lastY = ypos;
//        firstMouse = false;
//    }
//
//    float xoffset = xpos - lastX;
//    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
//
//    lastX = xpos;
//    lastY = ypos;
//
//    camera.ProcessMouseMovement(xoffset, yoffset);
//}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

void Renderer::drawBox(GLfloat x, GLfloat y, GLfloat width, GLfloat height)
{
    GLfloat vertices[] = {
        //   COORDINATES        / COLORS            / TexCoord  //
        -0.5f,  0.5f, 0.0f,     0.0f, 1.0f, 0.0f,	0.0f, 1.0f, // Upper left corner
        -0.5f, -0.5f, 0.0f,     1.0f, 0.0f, 0.0f,	0.0f, 0.0f, // Lower left corner
         0.5f,  0.5f, 0.0f,     0.0f, 0.0f, 1.0f,	1.0f, 1.0f, // Upper right corner
         0.5f, -0.5f, 0.0f,     1.0f, 1.0f, 1.0f,	1.0f, 0.0f  // Lower right corner
    };
}