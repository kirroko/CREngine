#ifndef RENDERER_CLASS_H
#define RENDERER_CLASS_H
#include<iostream>
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

class Renderer
{
public:
    Renderer();
    ~Renderer();

    void init();
    void render();
    void cleanUp();

    static int const screen_width = 1600;
    static int const screen_height = 900;

    void drawBox(GLfloat x, GLfloat y, GLfloat width, GLfloat height, GLboolean enable_texture);
private:
    GLFWwindow* window;
    Shader* shaderProgram;
    VAO* vao;
    VBO* vbo;
    EBO* ebo;
    Texture* container;
    GLboolean use_texture;

    void setUpScene();
    void setUpShaders();
    void setUpBuffers(GLfloat* vertices, size_t vertSize, GLuint* indices, size_t indexSize);
    void setUpTextures();
    void createWindow();


};
#endif
