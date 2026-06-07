#pragma once
#include "Widget.hpp"
#include <vector>

class TabPanel : public Widget {
public:
    TabPanel(float x = 0, float y = 0, float w = 400, float h = 300);
    void AddTab(const std::string& name, Widget* content);
    void SetActive(int idx);
    int GetActive() const { return active_; }
    void Update(float dt, float mx, float my, bool mousedown, bool mousepressed) override;
    void Render(UIRenderer& ui) override;
private:
    struct Tab { std::string name; Widget* content = nullptr; };
    std::vector<Tab> tabs_;
    int active_ = 0;
    float tab_h_ = 22.0f;
};
