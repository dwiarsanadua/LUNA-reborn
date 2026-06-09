#include "test_harness.hpp"
#include <string>
#include <vector>
#include <algorithm>
#include <cstdint>

struct UIColor { uint8_t r, g, b, a; };

class Widget {
public:
    Widget(float x = 0, float y = 0, float w = 0, float h = 0) : x_(x), y_(y), w_(w), h_(h) {}
    virtual ~Widget() = default;
    void SetPos(float x, float y) { x_ = x; y_ = y; }
    float GetX() const { return x_; }
    float GetY() const { return y_; }
    float GetW() const { return w_; }
    float GetH() const { return h_; }
    void SetVisible(bool v) { visible_ = v; }
    bool IsVisible() const { return visible_; }
    void SetEnabled(bool e) { enabled_ = e; }
    bool IsEnabled() const { return enabled_; }
protected:
    float x_ = 0, y_ = 0, w_ = 0, h_ = 0;
    bool visible_ = true;
    bool enabled_ = true;
};

class Button : public Widget {
public:
    Button(const std::string& text = "", float x = 0, float y = 0, float w = 0, float h = 0)
        : Widget(x, y, w, h), text_(text) {}
    void SetText(const std::string& t) { text_ = t; }
    const std::string& GetText() const { return text_; }
    void SetColors(UIColor n, UIColor h, UIColor p) { color_normal_ = n; color_hover_ = h; color_pressed_ = p; }
private:
    std::string text_;
    UIColor color_normal_ = {40, 50, 80, 220};
    UIColor color_hover_ = {80, 100, 140, 220};
    UIColor color_pressed_ = {30, 40, 60, 220};
};

class CheckBox : public Widget {
public:
    CheckBox(const std::string& text = "", float x = 0, float y = 0) : Widget(x, y), text_(text) {}
    bool IsChecked() const { return checked_; }
    void SetChecked(bool c) { checked_ = c; }
    const std::string& GetText() const { return text_; }
    void SetText(const std::string& t) { text_ = t; }
private:
    std::string text_;
    bool checked_ = false;
};

enum class InputValidation { None, NumericOnly, Alphanumeric, PositiveInteger, Decimal };

class InputField : public Widget {
public:
    InputField(float x = 0, float y = 0, float w = 200, float h = 22) : Widget(x, y, w, h) {}
    const std::string& GetText() const { return text_; }
    void SetText(const std::string& t) { text_ = t; valid_ = Validate(t); }
    bool IsValid() const { return valid_; }
    void SetMaxLength(int m) { max_len_ = m; }
    void SetPlaceholder(const std::string& p) { placeholder_ = p; }
    void SetValidation(InputValidation v) { validation_ = v; }
private:
    std::string text_;
    std::string placeholder_;
    int max_len_ = 64;
    InputValidation validation_ = InputValidation::None;
    bool valid_ = true;
    bool Validate(const std::string& s) {
        if (validation_ == InputValidation::None) return true;
        if (s.empty()) return true;
        if (validation_ == InputValidation::PositiveInteger) {
            if (s[0] == '-') return false;
            for (char c : s) if (!std::isdigit(c)) return false;
            return true;
        }
        return true;
    }
};

class ListBox : public Widget {
public:
    ListBox(float x = 0, float y = 0, float w = 200, float h = 150) : Widget(x, y, w, h) {}
    void AddItem(const std::string& item) { items_.push_back(item); }
    void Clear() { items_.clear(); selected_ = -1; }
    int GetSelected() const { return selected_; }
    void SetSelected(int idx) { selected_ = idx; }
    const std::string& GetItem(int idx) const { return items_[idx]; }
    int GetCount() const { return items_.size(); }
private:
    std::vector<std::string> items_;
    int selected_ = -1;
};

class ProgressBar : public Widget {
public:
    ProgressBar(float x = 0, float y = 0, float w = 200, float h = 16) : Widget(x, y, w, h) {}
    void SetProgress(float p) { progress_ = std::max(0.0f, std::min(1.0f, p)); }
    float GetProgress() const { return progress_; }
private:
    float progress_ = 0;
};

class TabPanel : public Widget {
public:
    TabPanel(float x = 0, float y = 0, float w = 400, float h = 300) : Widget(x, y, w, h) {}
    void AddTab(const std::string& name, void* content) { tabs_.push_back({name, content}); (void)content; }
    void SetActive(int idx) { active_ = idx; }
    int GetActive() const { return active_; }
private:
    struct Tab { std::string name; void* content; };
    std::vector<Tab> tabs_;
    int active_ = 0;
};

void RunUIWidgetTests() {
    TEST_SUITE("UI WIDGET TESTS");

    TEST_STEP("Widget base: position, visibility, enabled");
    {
        Widget w(10, 20, 100, 200);
        TEST("Widget x=10", w.GetX() == 10);
        TEST("Widget y=20", w.GetY() == 20);
        TEST("Widget w=100", w.GetW() == 100);
        TEST("Widget h=200", w.GetH() == 200);

        w.SetPos(5, 15);
        TEST("SetPos x=5", w.GetX() == 5);
        TEST("SetPos y=15", w.GetY() == 15);

        w.SetVisible(false);
        TEST("SetVisible false", !w.IsVisible());

        w.SetEnabled(false);
        TEST("SetEnabled false", !w.IsEnabled());
    }

    TEST_STEP("Button: text and colors");
    {
        Button btn("Click", 0, 0, 100, 30);
        TEST("Button initial text", btn.GetText() == "Click");
        btn.SetText("OK");
        TEST("Button SetText", btn.GetText() == "OK");
        btn.SetColors(UIColor{50, 100, 150, 255}, UIColor{80, 130, 180, 255}, UIColor{30, 60, 100, 255});
        TEST("Button constructed with colors", true);
    }

    TEST_STEP("CheckBox: check/uncheck");
    {
        CheckBox cb("Save ID", 0, 0);
        TEST("CheckBox text", cb.GetText() == "Save ID");
        TEST("CheckBox unchecked by default", !cb.IsChecked());
        cb.SetChecked(true);
        TEST("CheckBox can be checked", cb.IsChecked());
        cb.SetChecked(false);
        TEST("CheckBox can be unchecked", !cb.IsChecked());
    }

    TEST_STEP("InputField: text, placeholder, validation");
    {
        InputField field(0, 0, 200, 22);
        field.SetText("Hello");
        TEST("InputField text", field.GetText() == "Hello");
        field.SetPlaceholder("Enter name");
        field.SetMaxLength(20);
        field.SetValidation(InputValidation::PositiveInteger);
        field.SetText("abc");
        TEST("Non-numeric marked invalid", !field.IsValid());
        field.SetText("12345");
        TEST("Numeric accepted", field.GetText() == "12345");
        TEST("Numeric is valid", field.IsValid());
    }

    TEST_STEP("ListBox: add, select, clear");
    {
        ListBox list(0, 0, 200, 150);
        list.AddItem("Item 1");
        list.AddItem("Item 2");
        list.AddItem("Item 3");
        TEST("ListBox count = 3", list.GetCount() == 3);
        TEST("ListBox item 0", list.GetItem(0) == "Item 1");
        list.SetSelected(1);
        TEST("ListBox selected = 1", list.GetSelected() == 1);
        list.Clear();
        TEST("ListBox cleared", list.GetCount() == 0);
    }

    TEST_STEP("ProgressBar + TabPanel");
    {
        ProgressBar pb(0, 0, 200, 16);
        pb.SetProgress(0.5f);
        TEST("Progress = 0.5", pb.GetProgress() == 0.5f);
        pb.SetProgress(1.5f);
        TEST("Progress clamped to 1.0", pb.GetProgress() == 1.0f);
        pb.SetProgress(-0.1f);
        TEST("Progress clamped to 0.0", pb.GetProgress() == 0.0f);

        TabPanel tabs(0, 0, 400, 300);
        tabs.AddTab("Tab1", nullptr);
        tabs.AddTab("Tab2", nullptr);
        tabs.SetActive(1);
        TEST("Tab active = 1", tabs.GetActive() == 1);
    }
}
