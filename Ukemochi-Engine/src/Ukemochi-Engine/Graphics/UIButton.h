#ifndef UI_BUTTON_H
#define UI_BUTTON_H

#include "../vendor/glm/glm/glm.hpp"
#include <string>
#include <vector>
#include "TextRenderer.h"

class Shader;
class BatchRenderer2D;
class Camera;

enum class TextAlignment {
    Center,
    TopLeft,
    TopRight,
    BottomLeft,
    BottomRight
};

/*!
 * @class UIButton
 * @brief Represents a button in the UI, with a background texture and a text label.
 */
class UIButton {
public:
    std::string id;
    glm::vec2 position;    // Screen position
    glm::vec2 size;        // Size of the button
    GLuint textureID;      // Texture for the button background
    std::string text;      // Button label
    glm::vec3 textColor;   // Label color
    std::string fontName;  // Font for the label
    float textScale;       // Text scale
    TextAlignment textAlignment;

    /*!
     * @brief Constructs a UIButton object with the given parameters.
     * @param id of the button
     * @param position Screen position of the button.
     * @param size Size of the button.
     * @param textureID OpenGL texture ID for the button background.
     * @param text Label text on the button.
     * @param textColor Color of the label text.
     * @param fontName Font used for the label text.
     * @param textScale Scale of the label text.
     */
    UIButton(const std::string& id, glm::vec2 position, glm::vec2 size, GLuint textureID, const std::string& text, glm::vec3 textColor, std::string fontName, float textScale, TextAlignment alignment = TextAlignment::Center)
        : id(id), position(position), size(size), textureID(textureID), text(text), textColor(textColor), fontName(fontName), textScale(textScale), textAlignment(alignment) {}
};

/*!
 * @class UIButtonRenderer
 * @brief Renders a collection of UI buttons using a batch renderer and a text renderer.
 */
class UIButtonRenderer {
private:
    std::vector<UIButton> buttons;   // List of buttons to render
    std::shared_ptr<BatchRenderer2D> batchRenderer; // For button background
    TextRenderer* textRenderer;      // For button text
    glm::mat4 projectionMatrix;      // Static orthographic projection
    glm::mat4 viewMatrix;
    std::shared_ptr<Shader> uiShader;

public:

    UIButtonRenderer(std::shared_ptr<BatchRenderer2D> batchRenderer, TextRenderer* textRenderer, int screenWidth, int screenHeight, std::shared_ptr<Shader> uiShader);

    /*!
     * @brief Adds a button to the renderer's collection.
     * @param button The UIButton to add.
     */
    void addButton(const UIButton& button);

    /*!
     * @brief Renders all buttons in the collection.
     */
    void renderButtons(const Camera& camera); 

    void setViewMatrix(const glm::mat4& view);

    void removeButton(const std::string& id);

    void clearButtons();
};

#endif // UI_BUTTON_H
