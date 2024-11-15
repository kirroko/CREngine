#ifndef UI_BUTTON_CLASS_H
#define UI_BUTTON_CLASS_H
#include "../vendor/glm/glm/glm.hpp"
#include "glad/glad.h"
#include <string>
#include <iostream>

class VAO;
class VBO;
class EBO;
class Shader;
class TextRenderer;

struct ButtonVertex {
    glm::vec3 position;
    glm::vec3 color;
    glm::vec2 texCoord;
    GLint textureID;
};

struct UIButton {
    glm::vec2 position;
    glm::vec2 size;
    glm::vec3 color;             // Background color of the button
    GLint textureID = -1;        // ID of the texture, if available
    std::string text = "";       // Button text
    std::string fontName = "";   // Font name to use for the text
    glm::vec3 textColor = { 1.0f, 1.0f, 1.0f };  // Color of the text

    UIButton(const glm::vec2& pos, const glm::vec2& sz, const glm::vec3& col,
        const std::string& txt = "", const std::string& font = "", GLint texID = -1)
        : position(pos), size(sz), color(col), text(txt), fontName(font), textureID(texID) {}
};

class UIButtonRenderer {
public:
    UIButtonRenderer();
    ~UIButtonRenderer();
    void init(std::shared_ptr<Shader> sharedShader, GLuint screenWidth, GLuint screenHeight);

    void beginBatch();
    void endBatch();
    void flush();

    // Draw button with optional text and texture
    void drawButton(const UIButton& button);

private:
    void createVertexArray();
    void createVertexBuffer();

    std::unique_ptr<VAO> vao;
    std::unique_ptr<VBO> vbo;
    std::unique_ptr<EBO> ebo;
    std::shared_ptr<Shader> shader;
    std::vector<ButtonVertex> vertices;
    std::vector<GLuint> indices;
    TextRenderer* textRenderer; // For rendering text in buttons

    int maxButtons = 100;  // Adjust as needed
    GLuint screenWidth, screenHeight;
};

#endif // !
