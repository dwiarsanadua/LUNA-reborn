#include "InputField.hpp"
#include <algorithm>
#include <cctype>

InputField::InputField(float x, float y, float w, float h)
    : Widget(x, y, w, h) {}

void InputField::SetText(const std::string& t) {
    text_ = t;
    valid_ = Validate(text_);
    UIEvent e; e.type = UIEvent::TextChanged; e.str_value = text_;
    FireEvent(e);
}

bool InputField::Validate(const std::string& s) {
    if (validation_ == InputValidation::None) {
        return custom_validator_ ? custom_validator_(s) : true;
    }
    if (s.empty()) return true;
    switch (validation_) {
    case InputValidation::NumericOnly:
        for (char c : s) if (!std::isdigit(c)) return false;
        return true;
    case InputValidation::Alphanumeric:
        for (char c : s) if (!std::isalnum(c)) return false;
        return true;
    case InputValidation::PositiveInteger: {
        if (s.empty() || s[0] == '-') return false;
        for (char c : s) if (!std::isdigit(c)) return false;
        int val = std::atoi(s.c_str());
        return val >= num_min_ && val <= num_max_;
    }
    case InputValidation::Decimal: {
        int dots = 0;
        for (char c : s) {
            if (c == '.') { dots++; if (dots > 1) return false; }
            else if (!std::isdigit(c)) return false;
        }
        return true;
    }
    default: return true;
    }
}

void InputField::Update(float dt, float mx, float my, bool mousedown, bool mousepressed) {
    if (!visible_ || !enabled_) return;
    if (mousepressed) {
        focused_ = HitTest(mx, my);
    }
    cursor_timer_ += dt;
    if (cursor_timer_ > 0.5f) { cursor_timer_ = 0; cursor_show_ = !cursor_show_; }
}

void InputField::Render(UIRenderer& ui) {
    if (!visible_) return;
    UIColor bg = focused_ ? UIColor{30, 30, 50, 240} : UIColor{20, 20, 40, 220};
    UIColor border = focused_ ? UIColor{150, 200, 255, 200} : UIColor{60, 60, 80, 150};
    if (!valid_ && !text_.empty()) {
        border = UIColor{255, 80, 80, 220};
    }
    ui.DrawRect(x_, y_, w_, h_, bg);
    ui.DrawBorder(x_, y_, w_, h_, border);
    if (text_.empty() && !focused_) {
        ui.DrawText(x_ + 4, y_ + 3, 0xff666666, "%s", placeholder_.c_str());
    } else {
        ui.DrawText(x_ + 4, y_ + 3, 0xffffffff, "%s", text_.c_str());
        if (focused_ && cursor_show_) {
            float cx = x_ + 4 + text_.length() * 9.0f;
            ui.DrawRect(cx, y_ + 3, 2, h_ - 6, {200, 200, 255, 200});
        }
    }
}

bool InputField::HandleChar(unsigned int codepoint) {
    if (!focused_ || !visible_) return false;
    if (codepoint >= 32 && codepoint <= 126 && (int)text_.length() < max_len_) {
        std::string candidate = text_ + (char)codepoint;
        if (Validate(candidate)) {
            text_ += (char)codepoint;
            valid_ = true;
            UIEvent e; e.type = UIEvent::TextChanged; e.str_value = text_;
            FireEvent(e);
            return true;
        }
    }
    return false;
}

bool InputField::HandleKey(int key) {
    if (!focused_ || !visible_) return false;
    if (key == 259 && !text_.empty()) { // BACKSPACE
        text_.pop_back();
        valid_ = Validate(text_);
        UIEvent e; e.type = UIEvent::TextChanged; e.str_value = text_;
        FireEvent(e);
        return true;
    }
    return false;
}
