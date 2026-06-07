#pragma once
#include "Widget.hpp"
#include <string>
#include <functional>

enum class InputValidation {
    None,
    NumericOnly,
    Alphanumeric,
    PositiveInteger,
    Decimal
};

class InputField : public Widget {
public:
    InputField(float x = 0, float y = 0, float w = 200, float h = 22);
    const std::string& GetText() const { return text_; }
    void SetText(const std::string& t);
    void SetMaxLength(int m) { max_len_ = m; }
    void SetPlaceholder(const std::string& p) { placeholder_ = p; }
    bool IsFocused() const { return focused_; }
    void SetFocused(bool f) { focused_ = f; }
    void SetValidation(InputValidation v) { validation_ = v; }
    void SetCustomValidator(std::function<bool(const std::string&)> fn) { custom_validator_ = fn; }
    void SetNumericRange(int min, int max) { num_min_ = min; num_max_ = max; }
    bool IsValid() const { return valid_; }
    void Update(float dt, float mx, float my, bool mousedown, bool mousepressed) override;
    void Render(UIRenderer& ui) override;
    bool HandleKey(int key) override;
    bool HandleChar(unsigned int codepoint) override;
private:
    std::string text_;
    std::string placeholder_;
    int max_len_ = 64;
    bool focused_ = false;
    float cursor_timer_ = 0;
    bool cursor_show_ = true;
    InputValidation validation_ = InputValidation::None;
    std::function<bool(const std::string&)> custom_validator_;
    int num_min_ = 0;
    int num_max_ = 999999;
    bool valid_ = true;
    bool Validate(const std::string& s);
};
