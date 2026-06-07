#include "TabPanel.hpp"

TabPanel::TabPanel(float x, float y, float w, float h)
    : Widget(x, y, w, h) {}

void TabPanel::AddTab(const std::string& name, Widget* content) {
    Tab t;
    t.name = name;
    t.content = content;
    tabs_.push_back(t);
    if (content) content->SetVisible(active_ == (int)tabs_.size() - 1);
}

void TabPanel::SetActive(int idx) {
    if (idx < 0 || idx >= (int)tabs_.size()) return;
    if (tabs_[active_].content) tabs_[active_].content->SetVisible(false);
    active_ = idx;
    if (tabs_[active_].content) tabs_[active_].content->SetVisible(true);
    UIEvent e; e.type = UIEvent::TabSelected; e.int_value = idx;
    FireEvent(e);
}

void TabPanel::Update(float dt, float mx, float my, bool mousedown, bool mousepressed) {
    if (!visible_) return;
    // Tab header clicks
    if (mousepressed && my >= y_ && my <= y_ + tab_h_) {
        float tx = x_;
        for (int i = 0; i < (int)tabs_.size(); i++) {
            float tw = tabs_[i].name.length() * 9.0f + 16.0f;
            if (mx >= tx && mx <= tx + tw) {
                SetActive(i);
                return;
            }
            tx += tw;
        }
    }
    // Update active content
    if (active_ >= 0 && active_ < (int)tabs_.size() && tabs_[active_].content) {
        tabs_[active_].content->Update(dt, mx - x_, my - y_ - tab_h_, mousedown, mousepressed);
    }
}

void TabPanel::Render(UIRenderer& ui) {
    if (!visible_) return;
    // Tab headers
    float tx = x_;
    for (int i = 0; i < (int)tabs_.size(); i++) {
        float tw = tabs_[i].name.length() * 9.0f + 16.0f;
        UIColor c = (i == active_) ? UIColor{40, 40, 60, 230} : UIColor{20, 20, 40, 220};
        ui.DrawRect(tx, y_, tw, tab_h_, c);
        ui.DrawBorder(tx, y_, tw, tab_h_, {60, 60, 80, 150});
        ui.DrawText(tx + 6, y_ + 3, 0xffffffff, "%s", tabs_[i].name.c_str());
        tx += tw;
    }
    // Content area
    float cy = y_ + tab_h_;
    float ch = h_ - tab_h_;
    ui.DrawRect(x_, cy, w_, ch, {16, 16, 32, 220});
    ui.DrawBorder(x_, cy, w_, ch, {60, 60, 80, 150});
    // Render active content
    if (active_ >= 0 && active_ < (int)tabs_.size() && tabs_[active_].content) {
        tabs_[active_].content->SetPos(x_ + 4, cy + 4);
        tabs_[active_].content->Render(ui);
    }
}
