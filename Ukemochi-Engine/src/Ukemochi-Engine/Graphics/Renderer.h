#ifndef RENDERER_CLASS_H
#define RENDERER_CLASS_H
#include<iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "../vendor/stb/stb_image.h"
#include "../vendor/glm/glm/glm.hpp"
#include <../vendor/glm/glm/gtc/matrix_transform.hpp>
#include <../vendor/glm/glm/gtc/type_ptr.hpp>
#include <cmath> // Might need to remove later on
#include <vector> // Might need to remove later on
#include <algorithm>

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

    void drawBox(GLfloat x, GLfloat y, GLfloat width, GLfloat height, const std::string& texturePath = "");
    void drawCircle(GLfloat x, GLfloat y, GLfloat radius, const std::string& texturePath = "", GLint segments = 1000);

private:
    GLFWwindow* window;
    Shader* shaderProgram;
    std::vector<VAO*> vaos;
    std::vector<VBO*> vbos;
    std::vector<EBO*> ebos;
    std::vector<size_t> indices_count;
    std::vector<GLboolean> textures_enabled;
    std::vector<Texture*> textures;

    void setUpShaders();
    void setUpBuffers(GLfloat* vertices, size_t vertSize, GLuint* indices, size_t indexSize);
    void setUpTextures(const std::string& texturePath);
    void createWindow();


};
#endif
