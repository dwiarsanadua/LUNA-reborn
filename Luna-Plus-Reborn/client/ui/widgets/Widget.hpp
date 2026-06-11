#pragma once
#include <rendering/UIRenderer.hpp>
#include <functional>
#include <string>
#include <vector>

struct UIEvent {
    enum Type { None, Click, RightClick, Hover, DragStart, DragEnd, ValueChanged, TextChanged, TabSelected };
    Type type = None;
    int int_value = 0;
    float float_value = 0;
    std::string str_value;
    void* userdata = nullptr;
};

class Widget {
public:
    Widget() = default;
    Widget(float x, float y, float w, float h) : x_(x), y_(y), w_(w), h_(h) {}
    virtual ~Widget() = default;

    virtual void SetRect(float x, float y, float w, float h) { x_ = x; y_ = y; w_ = w; h_ = h; }
    virtual void SetPos(float x, float y) { x_ = x; y_ = y; }
    float GetX() const { return x_; }
    float GetY() const { return y_; }
    float GetW() const { return w_; }
    float GetH() const { return h_; }

    virtual void Update(float dt, float mx, float my, bool mousedown, bool mousepressed) { (void)dt;(void)mx;(void)my;(void)mousedown;(void)mousepressed; }
    virtual void Render(UIRenderer& ui) = 0;
    virtual bool HandleKey(int key) { (void)key; return false; }
    virtual bool HandleChar(unsigned int codepoint) { (void)codepoint; return false; }

    void SetVisible(bool v) { visible_ = v; }
    bool IsVisible() const { return visible_; }
    void SetEnabled(bool e) { enabled_ = e; }
    bool IsEnabled() const { return enabled_; }
    bool IsHovered() const { return hovered_; }
    void SetID(const std::string& id) { id_ = id; }
    const std::string& GetID() const { return id_; }

    using EventCallback = std::function<void(const UIEvent&)>;
    void OnEvent(EventCallback cb) { callback_ = cb; }
    bool HasEvent() const { return (bool)callback_; }
    void SetUserData(void* data) { userdata_ = data; }
    void* GetUserData() const { return userdata_; }
    void SetTooltip(const std::string& t) { tooltip_ = t; }
    const std::string& GetTooltip() const { return tooltip_; }

    virtual bool HitTest(float mx, float my) const {
        return visible_ && mx >= x_ && mx <= x_ + w_ && my >= y_ && my <= y_ + h_;
    }

protected:
    void FireEvent(const UIEvent& e) { if (callback_) callback_(e); }
    float x_ = 0, y_ = 0, w_ = 0, h_ = 0;
    bool visible_ = true;
    bool enabled_ = true;
    bool hovered_ = false;
    std::string id_;
    EventCallback callback_;
    void* userdata_ = nullptr;
    std::string tooltip_;
};
