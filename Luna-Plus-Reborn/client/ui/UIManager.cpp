#include "UIManager.hpp"
#include <spdlog/spdlog.h>
#include <algorithm>
#include <cstring>

void UIManager::Init() {
    spdlog::info("UI Manager initialized");
}

void UIManager::Render() {
    switch (current_screen_) {
        case Screen_Login: RenderLogin(); break;
        case Screen_CharacterSelect: RenderCharacterSelect(); break;
        case Screen_Game: break;
    }
}

void UIManager::Shutdown() {
    spdlog::info("UI Manager shutdown");
}

void UIManager::RenderLogin() {
    ImGui::SetNextWindowSize(ImVec2(400, 300));
    ImGui::SetNextWindowPos(ImVec2(440, 210), ImGuiCond_Once);

    ImGui::Begin("LUNA Plus - Login", nullptr,
                 ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

    ImGui::Text("Welcome to LUNA Plus Reborn");
    ImGui::Separator();
    ImGui::Spacing();

    char user[128] = {};
    std::memcpy(user, login_username.c_str(), std::min(login_username.size(), sizeof(user) - 1));
    if (ImGui::InputText("Username", user, sizeof(user)))
        login_username = user;

    char pass[128] = {};
    std::memcpy(pass, login_password.c_str(), std::min(login_password.size(), sizeof(pass) - 1));
    if (ImGui::InputText("Password", pass, sizeof(pass), ImGuiInputTextFlags_Password))
        login_password = pass;

    ImGui::Spacing();

    if (login_pending) {
        ImGui::Text("Connecting...");
    } else {
        if (ImGui::Button("Login", ImVec2(380, 0))) {
            if (on_login_click && !login_username.empty() && !login_password.empty()) {
                login_pending = true;
                login_status = "Connecting...";
                on_login_click(login_username, login_password);
            }
        }
    }

    if (!login_status.empty()) {
        ImGui::TextColored(ImVec4(1,1,0,1), "%s", login_status.c_str());
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Text("Test accounts: admin, test, demo");

    ImGui::End();
}

void UIManager::RenderCharacterSelect() {
    ImGui::SetNextWindowSize(ImVec2(500, 400));
    ImGui::SetNextWindowPos(ImVec2(390, 160), ImGuiCond_Once);

    ImGui::Begin("Character Selection", nullptr,
                 ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

    ImGui::Text("Select a Character");
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::Text("No characters yet. Create one!");
    ImGui::Spacing();

    if (ImGui::Button("Create New Character", ImVec2(480, 0))) {}
    ImGui::SameLine();
    if (ImGui::Button("Enter World", ImVec2(480, 0))) {
        if (on_start_game) on_start_game();
    }

    ImGui::End();
}
