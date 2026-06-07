#include "ProgressBar.hpp"
#include <algorithm>

ProgressBar::ProgressBar(float x, float y, float w, float h)
    : Widget(x, y, w, h) {}

void ProgressBar::Render(UIRenderer& ui) {
    if (!visible_) return;
    ui.DrawRect(x_, y_, w_, h_, bg_);
    if (progress_ > 0)
        ui.DrawRect(x_, y_, w_ * progress_, h_, fg_);
    ui.DrawBorder(x_, y_, w_, h_, {0, 0, 0, 180});
    if (!text_.empty()) {
        ui.DrawText(x_ + 4, y_ + 1, 0xffffffff, "%s", text_.c_str());
    }
}
