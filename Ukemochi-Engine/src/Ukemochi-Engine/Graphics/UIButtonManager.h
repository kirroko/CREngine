/* Start Header ************************************************************************/
/*!
\file       UIButtonManager.h
\author     TAN Shun Zhi Tomy, t.shunzhitomy, 2301341, t.shunzhitomy\@digipen.edu
\date       Feb 6, 2025
\brief      Manages UI buttons, allowing adding, removing, updating, and rendering
            UI elements in the game.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header **************************************************************************/
#ifndef UI_BUTTON_MANAGER_H
#define UI_BUTTON_MANAGER_H

#include <vector>
#include <memory>
#include "UIButton.h"
#include "../ECS/ECS.h"

using namespace Ukemochi;

/*!***********************************************************************
\brief
Manages UI buttons, handling their creation, updates, and rendering.
*************************************************************************/
class UIButtonManager : public Ukemochi::System {
public:
    std::unordered_map<std::string, std::shared_ptr<UIButton>> buttons; //!< Stores all UI buttons by their unique ID.

    /*!***********************************************************************
    \brief
    Adds a new UI button to the manager.

    \param id        Unique identifier for the button.
    \param pos       Position of the button.
    \param size      Size of the button.
    \param sprite    Sprite name associated with the button.
    \param color     Color of the button.
    \param renderer  Batch renderer for rendering the button.
    \param ui_layer  Layer to determine the rendering order.
    \param barType   Type of bar associated with the button (default: None).
    \param hoverEffect Enables hover effect if set to true (default: false).
    \param callback  Function callback executed when the button is clicked (default: nullptr).
    *************************************************************************/
    void addButton(const std::string& id, glm::vec3 pos, glm::vec2 size, const std::string& sprite, glm::vec3 color, std::shared_ptr<BatchRenderer2D> renderer, int ui_layer, BarType barType = BarType::None, bool hoverEffect = false, std::function<void()> callback = nullptr)
    {
        auto button = std::make_shared<UIButton>(id, pos, size, sprite, color, std::move(renderer), ui_layer, barType, hoverEffect, callback);
        buttons[id] = button;
    }

    /*!***********************************************************************
   \brief
   Retrieves a button by its unique ID.

   \param id The unique identifier of the button.
   \return A shared pointer to the UIButton if found, otherwise nullptr.
   *************************************************************************/
    std::shared_ptr<UIButton> getButtonByID(const std::string& id)
    {
        auto it = buttons.find(id);
        if (it != buttons.end())
        {
            return it->second;
        }

        return nullptr;
    }

    /*!***********************************************************************
    \brief
    Removes a button from the manager.

    \param id The unique identifier of the button to be removed.
    *************************************************************************/

    void removeButton(const std::string& id)
    {
        auto it = buttons.find(id);

        if (it != buttons.end())
        {
            buttons.erase(it);
        }
    }

    /*!***********************************************************************
    \brief
    Updates the progress bars associated with buttons based on given percentages.

    \param barPercentages A map of BarType to corresponding percentage values.
    *************************************************************************/
    void updateBars(std::unordered_map<BarType, float>& barPercentages)
    {
        for (auto& [id, button] : buttons)
        {
            if (button->barType != BarType::None)
            {
                auto it = barPercentages.find(button->barType);
                if (it != barPercentages.end())
                {
                    button->updateBar(it->second);
                }
            }
        }
    }

    /*!***********************************************************************
    \brief
    Updates the position of an existing button.

    \param id The unique identifier of the button.
    \param newPosition The new position for the button.
    *************************************************************************/
    void updateButtonPosition(const std::string& id, const glm::vec3& newPosition)
    {
        auto it = buttons.find(id);
        if (it != buttons.end())
        {
            it->second->position = newPosition;
        }
    }

    /*!***********************************************************************
    \brief
    Renders all UI buttons managed by this system.

    \param cameraPos The current camera position to apply proper rendering transformations.
    *************************************************************************/
    void render(glm::vec3& cameraPos)
    {
        for (auto& [id, button] : buttons)
        {
            button->render(cameraPos);
        }
    }
};

#endif
