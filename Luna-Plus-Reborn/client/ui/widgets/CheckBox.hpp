#pragma once
#include "Widget.hpp"

class CheckBox : public Widget {
public:
    CheckBox(const std::string& text = "", float x = 0, float y = 0);
    bool IsChecked() const { return checked_; }
    void SetChecked(bool c) { checked_ = c; }
    const std::string& GetText() const { return text_; }
    void SetText(const std::string& t) { text_ = t; }
    void Update(float dt, float mx, float my, bool mousedown, bool mousepressed) override;
    void Render(UIRenderer& ui) override;
private:
    std::string text_;
    bool checked_ = false;
    float box_size_ = 14.0f;
};
