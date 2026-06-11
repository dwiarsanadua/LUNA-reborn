#include "GaugeBar.hpp"
#include <ui/UiAtlasRegistry.hpp>
#include <algorithm>

GaugeBar::GaugeBar(float x, float y, float w, float h)
    : Widget(x, y, w, h) {}

void GaugeBar::Render(UIRenderer& ui) {
    if (!visible_) return;
    float fill_w = gauge_width_ > 0 ? gauge_width_ * progress_ : w_ * progress_;
    if (piece_uv_.atlas >= 0) {
        TextureInfo tex = UiAtlasRegistry::LoadAtlasTexture(ui, piece_uv_.atlas);
        if (bgfx::isValid(tex.handle)) {
            float pw = piece_uv_.u2 - piece_uv_.u1;
            float ph = piece_uv_.v2 - piece_uv_.v1;
            (void)pw; (void)ph;
            for (float px = 0; px < fill_w; px += w_) {
                float seg = std::min(w_, fill_w - px);
                ui.DrawImageUV(x_ + px, y_, seg, h_, tex.handle,
                    piece_uv_.u1, piece_uv_.v1, piece_uv_.u2, piece_uv_.v2);
            }
            return;
        }
    }
    // Fallback: filled bar with a top gloss highlight (Old gauge look)
    ui.DrawRect(x_, y_, w_, h_, {30, 30, 36, 210});
    ui.DrawRect(x_, y_, fill_w, h_, fill_color_);
    if (fill_w > 0)
        ui.DrawRect(x_, y_, fill_w, h_ * 0.4f, {255, 255, 255, 60});
    ui.DrawBorder(x_, y_, w_, h_, {0, 0, 0, 180});
}
