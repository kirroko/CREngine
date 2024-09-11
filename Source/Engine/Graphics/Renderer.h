#ifndef RENDERER_CLASS_H
#define RENDERER_CLASS_H
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb/stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shaderClass.h"
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"
#include "Texture.h"
#include "Camera.h"

class Renderer
{
public:
    Renderer();
    ~Renderer();

    void init();
    void render();
    void cleanUp();

    static int const screen_height = 800;
    static int const screen_width = 600;

private:
    GLFWwindow* window;
    Shader* shaderProgram;
    VAO* vao;
    VBO* vbo;
    EBO* ebo;
    Texture* container;
    Texture* smile;
    

    void setUpScene();
    void setUpShaders();
    void setUpBuffers(); 
};
#endif
