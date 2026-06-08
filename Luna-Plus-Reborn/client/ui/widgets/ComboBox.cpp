#include "ComboBox.hpp"

ComboBox::ComboBox(float x, float y, float w, float h)
    : Widget(x, y, w, h) { (void)dropdown_h_; }

void ComboBox::AddItem(const std::string& item) {
    items_.push_back(item);
    if (selected_ < 0) selected_ = 0;
}

void ComboBox::Clear() {
    items_.clear();
    selected_ = -1;
    open_ = false;
}

void ComboBox::SetSelected(int idx) {
    if (idx >= 0 && idx < (int)items_.size()) {
        selected_ = idx;
        UIEvent e; e.type = UIEvent::ValueChanged; e.int_value = idx; e.str_value = items_[idx];
        FireEvent(e);
    }
}

const std::string& ComboBox::GetSelectedText() const {
    static std::string empty;
    if (selected_ < 0 || selected_ >= (int)items_.size()) return empty;
    return items_[selected_];
}

void ComboBox::Update(float dt, float mx, float my, bool mousedown, bool mousepressed) {
    if (!visible_ || !enabled_) return;
    bool hit = HitTest(mx, my);
    if (mousepressed) {
        if (hit) {
            open_ = !open_;
        } else if (open_) {
            // Check dropdown list clicks
            float dy = y_ + h_;
            for (int i = 0; i < (int)items_.size(); i++) {
                if (mx >= x_ && mx <= x_ + w_ && my >= dy && my <= dy + h_) {
                    SetSelected(i);
                    open_ = false;
                    return;
                }
                dy += h_;
            }
            open_ = false;
        }
    }
}

void ComboBox::Render(UIRenderer& ui) {
    if (!visible_) return;
    ui.DrawRect(x_, y_, w_, h_, {30, 30, 50, 220});
    ui.DrawBorder(x_, y_, w_, h_, {60, 60, 80, 150});
    if (selected_ >= 0 && selected_ < (int)items_.size()) {
        ui.DrawText(x_ + 4, y_ + 3, 0xffffffff, "%s", items_[selected_].c_str());
    }
    // Dropdown arrow
    ui.DrawRect(x_ + w_ - 16, y_ + 4, 12, h_ - 8, {60, 60, 80, 200});
    ui.DrawText(x_ + w_ - 14, y_ + 3, 0xffffffff, "v");
    
    if (open_) {
        float dy = y_ + h_;
        for (int i = 0; i < (int)items_.size(); i++) {
            UIColor c = (i == selected_) ? UIColor{50, 50, 80, 230} : UIColor{20, 20, 40, 230};
            ui.DrawRect(x_, dy, w_, h_, c);
            ui.DrawText(x_ + 4, dy + 3, 0xffffffff, "%s", items_[i].c_str());
            dy += h_;
        }
        ui.DrawBorder(x_, y_ + h_, w_, items_.size() * h_, {80, 80, 120, 200});
    }
}
