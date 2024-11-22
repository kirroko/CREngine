#include "PreCompile.h"
#include "UIButton.h"
#include <../vendor/glm/glm/gtc/matrix_transform.hpp> 
#include "shaderClass.h"
#include "BatchRenderer.h"
#include "Camera2D.h"
/*!
 * @brief Constructs a UIButtonRenderer with the given renderers and screen dimensions.
 */
UIButtonRenderer::UIButtonRenderer(std::shared_ptr<BatchRenderer2D> batchRenderer,
    TextRenderer* textRenderer,
    int screenWidth,
    int screenHeight,
    std::shared_ptr<Shader> uiShader)
    : batchRenderer(batchRenderer), textRenderer(textRenderer), uiShader(uiShader) {
    projectionMatrix = glm::ortho(0.0f, static_cast<float>(screenWidth), 0.0f, static_cast<float>(screenHeight));
}



void UIButtonRenderer::setViewMatrix(const glm::mat4& view)
{
    viewMatrix = view;
}

/*!
 * @brief Adds a button to the renderer's collection.
 */
void UIButtonRenderer::addButton(const UIButton& button) {
    buttons.push_back(button);
}

/*!
 * @brief Renders all buttons in the collection.
 */
void UIButtonRenderer::renderButtons(const Camera& camera)
{
    batchRenderer->setActiveShader(uiShader);
    uiShader->setMat4("projection", projectionMatrix); 

    batchRenderer->beginBatch();

    for (const UIButton& button : buttons) 
    {
        // Adjust the button position by subtracting the camera's position
        glm::vec2 adjustedPosition = button.position + camera.position;

        GLfloat uvCoordinates[8] = { 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f };
        batchRenderer->drawSprite(adjustedPosition, button.size, glm::vec3(1.0f), button.textureID, uvCoordinates);
    }

    batchRenderer->endBatch();

    for (const UIButton& button : buttons) 
    {
        glm::vec2 textPos = button.position + glm::vec2(button.size.x * 0.f, button.size.y * -0.1f);
        textPos -= glm::vec2(textRenderer->getTextWidth(button.text, button.textScale, button.fontName) * 0.5f, 0.0f);
        textRenderer->addTextObject("ui_" + button.text, TextObject(button.text, textPos, button.textScale, button.textColor, button.fontName));
    }

    textRenderer->renderAllText();
}

void UIButtonRenderer::removeButton(const std::string& id) 
{
    buttons.erase(std::remove_if(buttons.begin(), buttons.end(), [&](const UIButton& button) {return button.id == id;}),buttons.end());
}

void UIButtonRenderer::clearButtons() 
{
    buttons.clear();
}

std::vector<UIButton>& UIButtonRenderer::GetButtons()
{
    return buttons;
}
