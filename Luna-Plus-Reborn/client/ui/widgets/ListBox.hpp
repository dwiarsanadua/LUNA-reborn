#pragma once
#include "Widget.hpp"
#include <vector>

class ListBox : public Widget {
public:
    ListBox(float x = 0, float y = 0, float w = 200, float h = 150);
    void AddItem(const std::string& item);
    void Clear();
    int GetSelected() const { return selected_; }
    void SetSelected(int idx) { selected_ = idx; }
    const std::string& GetItem(int idx) const;
    int GetCount() const { return items_.size(); }
    
    void Update(float dt, float mx, float my, bool mousedown, bool mousepressed) override;
    void Render(UIRenderer& ui) override;
private:
    std::vector<std::string> items_;
    int selected_ = -1;
    int scroll_offset_ = 0;
    float item_h_ = 18.0f;
};
