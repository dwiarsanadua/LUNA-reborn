#pragma once
#include "Button.hpp"

class ToggleButton : public Button {
public:
    ToggleButton(const std::string& text = "", float x = 0, float y = 0, float w = 0, float h = 0);
    void SetPushed(bool p) { pushed_ = p; }
    bool IsPushed() const { return pushed_; }
    void Update(float dt, float mx, float my, bool mousedown, bool mousepressed) override;
    void Render(UIRenderer& ui) override;
private:
    bool pushed_ = false;
};
