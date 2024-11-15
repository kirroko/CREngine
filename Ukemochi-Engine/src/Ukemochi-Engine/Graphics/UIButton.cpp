#include "UIButton.h"
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"
#include "shaderClass.h"
#include "textRenderer.h"

UIButtonRenderer::UIButtonRenderer()
{
}

UIButtonRenderer::~UIButtonRenderer()
{
    vao->Delete();
    vbo->Delete();
    ebo->Delete();
    vertices.clear();
}

void UIButtonRenderer::init(std::shared_ptr<Shader> sharedShader, GLuint screenWidth, GLuint screenHeight)
{
    this->screenWidth = screenWidth;
    this->screenHeight = screenHeight;
    this->shader = sharedShader;

    // Reserve memory for vertices and indices
    vertices.reserve(maxButtons * 4);
    indices.reserve(maxButtons * 6);

    createVertexArray();
    createVertexBuffer();

    // Create index buffer for buttons
    int offset = 0;
    for (int i = 0; i < maxButtons; ++i) {
        indices.push_back(offset + 0);
        indices.push_back(offset + 1);
        indices.push_back(offset + 2);
        indices.push_back(offset + 2);
        indices.push_back(offset + 3);
        indices.push_back(offset + 0);
        offset += 4;
    }
    ebo = std::make_unique<EBO>(indices.data(), indices.size() * sizeof(GLuint));
}

void UIButtonRenderer::createVertexArray()
{
    vao = std::make_unique<VAO>();
    vao->Bind();
}

void UIButtonRenderer::createVertexBuffer()
{
    vbo = std::make_unique<VBO>(nullptr, sizeof(ButtonVertex) * maxButtons * 4);
    vbo->Bind();

    vao->LinkAttrib(*vbo, 0, 3, GL_FLOAT, sizeof(ButtonVertex), (void*)offsetof(ButtonVertex, position));
    vao->LinkAttrib(*vbo, 1, 3, GL_FLOAT, sizeof(ButtonVertex), (void*)offsetof(ButtonVertex, color));
    vao->LinkAttrib(*vbo, 2, 2, GL_FLOAT, sizeof(ButtonVertex), (void*)offsetof(ButtonVertex, texCoord));
    vao->LinkAttribInteger(*vbo, 3, 1, GL_INT, sizeof(ButtonVertex), (void*)offsetof(ButtonVertex, textureID));
}

void UIButtonRenderer::beginBatch()
{
    vertices.clear();
}

void UIButtonRenderer::endBatch()
{
    flush();
}

void UIButtonRenderer::drawButton(const UIButton& button)
{
    if (vertices.size() >= maxButtons * 4)
    {
        flush();
        beginBatch();
    }

    // Transform button coordinates into screen space
    glm::vec3 bottomLeft = glm::vec3(button.position, 0.0f);
    glm::vec3 bottomRight = glm::vec3(button.position.x + button.size.x, button.position.y, 0.0f);
    glm::vec3 topRight = glm::vec3(button.position + button.size, 0.0f);
    glm::vec3 topLeft = glm::vec3(button.position.x, button.position.y + button.size.y, 0.0f);

    glm::vec2 uv[4] = { {0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f} };

    // Add vertices with color or texture
    vertices.push_back({ bottomLeft, button.color, uv[0], button.textureID });
    vertices.push_back({ bottomRight, button.color, uv[1], button.textureID });
    vertices.push_back({ topRight, button.color, uv[2], button.textureID });
    vertices.push_back({ topLeft, button.color, uv[3], button.textureID });

    // If text is present, render it using TextRenderer
    if (!button.text.empty()) {
        textRenderer->setActiveFont(button.fontName);
        textRenderer->addTextObject(button.text, TextObject(button.text, button.position, 1.0f, button.textColor, button.fontName));
    }
}

void UIButtonRenderer::flush()
{
    if (vertices.empty()) return;

    vao->Bind();
    vbo->UpdateData(vertices.data(), vertices.size() * sizeof(ButtonVertex));
    shader->Activate();

    // Draw all button quads
    glDrawElements(GL_TRIANGLES, static_cast<int>((vertices.size() / 4) * 6), GL_UNSIGNED_INT, 0);
    vao->Unbind();

    // Render text on buttons
    textRenderer->renderAllText();
}