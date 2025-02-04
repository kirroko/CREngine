#ifndef UI_BUTTON_MANAGER_H
#define UI_BUTTON_MANAGER_H

#include <vector>
#include <memory>
#include "UIButton.h"
#include "../ECS/ECS.h"

using namespace Ukemochi;

class UIButtonManager : public Ukemochi::System {
public:
    std::unordered_map<std::string, std::shared_ptr<UIButton>> buttons;

    void addButton(const std::string& id, glm::vec3 pos, glm::vec2 size, const std::string& sprite, glm::vec3 color, std::shared_ptr<BatchRenderer2D> renderer, int ui_layer, BarType barType = BarType::None, std::function<void()> callback = nullptr)
    {
        auto button = std::make_shared<UIButton>(id, pos, size, sprite, color, std::move(renderer), ui_layer, barType, callback);
        buttons[id] = button;
    }

    std::shared_ptr<UIButton> getButtonByID(const std::string& id)
    {
        auto it = buttons.find(id);
        if (it != buttons.end())
        {
            return it->second;
        }

        return nullptr;
    }

    void removeButton(const std::string& id)
    {
        auto it = buttons.find(id);

        if (it != buttons.end())
        {
            buttons.erase(it);
        }
    }

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

    void render(glm::vec3& cameraPos)
    {
        for (auto& [id, button] : buttons)
        {
            button->render(cameraPos);
        }
    }
};

#endif
