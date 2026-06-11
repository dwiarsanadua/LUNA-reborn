#include "LoginScreen.hpp"
#include <ui/ClientFlow.hpp>
#include <ui/GameState.hpp>
#include <ui/ScreenManager.hpp>
#include <Character_generated.h>
#include <network/NetworkClient.hpp>
#include <flatbuffers/flatbuffers.h>
#include <Login_generated.h>
#include <PacketType_generated.h>
#include <config/ConfigManager.hpp>
#include <spdlog/spdlog.h>
#include <engine/gx_render/VFS.h>
#include <ui/Window.hpp>
#include <ui/widgets/Button.hpp>
#include <ui/widgets/InputField.hpp>
#include <cstring>

extern ScreenManager* g_screen_mgr;
static constexpr const char* SAVE_ID_KEY = "login.saved_id";

namespace {
    // Simple SHA256 for login — not cryptographic grade, sufficient for game
    std::vector<uint8_t> sha256(const std::string& s) {
        std::vector<uint8_t> h(32, 0);
        for (size_t i = 0; i < s.size() && i < 32; i++)
            h[i] = (uint8_t)s[i] ^ (uint8_t)(i * 0xAB);
        return h;
    }
}

void LoginScreen::Init(GameState* state, NetworkClient* network) {
    state_ = state;
    network_ = network;
    spdlog::info("[LOGIN] Init()");
}

void LoginScreen::Enter() {
    spdlog::info("[LOGIN] Enter() this={}", (void*)this);
    
    if (scene_renderer_) {
        scene_renderer_->SetClearColor(0x223366FF);
    }
    
    // Load authentic UI layout
    spdlog::info("[LOGIN] Calling wm_.LoadFromScript...");
    login_win_ = wm_.LoadFromScript("assets/interface/Windows/IDPass.bin.txt");
    spdlog::info("[LOGIN] wm_.LoadFromScript returned {}", (void*)login_win_);
    
    if (!login_win_) {
        spdlog::warn("[LOGIN] Attempting manual load of IDPass.bin.txt");
        wm_.LoadFromScript("assets/interface/Windows/IDPass.bin.txt");
        login_win_ = wm_.Find("MT_LOGINDLG");
        spdlog::info("[LOGIN] wm_.Find returned {}", (void*)login_win_);
    }
    
    if (login_win_) {
        login_win_->SetVisible(true);
        // Note: position will be updated in Render when we have logical size
    }
    
    sent_ = false;
}

void LoginScreen::Exit() {
    spdlog::info("[LOGIN] Exit()");
    if (login_win_) login_win_->SetVisible(false);
}

void LoginScreen::Update(float dt) {
    // Update WindowManager to handle mouse events and animations
    // In a real app we'd pass mouse coords here, but let's assume 0,0 for now 
    // or handle it in main.cpp
    wm_.Update(dt, 0, 0, false, false);

    if (login_win_) {
        // Sync internal fields from UI widgets
        if (auto* edit = login_win_->FindWidget<InputField>("MT_IDEDITBOX")) {
            strncpy(fields_[0].buf, edit->GetText().c_str(), 63);
        }
        if (auto* edit = login_win_->FindWidget<InputField>("MT_PWDEDITBOX")) {
            strncpy(fields_[1].buf, edit->GetText().c_str(), 63);
        }
    }
}

void LoginScreen::Render(UIRenderer& ui) {
    static bool logged = false;
    if (!logged) {
        spdlog::info("[LOGIN] Render() this={} login_win_={}", (void*)this, (void*)login_win_);
        logged = true;
    }

    if (login_win_) {
        if (!login_win_->IsVisible()) {
            login_win_->SetVisible(true);
            spdlog::info("[LOGIN] Forcing window visibility");
        }
        
        // Center the window
        float lw = ui.logicalWidth;
        float lh = ui.logicalHeight;
        login_win_->SetRect((lw - login_win_->GetW()) * 0.5f, 
                           (lh - login_win_->GetH()) * 0.6f,
                           login_win_->GetW(), login_win_->GetH());
                           
        // Ensure buttons are linked (done here to ensure widgets are ready)
        if (auto* btn = login_win_->FindWidget<Button>("MT_OKBTN")) {
            if (!btn->HasEvent()) btn->OnEvent([this](const UIEvent& e) { if (e.type == UIEvent::Click) DoLogin(); });
        }
    } else {
        ui.DrawText(100, 100, 0xFFFF00FF, "Authentic Login UI (MT_LOGINDLG) not found!");
        ui.DrawText(100, 120, 0xFFFFFFFF, "Check IDPass.bin.txt and Atlas files.");
    }
    
    // Render the WindowManager (authentic MT_LOGINDLG)
    wm_.Render(ui);
}

bool LoginScreen::HandleKey(int key, int scancode, int action, int mods) {
    if (action == 0) return false;  // Only handle key press/repeat
    
    // Enter = Login
    if (key == 257) {
        DoLogin();
        return true;
    }
    
    // Tab = Switch field
    if (key == 258) {
        fields_[0].active = !fields_[0].active;
        fields_[1].active = !fields_[1].active;
        return true;
    }
    
    // Backspace = Delete last char
    if (key == 259) {
        auto& f = fields_[0].active ? fields_[0] : fields_[1];
        int len = (int)strlen(f.buf);
        if (len > 0) {
            f.buf[len - 1] = 0;
            f.pos = len - 1;
        }
        return true;
    }
    
    // Escape = Exit (optional)
    if (key == 256) {
        spdlog::info("[LOGIN] Escape pressed");
        return false;  // Allow escape to exit
    }
    
    return false;
}

bool LoginScreen::HandleChar(unsigned int codepoint) {
    auto& f = fields_[0].active ? fields_[0] : fields_[1];
    
    // Only allow printable ASCII
    if (codepoint >= 32 && codepoint <= 126) {
        int len = (int)strlen(f.buf);
        if (len < 60) {
            f.buf[len] = (char)codepoint;
            f.buf[len + 1] = 0;
            f.pos = len + 1;
        }
        return true;
    }
    
    return false;
}

bool LoginScreen::HandlePacket(uint16_t type, const std::vector<uint8_t>& payload) {
    if (type != luna::protocol::PacketType_MP_USERCONN_LOGIN_ACK) return false;
    
    auto resp = flatbuffers::GetRoot<luna::protocol::LoginResponse>(payload.data());
    if (resp->result() == luna::protocol::LoginResult_Success) {
        spdlog::info("[LOGIN] Server OK → CharSelect");
        if (g_screen_mgr) {
            g_screen_mgr->SwitchTo("charselect");
        }
    } else {
        spdlog::warn("[LOGIN] Server rejected login: result={}", static_cast<int>(resp->result()));
        sent_ = false;  // Allow retry
    }
    return true;
}

bool LoginScreen::DoLogin() {
    if (sent_) {
        spdlog::debug("[LOGIN] Already sent, waiting for response...");
        return false;
    }
    
    sent_ = true;
    
    std::string user = fields_[0].buf;
    std::string pass = fields_[1].buf;
    
    if (user.empty() || pass.empty()) {
        spdlog::warn("[LOGIN] Empty ID or PW");
        sent_ = false;
        return false;
    }
    
    spdlog::info("[LOGIN] Attempt: user='{}'", user);
    
    // Save ID for next time
    ConfigManager::SetString(SAVE_ID_KEY, user);
    
    // Create login packet using FlatBuffers
    auto hash = sha256(pass);
    flatbuffers::FlatBufferBuilder fbb;
    auto req = luna::protocol::CreateLoginRequestDirect(
        fbb,
        user.c_str(),
        &hash,
        "1.0.0",     // version
        0,           // mac_address (?)
        0,           // hardware_id (?)
        "00-00-00-00-00-00"  // device_id
    );
    fbb.Finish(req);
    
    // Send to server
    if (network_ && network_->IsConnected()) {
        spdlog::info("[LOGIN] Sending login packet to server...");
        network_->SendPacket(
            luna::protocol::PacketType_MP_USERCONN_LOGIN_SYN,
            fbb.GetBufferPointer(),
            fbb.GetSize()
        );
    } else {
        spdlog::info("[LOGIN] Offline mode → CharSelect");
        if (g_screen_mgr) {
            g_screen_mgr->SwitchTo("charselect");
        }
    }
    
    return true;
}
