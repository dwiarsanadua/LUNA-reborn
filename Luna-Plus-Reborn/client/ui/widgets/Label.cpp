#include "Label.hpp"

Label::Label(const std::string& text, float x, float y, uint32_t color)
    : Widget(x, y, 0, 0), text_(text), color_(color) {}

void Label::Render(UIRenderer& ui) {
    if (!visible_) return;
    if (shadow_) {
        ui.DrawText(x_ + 1, y_ + 1, (color_ & 0x000000FF) | 0x88000000, "%s", text_.c_str());
    }
    ui.DrawText(x_, y_, color_, "%s", text_.c_str());
}
