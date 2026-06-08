#pragma once
#include <rendering/UIRenderer.hpp>
#include <string>

class UiTooltip {
public:
    static void Set(float x, float y, const std::string& text);
    static void Clear();
    static void Render(UIRenderer& ui);
};
