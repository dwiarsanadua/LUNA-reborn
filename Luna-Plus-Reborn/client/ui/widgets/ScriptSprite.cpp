#include "ScriptSprite.hpp"
#include <ui/UiAtlasRegistry.hpp>

ScriptSprite::ScriptSprite(float x, float y, float w, float h, const UiScriptUV& uv)
    : Widget(x, y, w, h), uv_(uv) {}

void ScriptSprite::Render(UIRenderer& ui) {
    if (!visible_ || uv_.atlas < 0) return;
    TextureInfo tex = UiAtlasRegistry::LoadAtlasTexture(ui, uv_.atlas);
    if (!bgfx::isValid(tex.handle)) return;
    ui.DrawImageUV(x_, y_, w_, h_, tex.handle, uv_.u1, uv_.v1, uv_.u2, uv_.v2);
}
