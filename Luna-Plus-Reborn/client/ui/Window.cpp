#include "Window.hpp"
#include "widgets/Widget.hpp"
#include <algorithm>

Window::Window(const std::string& title, float x, float y, float w, float h)
    : title_(title), x_(x), y_(y), w_(w), h_(h) {}

bool Window::HitTest(float mx, float my) const {
    return mx >= x_ && mx <= x_ + w_ && my >= y_ && my <= y_ + h_;
}

bool Window::HitTestTitle(float mx, float my) const {
    return mx >= x_ && mx <= x_ + w_ && my >= y_ && my <= y_ + GetTitleBarH();
}

void Window::Update(float dt, float mx, float my, bool mousedown, bool mousepressed) {
    if (!visible_) return;
    
    // Window dragging
    if (movable_ && mousedown && HitTestTitle(mx, my) && !dragging_) {
        dragging_ = true;
        drag_off_x_ = mx - x_;
        drag_off_y_ = my - y_;
    }
    if (!mousedown) dragging_ = false;
    if (dragging_) {
        x_ = mx - drag_off_x_;
        y_ = my - drag_off_y_;
    }
    
    if (closable_ && draw_chrome_ && mousepressed) {
        float cx = x_ + w_ - 18, cy = y_ + 3;
        if (mx >= cx && mx <= cx + 14 && my >= cy && my <= cy + 14) {
            Close();
            if (close_cb_) close_cb_();
            return;
        }
    }
    
    float child_mx = mx - x_;
    float child_my = script_layout_ ? (my - y_) : (my - y_ - GetTitleBarH());
    for (auto it = widgets_.rbegin(); it != widgets_.rend(); ++it) {
        (*it)->Update(dt, child_mx, child_my, mousedown, mousepressed);
    }
}

void Window::Render(UIRenderer& ui) {
    if (!visible_) return;
    
    if (custom_bg_cb_) {
        custom_bg_cb_(ui, x_, y_, w_, h_);
    } else if (draw_chrome_) {
        float th = GetTitleBarH();
        // Title bar gradient: top half lighter, bottom half darker
        ui.DrawRect(x_, y_, w_, th / 2, title_color_);
        ui.DrawRect(x_, y_ + th / 2, w_, th - th / 2,
                    {uint8_t(title_color_.r * 2 / 3),
                     uint8_t(title_color_.g * 2 / 3),
                     uint8_t(title_color_.b * 2 / 3),
                     title_color_.a});

        // Window body fill
        ui.DrawRect(x_, y_ + th, w_, h_ - th, body_color_);

        // 3D border (raised style)
        ui.DrawBorder(x_, y_, w_, h_, {150, 200, 255, 200}, 1);
        ui.DrawBorder(x_, y_, w_, h_, {50, 80, 120, 200}, 2);

        // Title text with shadow
        if (!title_.empty()) {
            ui.DrawText(x_ + 6, y_ + 4, 0xFF000000, "%s", title_.c_str());
            ui.DrawText(x_ + 5, y_ + 3, 0xffffffff, "%s", title_.c_str());
        }

        // Close button (pojok kanan atas)
        if (closable_) {
            float cb_x = x_ + w_ - 20, cb_y = y_ + 2, cb_s = 16;
            ui.DrawRect(cb_x, cb_y, cb_s, cb_s, {180, 20, 20, 220});
            ui.DrawText(cb_x + 4, cb_y + 1, 0xFFFFFFFF, "X");
        }
    }
    
    float ox = x_;
    float oy = script_layout_ ? y_ : (y_ + GetTitleBarH());
    for (auto& w : widgets_) {
        if (!w->IsVisible()) continue;
        float wx = w->GetX(), wy = w->GetY();
        w->SetPos(ox + wx, oy + wy);
        w->Render(ui);
        w->SetPos(wx, wy);
    }
}

void Window::HandleKey(int key) {
    if (!visible_) return;
    for (auto& w : widgets_) w->HandleKey(key);
}

void Window::HandleChar(unsigned int codepoint) {
    if (!visible_) return;
    for (auto& w : widgets_) w->HandleChar(codepoint);
}
