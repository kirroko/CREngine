/* Start Header ************************************************************************/
/*!
\file       UIButtion.h
\author     TAN Shun Zhi Tomy, t.shunzhitomy, 2301341, t.shunzhitomy\@digipen.edu
\date       Feb 6, 2025
\brief      Defines the UIButton and UIButtonRenderer classes for creating and managing
            UI buttons in a graphical application.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header **************************************************************************/
#ifndef UI_BUTTON_H
#define UI_BUTTON_H

#include "../vendor/glm/glm/glm.hpp"
#include <string>
#include <functional>
#include <memory>

#include "BatchRenderer.h"

/*!***********************************************************************
\brief
Defines the types of UI bars that can be used in the game.
*************************************************************************/
enum class BarType{
    None,
    Health,
    Blue_Soul,
    Red_Soul,
    Blue_Charge_Bar,
    Red_Charge_Bar
};

/*!***********************************************************************
\brief
Represents a UI button with properties like position, size, color, and
callbacks for interactions.
*************************************************************************/
class UIButton {
public:
    std::string id; // Unique ID for each button
    glm::vec3 originalPosition;
    glm::vec2 originalSize;
    glm::vec3 position;
    glm::vec2 size;
    glm::vec3 color;
    std::string spriteName;
    std::function<void()> onClick;
    bool enableHoverEffect = false;

    bool isHovered = false;
    int ui_layer;
    BarType barType = BarType::None;
    float darkenTimer = 0.0f;          // Timer for darken effect
    const float darkenDuration = 1.f; // Duration of the darken effect

    std::shared_ptr<BatchRenderer2D> batchRenderer;

    /*!***********************************************************************
    \brief
    Constructor for UIButton.

    \param buttonID  Unique ID for the button.
    \param pos       Position of the button.
    \param sz        Size of the button.
    \param sprite    Sprite texture name.
    \param clr       Color of the button.
    \param renderer  Batch renderer for UI elements.
    \param layer     UI layer of the button.
    \param bar       Bar type associated with the button.
    \param hoverEffect Enables hover effect if true.
    \param callback  Callback function executed when the button is clicked.
    *************************************************************************/
    UIButton(const std::string& buttonID, glm::vec3 pos, glm::vec2 sz, const std::string& sprite, glm::vec3 clr, std::shared_ptr<BatchRenderer2D> renderer, int layer = 0, BarType bar = BarType::None, bool hoverEffect = false, std::function<void()> callback = nullptr)
       : id(buttonID), originalPosition(pos), originalSize(sz), position(pos), size(sz), color(clr), spriteName(sprite), batchRenderer(std::move(renderer)), ui_layer(layer), barType(bar), enableHoverEffect(hoverEffect), onClick(callback){} 

    /*!***********************************************************************
    \brief
    Updates the size of a progress bar based on a percentage value.

    \param percentage The percentage to set the bar to (clamped between 0.0f and 1.0f).
    *************************************************************************/
    void updateBar(float percentage)
    {
        percentage = glm::clamp(percentage, 0.f, 1.f);

        size.x = originalSize.x * percentage;

        position.x  = originalPosition.x - (originalSize.x - size.x) * 0.5f;
    }

    /*!***********************************************************************
    \brief
    Updates the hover state and handles button clicks.

    \param mousePos      The current mouse position.
    \param mousePressed  Boolean indicating if the mouse button is pressed.
    *************************************************************************/
    void update(glm::vec2 mousePos, bool mousePressed)
    {
        isHovered = (mousePos.x >= position.x && mousePos.x <= position.x + size.x &&
            mousePos.y >= position.y && mousePos.y <= position.y + size.y);

        if (isHovered && mousePressed && onClick)
        {
            onClick();
        }
    }

    /*!***********************************************************************
    \brief
    Triggers a darkening effect when the button is clicked.
    *************************************************************************/
    void triggerDarkenEffect() 
    {
        darkenTimer = darkenDuration;
    }

    /*!***********************************************************************
    \brief
    Renders the button with hover and darkening effects.

    \param cameraPosition The current camera position.
    *************************************************************************/
    void render(const glm::vec3& cameraPosition)
    {
        if (batchRenderer)
        {
            glm::vec3 renderColor = color;

            // Apply hover effect only if enabled
            if (isHovered && enableHoverEffect) 
            {
                float hoverIntensity = 0.8f;    // Darken the button when hovered
                renderColor *= hoverIntensity;  // Reduce brightness
            }

            if (darkenTimer > 0.0f) 
            {
                renderColor *= 0.5f; // Darken more when pressed
                darkenTimer -= 0.016f; // Assuming ~60 FPS, decrease timer
            }

            glm::vec3 screenPosition = position - cameraPosition + cameraPosition;
            batchRenderer->drawSprite(screenPosition, size, renderColor, 1.f, spriteName, 0.0f, ui_layer);
        }
    }
};

#endif // UI_BUTTON_H
