#pragma once
#include <cstdint>
#include <string>
#include <functional>

class UIRenderer;

class RevivalDialog {
public:
    RevivalDialog();
    ~RevivalDialog();

    void Open();
    void Close();
    bool IsOpen() const { return open_; }

    void SetCanReviveHere(bool can);
    void SetCanReviveTown(bool can);
    bool CanReviveHere() const { return can_revive_here_; }
    bool CanReviveTown() const { return can_revive_town_; }

    void SetLevel(int level) { player_level_ = level; }

    void SetOnReviveHere(std::function<void()> cb) { on_revive_here_ = std::move(cb); }
    void SetOnReviveTown(std::function<void()> cb) { on_revive_town_ = std::move(cb); }
    void SetOnClose(std::function<void()> cb) { on_close_ = std::move(cb); }

    void Render(UIRenderer& ui);
    bool HandleClick(float mx, float my);

private:
    bool open_ = false;
    bool can_revive_here_ = true;
    bool can_revive_town_ = true;
    int player_level_ = 1;
    bool show_confirm_ = false;
    int confirm_target_ = 0;

    float x_ = 0.0f;
    float y_ = 0.0f;
    float width_ = 300.0f;
    float height_ = 210.0f;

    std::function<void()> on_revive_here_;
    std::function<void()> on_revive_town_;
    std::function<void()> on_close_;

    void RenderConfirm(UIRenderer& ui);
    bool HandleConfirmClick(float mx, float my);
};

extern RevivalDialog* g_revival_dlg;
