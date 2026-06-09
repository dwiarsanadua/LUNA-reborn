#pragma once
#include <ui/Window.hpp>
#include <ui/WindowManager.hpp>
#include <ui/GameState.hpp>
#include <ui/widgets/Button.hpp>
#include <ui/widgets/Label.hpp>
#include <ui/widgets/InputField.hpp>
#include <functional>
#include <vector>
#include <cstdint>

class FamilyMarkDialog {
public:
    Window* GetWindow() { return window_; }
    void Open(WindowManager* wm = nullptr);
    void Close();
    void RenderMarkPreview(UIRenderer& ui, float x, float y, float size);

    void SetNetworkCallbacks(
        std::function<void(uint32_t, const std::string&)> on_register_mark,
        std::function<void(uint32_t, const std::string&)> on_register_union_mark);

    void ShowFamilyMark();
    void ShowFamilyUnionMark();

private:
    Window* window_ = nullptr;
    bool union_mode_ = false;

    // Emblem data
    int mark_w_ = 16;
    int mark_h_ = 12;
    std::vector<uint8_t> mark_rgba_;

    // Color/pattern state
    int selected_color_ = 1; // index into palette_
    static const int NUM_COLORS = 16;
    static const uint32_t palette_[NUM_COLORS];

    // Widgets
    InputField* name_input_ = nullptr;
    Button* btn_register_ = nullptr;
    Button* btn_union_register_ = nullptr;
    Label* info_text_ = nullptr;
    Label* mode_label_ = nullptr;

    // Callbacks
    std::function<void(uint32_t, const std::string&)> on_register_;
    std::function<void(uint32_t, const std::string&)> on_union_register_;

    void GenerateDefaultMark();
    void SelectColor(int index);
    void ApplyPreset(int index);
    void OnRegister();
    std::string EncodeBase64() const;
};
