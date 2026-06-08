#include "LoginScreen.hpp"
#include <ui/GameState.hpp>
#include <Character_generated.h>
#include <network/NetworkClient.hpp>
#include <flatbuffers/flatbuffers.h>
#include <Login_generated.h>
#include <PacketType_generated.h>
#include <spdlog/spdlog.h>
#include <bgfx/bgfx.h>
#include <stb_image.h>

void LoginScreen::Init(GameState* state, NetworkClient* network) {
    state_ = state;
    network_ = network;
}

void LoginScreen::Enter() {
    spdlog::info("Entering Login Screen");
}

void LoginScreen::Exit() {
    spdlog::info("Exiting Login Screen");
}

bool LoginScreen::HandlePacket(uint16_t type, const std::vector<uint8_t>& payload) {
    if (type != luna::protocol::PacketType_MP_USERCONN_LOGIN_ACK) return false;
    
    auto resp = flatbuffers::GetRoot<luna::protocol::LoginResponse>(payload.data());
    if (resp->result() == luna::protocol::LoginResult_Success) {
        state_->login_ok = true;
        if (resp->session_token()) state_->session_token = resp->session_token()->str();
        flatbuffers::FlatBufferBuilder fbb;
        auto req = luna::protocol::CreateCharacterListRequestDirect(fbb, state_->session_token.c_str());
        fbb.Finish(req);
        network_->SendPacket(luna::protocol::PacketType_MP_USERCONN_CHARACTERLIST_SYN, fbb.GetBufferPointer(), fbb.GetSize());
        // ScreenManager will switch based on state change
    } else {
        state_->login_error = "Invalid credentials";
        sent_ = false;
    }
    return true;
}

void LoginScreen::Update(float dt) {
    (void)dt;
}

void LoginScreen::Render(UIRenderer& ui) {
    float lw = ui.logicalWidth;
    float lh = ui.logicalHeight;

    // Cache login textures across frames (static so they load once)
    static bgfx::TextureHandle s_bg = BGFX_INVALID_HANDLE;
    static bgfx::TextureHandle s_bar = BGFX_INVALID_HANDLE;
    static bgfx::TextureHandle s_btn = BGFX_INVALID_HANDLE;
    
    auto LoadOnce = [&](bgfx::TextureHandle& cache, const std::string& name) {
        if (bgfx::isValid(cache)) return cache;
        std::string paths[] = {
            "assets/textures/ui/Launcher/" + name,
            "assets/textures/ui/" + name,
            "assets/textures/" + name,
            "assets/textures/unpacked/image/" + name,
            "assets/textures/unpacked/map/" + name,
        };
        for (auto& p : paths) {
            int w, h, n;
            unsigned char* d = stbi_load(p.c_str(), &w, &h, &n, 4);
            if (d) {
                cache = bgfx::createTexture2D((uint16_t)w, (uint16_t)h, false, 1,
                    bgfx::TextureFormat::RGBA8, BGFX_SAMPLER_NONE,
                    bgfx::copy(d, w * h * 4));
                stbi_image_free(d);
                spdlog::info("LoginScreen: cached {}", p);
                return cache;
            }
        }
        return cache;
    };

    // Load all textures once (cached statically)
    LoadOnce(s_bg, "Launcher_01_01.png");
    if (!bgfx::isValid(s_bg)) LoadOnce(s_bg, "login.png");
    LoadOnce(s_bar, "login_bar00.png");
    LoadOnce(s_btn, "login_bar01.png");
    
    // Background
    if (bgfx::isValid(s_bg)) ui.DrawImage(0, 0, lw, lh, s_bg);
    
    // Bottom bar
    if (bgfx::isValid(s_bar)) ui.DrawImage(0, lh - 120.0f, lw, 120, s_bar);

    // Title
    ui.DrawTextCentered(lh * 0.2f, 0xffffcc88, "LUNA Plus Reborn");
    ui.DrawTextCentered(lh * 0.2f + 25.0f, 0xff888888, "v1.1.0 (GitHub Build)");
    
    // Account buttons
    const char* names[3] = {"admin", "test", "demo"};
    for (int i = 0; i < 3; i++) {
        float by = lh * 0.45f + i * 50;
        bool sel = (i == state_->selected_account);
        if (bgfx::isValid(s_btn)) {
            ui.DrawImage(lw * 0.5f - 200, by, 400, 40, s_btn, sel ? UIColor{255,255,200,255} : UIColor{200,200,200,255});
        }
        ui.DrawTextCentered(by + 10, sel ? 0xffffffff : 0xffaaaaaa, names[i]);
    }
    
    ui.DrawTextCentered(lh * 0.75f, 0xffffffff, "Press ENTER to Login");
    if (!state_->login_error.empty())
        ui.DrawTextCentered(lh * 0.8f, 0xffff4444, "%s", state_->login_error.c_str());
}

bool LoginScreen::HandleKey(int key, int scancode, int action, int mods) {
    if (action != 1) return false;

    if (key == 257) { // Enter
        if (!sent_) {
            flatbuffers::FlatBufferBuilder fbb;
            const char* unames[] = {"admin", "test", "demo"};
            std::vector<uint8_t> pass_hash = {0,0,0,0}; // Dummy hash for now
            auto req = luna::protocol::CreateLoginRequestDirect(fbb, unames[state_->selected_account], &pass_hash);
            fbb.Finish(req);
            network_->SendPacket(luna::protocol::PacketType_MP_USERCONN_LOGIN_SYN, fbb.GetBufferPointer(), fbb.GetSize());
            sent_ = true;
        }
        return true;
    }
    
    if (key == 264) { // Down
        state_->selected_account = (state_->selected_account + 1) % 3;
        return true;
    }
    if (key == 265) { // Up
        state_->selected_account = (state_->selected_account + 2) % 3;
        return true;
    }

    return false;
}
