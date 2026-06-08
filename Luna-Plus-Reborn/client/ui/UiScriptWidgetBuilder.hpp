#pragma once
class Window;
struct UiElement;

namespace UiScriptWidgetBuilder {
    void AddTree(Window* win, const UiElement& root);
    void AddElement(Window* win, const UiElement& elem);
}
