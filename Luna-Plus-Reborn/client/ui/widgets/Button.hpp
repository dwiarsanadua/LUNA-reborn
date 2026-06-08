#pragma once
#include "Widget.hpp"
#include <ui/UiScriptParser.hpp>

class Button : public Widget {
public:
    Button(const std::string& text = "", float x = 0, float y = 0, float w = 0, float h = 0);
    void SetText(const std::string& t) { text_ = t; }
    const std::string& GetText() const { return text_; }
    void Update(float dt, float mx, float my, bool mousedown, bool mousepressed) override;
    void Render(UIRenderer& ui) override;
    void SetColors(UIColor n, UIColor h, UIColor p) { color_normal_ = n; color_hover_ = h; color_pressed_ = p; }
    void SetTextures(const TextureInfo& normal, const TextureInfo& hover, const TextureInfo& pressed) {
        tex_normal_ = normal;
        tex_hover_ = hover;
        tex_pressed_ = pressed;
    }
    void SetUVs(const UiScriptUV& normal, const UiScriptUV& hover, const UiScriptUV& pressed) {
        uv_normal_ = normal;
        uv_hover_ = hover;
        uv_pressed_ = pressed;
        use_uv_ = normal.atlas >= 0;
    }

protected:
    std::string text_;
    bool pressed_ = false;
    UIColor color_normal_ = {40, 50, 80, 220};
    UIColor color_hover_ = {80, 100, 140, 220};
    UIColor color_pressed_ = {30, 40, 60, 220};
    TextureInfo tex_normal_;
    TextureInfo tex_hover_;
    TextureInfo tex_pressed_;
    UiScriptUV uv_normal_{-1,0,0,0,0};
    UiScriptUV uv_hover_{-1,0,0,0,0};
    UiScriptUV uv_pressed_{-1,0,0,0,0};
    bool use_uv_ = false;
};
