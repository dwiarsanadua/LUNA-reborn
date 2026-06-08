#pragma once
#include <ui/Window.hpp>
#include <ui/WindowManager.hpp>
#include <rendering/UIRenderer.hpp>
#include <string>
#include <vector>
#include <cstdint>

struct HelperBuff {
    std::string name;
    std::string icon;
    float duration;
    float remaining;
    uint32_t color;
};

class HelperDlg {
public:
    HelperDlg();

    void Open(WindowManager* wm);
    void Close();
    Window* GetWindow() { return window_; }
    bool IsOpen() const { return window_ != nullptr; }

    void SetCompanionName(const std::string& name);
    void SetCompanionLevel(int level);
    void SetCompanionHP(int hp, int max_hp);
    void SetCompanionEXP(int exp, int next_exp);
    void SetCompanionMood(float mood); // 0.0 - 1.0

    void AddBuff(const HelperBuff& buff);
    void ClearBuffs();
    void Feed();
    void Interact();

    void Update(float dt);
    void Render(UIRenderer& ui);

private:
    Window* window_ = nullptr;
    std::string companion_name_ = "Helper";
    int companion_level_ = 1;
    int companion_hp_ = 100, companion_max_hp_ = 100;
    int companion_exp_ = 0, companion_exp_next_ = 200;
    float companion_mood_ = 0.8f;
    std::vector<HelperBuff> buffs_;

    bool auto_feed_ = false;
    float hunger_ = 1.0f;
};
