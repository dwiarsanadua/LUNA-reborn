#pragma once
#include "widgets/Widget.hpp"
#include <vector>
#include <memory>
#include <string>

class Window {
public:
    Window() = default;
    Window(const std::string& title, float x, float y, float w, float h);
    virtual ~Window() = default;

    void SetTitle(const std::string& t) { title_ = t; }
    const std::string& GetTitle() const { return title_; }
    void SetRect(float x, float y, float w, float h) { x_ = x; y_ = y; w_ = w; h_ = h; }
    float GetX() const { return x_; }
    float GetY() const { return y_; }
    float GetW() const { return w_; }
    float GetH() const { return h_; }
    float GetTitleBarH() const { return title_h_; }
    void SetTitleBarH(float th) { title_h_ = th; }
    
    void SetVisible(bool v) { visible_ = v; }
    bool IsVisible() const { return visible_; }
    void SetClosable(bool c) { closable_ = c; }
    void SetMovable(bool m) { movable_ = m; }
    void SetModal(bool m) { modal_ = m; }
    bool IsModal() const { return modal_; }
    void SetScriptLayout(bool v) { script_layout_ = v; }
    bool UsesScriptLayout() const { return script_layout_; }
    void SetDrawChrome(bool v) { draw_chrome_ = v; }
    int GetZOrder() const { return z_order_; }
    void SetZOrder(int z) { z_order_ = z; }

    template<typename T, typename... Args>
    T* AddWidget(Args&&... args) {
        auto w = std::make_unique<T>(std::forward<Args>(args)...);
        T* ptr = w.get();
        widgets_.push_back(std::move(w));
        return ptr;
    }

    template<typename T>
    T* FindWidget(const std::string& id) {
        for (auto& w : widgets_) {
            if (w->GetID() == id) return static_cast<T*>(w.get());
        }
        return nullptr;
    }

    const std::vector<std::unique_ptr<Widget>>& GetWidgets() const { return widgets_; }

    void Update(float dt, float mx, float my, bool mousedown, bool mousepressed);
    void Render(UIRenderer& ui);
    void HandleKey(int key);
    void HandleChar(unsigned int codepoint);
    void Close() { visible_ = false; }
    
    bool HitTest(float mx, float my) const;
    bool HitTestTitle(float mx, float my) const;

    using CloseCallback = std::function<void()>;
    void OnClose(CloseCallback cb) { close_cb_ = cb; }

    using RenderCallback = std::function<void(UIRenderer&, float, float, float, float)>;
    void SetCustomBackground(RenderCallback cb) { custom_bg_cb_ = cb; }
    
    // Allows disabling default window frame for custom textured windows
    void SetColors(UIColor title, UIColor body, UIColor border) {
        title_color_ = title; body_color_ = body; border_color_ = border;
    }
    
    UIColor title_color_ = {42, 42, 110, 230};
    UIColor body_color_ = {16, 16, 32, 220};
    UIColor border_color_ = {100, 180, 255, 200};

private:
    std::string title_;
    float x_ = 100, y_ = 100, w_ = 300, h_ = 200;
    bool visible_ = true;
    bool closable_ = true;
    bool movable_ = true;
    bool modal_ = false;
    bool script_layout_ = false;
    bool draw_chrome_ = true;
    int z_order_ = 0;
    bool dragging_ = false;
    float drag_off_x_ = 0, drag_off_y_ = 0;
    std::vector<std::unique_ptr<Widget>> widgets_;
    CloseCallback close_cb_;
    RenderCallback custom_bg_cb_;
    float title_h_ = 22.0f;
};
