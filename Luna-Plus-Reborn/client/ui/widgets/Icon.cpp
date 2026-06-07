#include "Icon.hpp"

Icon::Icon(bgfx::TextureHandle tex, float x, float y, float size)
    : Widget(x, y, size, size), tex_(tex) {}

void Icon::Render(UIRenderer& ui) {
    if (!visible_) return;
    if (bgfx::isValid(tex_))
        ui.DrawImage(x_, y_, w_, h_, tex_, tint_);
    else
        ui.DrawRect(x_, y_, w_, h_, {60, 60, 80, 200});
}
