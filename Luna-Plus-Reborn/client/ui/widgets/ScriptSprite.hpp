#pragma once
#include "Widget.hpp"
#include <ui/UiScriptParser.hpp>

class ScriptSprite : public Widget {
public:
    ScriptSprite(float x, float y, float w, float h, const UiScriptUV& uv);
    void Render(UIRenderer& ui) override;
private:
    UiScriptUV uv_;
};
