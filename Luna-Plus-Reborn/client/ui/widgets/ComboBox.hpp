#pragma once
#include "Widget.hpp"
#include <vector>

class ComboBox : public Widget {
public:
    ComboBox(float x = 0, float y = 0, float w = 200, float h = 22);
    void AddItem(const std::string& item);
    void Clear();
    int GetSelected() const { return selected_; }
    void SetSelected(int idx);
    const std::string& GetSelectedText() const;
    void Update(float dt, float mx, float my, bool mousedown, bool mousepressed) override;
    void Render(UIRenderer& ui) override;
private:
    std::vector<std::string> items_;
    int selected_ = -1;
    bool open_ = false;
    float dropdown_h_ = 100;
};
