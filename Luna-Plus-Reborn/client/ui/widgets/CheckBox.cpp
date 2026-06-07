#include "CheckBox.hpp"

CheckBox::CheckBox(const std::string& text, float x, float y)
    : Widget(x, y, 0, box_size_), text_(text) {}

void CheckBox::Update(float dt, float mx, float my, bool mousedown, bool mousepressed) {
    if (!visible_ || !enabled_) return;
    if (HitTest(mx, my) && mousepressed) {
        checked_ = !checked_;
        UIEvent e; e.type = UIEvent::ValueChanged; e.int_value = checked_ ? 1 : 0;
        FireEvent(e);
    }
}

void CheckBox::Render(UIRenderer& ui) {
    if (!visible_) return;
    // Checkbox box
    ui.DrawRect(x_, y_, box_size_, box_size_, {20, 20, 40, 220});
    ui.DrawBorder(x_, y_, box_size_, box_size_, {60, 60, 80, 150});
    if (checked_) {
        ui.DrawText(x_ + 2, y_ - 1, 0xffffffff, "X");
    }
    // Label
    ui.DrawText(x_ + box_size_ + 6, y_ + 1, 0xffffffff, "%s", text_.c_str());
}
