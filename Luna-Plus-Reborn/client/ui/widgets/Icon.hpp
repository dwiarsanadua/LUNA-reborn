#pragma once
#include "Widget.hpp"

class Icon : public Widget {
public:
    Icon(bgfx::TextureHandle tex = BGFX_INVALID_HANDLE, float x = 0, float y = 0, float size = 32);
    void SetTexture(bgfx::TextureHandle t) { tex_ = t; }
    void SetTint(UIColor c) { tint_ = c; }
    void Render(UIRenderer& ui) override;
private:
    bgfx::TextureHandle tex_ = BGFX_INVALID_HANDLE;
    UIColor tint_ = {255, 255, 255, 255};
};
