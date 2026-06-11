#include "Button.hpp"

Button::Button(const std::string& text, float x, float y, float w, float h)
    : Widget(x, y, w, h), text_(text) {}

void Button::Update(float dt, float mx, float my, bool mousedown, bool mousepressed) {
    if (!visible_ || !enabled_) return;
    bool hit = HitTest(mx, my);
    hovered_ = hit;
    if (hit && mousepressed) {
        pressed_ = true;
        UIEvent e; e.type = UIEvent::Click; e.str_value = text_;
        FireEvent(e);
    }
    if (!mousedown) pressed_ = false;
}

void Button::Render(UIRenderer& ui) {
    if (!visible_) return;

    TextureInfo tex = pressed_ ? tex_pressed_ : (hovered_ ? tex_hover_ : tex_normal_);
    UiScriptUV uv = pressed_ ? uv_pressed_ : (hovered_ ? uv_hover_ : uv_normal_);

    if (use_uv_ && bgfx::isValid(tex.handle)) {
        ui.DrawImageUV(x_, y_, w_, h_, tex.handle, uv.u1, uv.v1, uv.u2, uv.v2);
    } else if (bgfx::isValid(tex.handle)) {
        ui.DrawNinePatch(x_, y_, w_, h_, tex, 4, 4, 4, 4);
    } else {
        // Old-style beveled button: body + top-half highlight + double outline
        UIColor c = pressed_ ? color_pressed_ : (hovered_ ? color_hover_ : color_normal_);
        ui.DrawRect(x_, y_, w_, h_, c);
        if (!pressed_)
            ui.DrawRect(x_, y_, w_, h_ * 0.45f, {255, 255, 255, hovered_ ? uint8_t(50) : uint8_t(30)});
        ui.DrawBorder(x_, y_, w_, h_, {16, 20, 36, 220}, 1);
        ui.DrawBorder(x_ + 1, y_ + 1, w_ - 2, h_ - 2,
                      hovered_ ? UIColor{190, 220, 255, 200} : UIColor{140, 170, 210, 160}, 1);
    }

    if (!text_.empty()) {
        float tx = x_ + w_ * 0.5f - text_.length() * 4.0f;
        float ty = y_ + h_ * 0.5f - 7.0f + (pressed_ ? 1.0f : 0.0f);
        ui.DrawText(tx + 1, ty + 1, 0xb0000000, "%s", text_.c_str());
        ui.DrawText(tx, ty, 0xffffffff, "%s", text_.c_str());
    }
}
