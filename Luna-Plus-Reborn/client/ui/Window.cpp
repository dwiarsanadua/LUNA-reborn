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
        // Title bar: 4-step vertical gradient like Old's window cap texture
        float step = th / 4.0f;
        UIColor tc = title_color_;
        UIColor steps[4] = {
            {uint8_t(std::min(255, tc.r * 5 / 3)), uint8_t(std::min(255, tc.g * 5 / 3)),
             uint8_t(std::min(255, tc.b * 5 / 3)), tc.a},
            tc,
            {uint8_t(tc.r * 3 / 4), uint8_t(tc.g * 3 / 4), uint8_t(tc.b * 3 / 4), tc.a},
            {uint8_t(tc.r / 2), uint8_t(tc.g / 2), uint8_t(tc.b / 2), tc.a},
        };
        for (int i = 0; i < 4; ++i)
            ui.DrawRect(x_, y_ + step * i, w_, step + 1, steps[i]);
        // Thin gold separator under the title (Old chrome accent)
        ui.DrawRect(x_, y_ + th - 1, w_, 1, {212, 175, 96, 230});

        // Window body fill with subtle inner panel
        ui.DrawRect(x_, y_ + th, w_, h_ - th, body_color_);
        ui.DrawBorder(x_ + 3, y_ + th + 3, w_ - 6, h_ - th - 6, {255, 255, 255, 18}, 1);

        // Outer border: dark outline + light blue inner bevel
        ui.DrawBorder(x_ - 1, y_ - 1, w_ + 2, h_ + 2, {10, 12, 24, 220}, 1);
        ui.DrawBorder(x_, y_, w_, h_, {150, 200, 255, 190}, 1);

        // Title text with shadow
        if (!title_.empty()) {
            ui.DrawText(x_ + 6, y_ + 4, 0xFF000000, "%s", title_.c_str());
            ui.DrawText(x_ + 5, y_ + 3, 0xffffe8c0, "%s", title_.c_str());
        }

        // Close button (top-right), rounded-look red with bevel
        if (closable_) {
            float cb_x = x_ + w_ - 20, cb_y = y_ + 2, cb_s = 16;
            ui.DrawRect(cb_x, cb_y, cb_s, cb_s, {150, 26, 26, 230});
            ui.DrawRect(cb_x, cb_y, cb_s, cb_s / 2, {200, 60, 60, 120});
            ui.DrawBorder(cb_x, cb_y, cb_s, cb_s, {255, 160, 160, 160}, 1);
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
