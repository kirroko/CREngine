#ifndef UI_BUTTON_MANAGER_H
#define UI_BUTTON_MANAGER_H

#include <vector>
#include <memory>
#include "UIButton.h"

class UIButtonManager {
public:
	std::vector<std::shared_ptr<UIButton>> buttons;

    void addButton(glm::vec3 pos, glm::vec2 size, const std::string& sprite, glm::vec3 color, std::shared_ptr<BatchRenderer2D> renderer, int ui_layer, std::function<void()> callback) 
    {
        buttons.push_back(std::make_shared<UIButton>(pos, size, sprite, color, std::move(renderer), ui_layer, callback));
    }

    void update(glm::vec2 mousePos, bool mousePressed) 
    {
        for (auto& button : buttons) 
        {
            button->update(mousePos, mousePressed);
        }
    }

    void render(glm::vec3& cameraPos)
    {
        for (auto& button : buttons) 
        {
            button->render(cameraPos);
        }


    }
};

#endif
