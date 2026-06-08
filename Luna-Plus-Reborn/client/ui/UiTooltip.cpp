#include "UiTooltip.hpp"
#include <cstring>

static bool s_active = false;
static float s_x = 0, s_y = 0;
static std::string s_text;

void UiTooltip::Set(float x, float y, const std::string& text) {
    if (text.empty()) { s_active = false; return; }
    s_x = x; s_y = y; s_text = text; s_active = true;
}

void UiTooltip::Clear() { s_active = false; s_text.clear(); }

void UiTooltip::Render(UIRenderer& ui) {
    if (!s_active || s_text.empty()) return;
    float tw = (float)s_text.size() * 7.0f + 12.0f;
    float th = 22.0f;
    float tx = s_x + 12.0f;
    float ty = s_y - th - 4.0f;
    if (tx + tw > ui.logicalWidth) tx = ui.logicalWidth - tw - 4.0f;
    if (ty < 0) ty = s_y + 16.0f;
    ui.DrawRect(tx, ty, tw, th, {20, 20, 40, 230});
    ui.DrawBorder(tx, ty, tw, th, {180, 180, 220, 200});
    ui.DrawText(tx + 6, ty + 4, 0xffffffff, "%s", s_text.c_str());
}
