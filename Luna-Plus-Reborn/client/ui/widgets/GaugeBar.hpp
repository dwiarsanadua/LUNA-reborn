#pragma once
#include "Widget.hpp"
#include <ui/UiScriptParser.hpp>

class GaugeBar : public Widget {
public:
    GaugeBar(float x = 0, float y = 0, float w = 120, float h = 10);
    void SetProgress(float p) { progress_ = p; }
    float GetProgress() const { return progress_; }
    void SetPieceImage(const UiScriptUV& uv) { piece_uv_ = uv; }
    void SetGaugeWidth(float w) { gauge_width_ = w; }
    void Render(UIRenderer& ui) override;
private:
    float progress_ = 1.0f;
    float gauge_width_ = 100.0f;
    UiScriptUV piece_uv_{-1, 0, 0, 0, 0};
};
