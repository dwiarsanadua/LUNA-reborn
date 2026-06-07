#pragma once
#include "Widget.hpp"

class Label : public Widget {
public:
    Label(const std::string& text = "", float x = 0, float y = 0, uint32_t color = 0xffffffff);
    void SetText(const std::string& t) { text_ = t; }
    const std::string& GetText() const { return text_; }
    void SetColor(uint32_t c) { color_ = c; }
    void SetShadow(bool s) { shadow_ = s; }
    void Render(UIRenderer& ui) override;
private:
    std::string text_;
    uint32_t color_ = 0xffffffff;
    bool shadow_ = true;
};
