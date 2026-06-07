#include "ListBox.hpp"

ListBox::ListBox(float x, float y, float w, float h)
    : Widget(x, y, w, h) {}

void ListBox::AddItem(const std::string& item) {
    items_.push_back(item);
}

void ListBox::Clear() {
    items_.clear();
    selected_ = -1;
    scroll_offset_ = 0;
}

const std::string& ListBox::GetItem(int idx) const {
    static std::string empty;
    if (idx < 0 || idx >= (int)items_.size()) return empty;
    return items_[idx];
}

void ListBox::Update(float dt, float mx, float my, bool mousedown, bool mousepressed) {
    if (!visible_ || !enabled_) return;
    if (HitTest(mx, my) && mousepressed) {
        float ly = my - y_;
        int idx = (int)(ly / item_h_) + scroll_offset_;
        if (idx >= 0 && idx < (int)items_.size()) {
            selected_ = idx;
            UIEvent e; e.type = UIEvent::ValueChanged; e.int_value = idx; e.str_value = items_[idx];
            FireEvent(e);
        }
    }
}

void ListBox::Render(UIRenderer& ui) {
    if (!visible_) return;
    ui.DrawRect(x_, y_, w_, h_, {20, 20, 40, 220});
    ui.DrawBorder(x_, y_, w_, h_, {60, 60, 80, 150});
    
    float ly = y_;
    int max_visible = (int)(h_ / item_h_);
    for (int i = scroll_offset_; i < (int)items_.size() && i < scroll_offset_ + max_visible; i++) {
        if (i == selected_) {
            ui.DrawRect(x_ + 1, ly, w_ - 2, item_h_, {60, 60, 120, 200});
        }
        ui.DrawText(x_ + 4, ly + 2, i == selected_ ? 0xffffcc00 : 0xffffffff, "%s", items_[i].c_str());
        ly += item_h_;
    }
}
