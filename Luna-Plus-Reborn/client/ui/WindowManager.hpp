#pragma once
#include "Window.hpp"
#include <memory>
#include <vector>
#include <string>
#include <unordered_set>

class WindowManager {
public:
    void Update(float dt, float mx, float my, bool mousedown, bool mousepressed);
    void Render(UIRenderer& ui);
    
    Window* Open(const std::string& title, float x, float y, float w, float h);
    Window* LoadFromScript(const std::string& path);
    void PreloadUI(const std::string& interface_path);
    void Close(const std::string& title);
    void CloseAll();
    Window* Find(const std::string& title);
    bool IsOpen(const std::string& title) const;
    bool HasModal() const;

private:
    std::vector<std::unique_ptr<Window>> windows_;
    int next_z_ = 1;
    std::unordered_set<int> preloaded_atlases_;
    
    void BringToFront(Window* win);
};
