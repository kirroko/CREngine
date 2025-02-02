/* Start Header ************************************************************************/
/*!
\file       UIButtion.h
\author     TAN Shun Zhi Tomy, t.shunzhitomy, 2301341, t.shunzhitomy@digipen.edu
\date       Nov 6, 2024
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

enum class BarType{
    None,
    Health,
    Red_Soul,
    Blue_Soul
};

class UIButton {
public:
    glm::vec3 originalPosition;
    glm::vec2 originalSize;
    glm::vec3 position;
    glm::vec2 size;
    glm::vec3 color;
    std::string spriteName;
    std::function<void()> onClick;

    bool isHovered = false;
    int ui_layer;
    BarType barType = BarType::None;

    std::shared_ptr<BatchRenderer2D> batchRenderer;

    UIButton(glm::vec3 pos, glm::vec2 sz, const std::string& sprite, glm::vec3 clr, std::shared_ptr<BatchRenderer2D> renderer, int layer = 0, BarType bar = BarType::None, std::function<void()> callback = nullptr)
       : originalPosition(pos), originalSize(sz), position(pos), size(sz), color(clr), spriteName(sprite), batchRenderer(std::move(renderer)), ui_layer(layer), barType(bar), onClick(callback){} 

    //void updateHealthBar(float healthPercentage)
    //{
    //    if (isHealthBar)
    //    {
    //        healthPercentage = glm::clamp(healthPercentage, 0.0f, 1.0f);

    //        // Adjust width based on health
    //        size.x = originalSize.x * healthPercentage;

    //        // Keep the left side fixed, shift the right side
    //        position.x = originalPosition.x - (originalSize.x - size.x) * 0.5f;
    //    }
    //}

    void updateBar(float percentage)
    {
        percentage = glm::clamp(percentage, 0.f, 1.f);

        size.x = originalSize.x * percentage;

        position.x  = originalPosition.x - (originalSize.x - size.x) * 0.5f;
    }

    void update(glm::vec2 mousePos, bool mousePressed)
    {
        isHovered = (mousePos.x >= position.x && mousePos.x <= position.x + size.x &&
            mousePos.y >= position.y && mousePos.y <= position.y + size.y);

        if (isHovered && mousePressed && onClick)
        {
            onClick();
        }
    }

    void render(const glm::vec3& cameraPosition)
    {
        if (batchRenderer)
        {
            glm::vec3 renderColor = isHovered ? glm::vec3(1.0f, 1.0f, 0.5f) : color;
            glm::vec3 screenPosition = position - cameraPosition + cameraPosition;
            batchRenderer->drawSprite(screenPosition, size, renderColor, spriteName, 0.0f, ui_layer);
        }
    }
};

#endif // UI_BUTTON_H
