#pragma once
#include <ui/Window.hpp>
#include <ui/WindowManager.hpp>
#include <ui/GameState.hpp>
#include <ui/widgets/Label.hpp>
#include <ui/widgets/Button.hpp>
#include <ui/widgets/TabPanel.hpp>
#include <ui/widgets/InputField.hpp>
#include <gameobjects/FamilySystem.hpp>
#include <functional>

class FamilyDialog {
public:
    Window* GetWindow() { return window_; }
    void Open(GameState* state, WindowManager* wm, FamilySystem* family);
    void Close() { window_ = nullptr; }
    void UpdateFromState(GameState* state);
    void SetNetworkCallbacks(std::function<void(const std::string&)> create_fn,
                             std::function<void(uint32_t, const std::string&)> propose_fn,
                             std::function<void()> accept_fn,
                             std::function<void()> reject_fn,
                             std::function<void()> divorce_fn,
                             std::function<void()> leave_fn,
                             std::function<void()> refresh_fn);
    void SetInviteCallback(std::function<void(const std::string&)> invite_fn) { invite_fn_ = std::move(invite_fn); }

private:
    Window* window_ = nullptr;
    TabPanel* tabs_ = nullptr;
    Label* tab1_label_ = nullptr;
    Label* tab2_label_ = nullptr;
    Label* tab3_label_ = nullptr;
    InputField* family_name_input_ = nullptr;
    InputField* partner_id_input_ = nullptr;
    InputField* partner_name_input_ = nullptr;
    InputField* invite_name_input_ = nullptr;
    FamilySystem* family_ = nullptr;
    std::function<void(const std::string&)> create_fn_;
    std::function<void(uint32_t, const std::string&)> propose_fn_;
    std::function<void()> accept_fn_;
    std::function<void()> reject_fn_;
    std::function<void()> divorce_fn_;
    std::function<void()> leave_fn_;
    std::function<void()> refresh_fn_;
    std::function<void(const std::string&)> invite_fn_;
    void Refresh(GameState* state);
    static const char* RelationLabel(uint8_t relation);
    void DoInvite(GameState* state);
};
