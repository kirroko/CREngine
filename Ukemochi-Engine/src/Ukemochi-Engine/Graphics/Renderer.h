#ifndef RENDERER_CLASS_H
#define RENDERER_CLASS_H
#include<iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb/stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cmath> // Might need to remove later on
#include <vector> // Might need to remove later on

#include "shaderClass.h"
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"
#include "Texture.h"

enum class ObjectType { Box, Circle };

class GameObject {

public:
    ObjectType type;
    GLfloat x, y;
    GLfloat width, height;
    GLfloat radius;
    GLboolean enable_texture;

    GameObject(GLfloat x, GLfloat y, GLfloat width, GLfloat height, GLboolean enable_texture)
        : type(ObjectType::Box), x(x), y(y), width(width), height(height), enable_texture(enable_texture) {};

    GameObject(GLfloat x, GLfloat y, GLfloat radius, GLboolean enable_texture)
        : type(ObjectType::Circle), x(x), y(y), radius(radius), enable_texture(enable_texture) {};

};


class Renderer
{
public:
    Renderer();
    ~Renderer();

    void init();
    void render();
    void cleanUp();

    std::vector<GameObject> testObjects;

    void addObjects(const GameObject& object);
    void renderObjects();

    static int const screen_width = 1600;
    static int const screen_height = 900;

    void drawBox(GLfloat x, GLfloat y, GLfloat width, GLfloat height, GLboolean enable_texture);
    void drawCircle(GLfloat x, GLfloat y, GLfloat radius, GLboolean useTexture, GLint segments = 1000);

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
