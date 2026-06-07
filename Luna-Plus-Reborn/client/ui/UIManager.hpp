#pragma once
#include <string>
#include <functional>
#include <imgui.h>

class UIManager {
public:
    enum Screen { Screen_Login, Screen_CharacterSelect, Screen_Game };

    void Init();
    void Render();
    void Shutdown();
    void SwitchTo(Screen screen) { current_screen_ = screen; }
    Screen Current() const { return current_screen_; }

    std::string login_username;
    std::string login_password;
    std::string login_status;
    bool login_pending = false;

    std::function<void(const std::string&, const std::string&)> on_login_click;
    std::function<void()> on_start_game;

private:
    Screen current_screen_ = Screen_Login;
    void RenderLogin();
    void RenderCharacterSelect();
};
