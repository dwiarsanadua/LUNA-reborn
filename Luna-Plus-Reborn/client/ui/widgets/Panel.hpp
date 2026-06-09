#pragma once
#include "Widget.hpp"
#include <ui/Window.hpp>

class Panel : public Widget {
public:
    Panel(float x, float y, float w, float h) : Widget(x, y, w, h) {
        window_ = new Window("", x, y, w, h);
        window_->SetDrawChrome(false);
    }
    ~Panel() override { delete window_; }

    template<typename T, typename... Args>
    T* AddWidget(Args&&... args) {
        return window_->AddWidget<T>(std::forward<Args>(args)...);
    }

    void Update(float dt, float mx, float my, bool mousedown, bool mousepressed) override {
        if (!visible_) return;
        window_->Update(dt, mx, my, mousedown, mousepressed);
    }
    void Render(UIRenderer& ui) override {
        if (!visible_) return;
        window_->SetRect(x_, y_, w_, h_);
        window_->Render(ui);
    }

    Window* GetWindow() { return window_; }

private:
    Window* window_ = nullptr;
};
