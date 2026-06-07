#pragma once
#include "Widget.hpp"

class ProgressBar : public Widget {
public:
    ProgressBar(float x = 0, float y = 0, float w = 200, float h = 16);
    void SetProgress(float p) { progress_ = std::max(0.0f, std::min(1.0f, p)); }
    float GetProgress() const { return progress_; }
    void SetColors(UIColor fg, UIColor bg) { fg_ = fg; bg_ = bg; }
    void SetText(const std::string& t) { text_ = t; }
    void Render(UIRenderer& ui) override;
private:
    float progress_ = 0;
    UIColor fg_ = {60, 200, 60, 255};
    UIColor bg_ = {60, 0, 0, 180};
    std::string text_;
};
