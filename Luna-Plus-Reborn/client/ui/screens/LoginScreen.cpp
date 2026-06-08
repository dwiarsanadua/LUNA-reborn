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
    textures_loaded_ = false;
}

void LoginScreen::LoadTexture(bgfx::TextureHandle& cache, const std::string& name) {
    if (bgfx::isValid(cache)) return;
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
            spdlog::info("LoginScreen: loaded {}", p);
            if (bgfx::isValid(cache)) {
                spdlog::info("LoginScreen: {} texture handle valid ({}x{})", name, w, h);
            } else {
                spdlog::warn("LoginScreen: {} texture create FAILED after loading", name);
            }
            return;
        }
    }
    spdlog::warn("LoginScreen: could not find {}", name);
}

void LoginScreen::Enter() {
    spdlog::info("Entering Login Screen");
    if (!textures_loaded_) {
        LoadTexture(tex_bg_, "Launcher_01_01.png");
        if (!bgfx::isValid(tex_bg_)) LoadTexture(tex_bg_, "login.png");
        LoadTexture(tex_bar_, "login_bar00.png");
        LoadTexture(tex_btn_, "login_bar01.png");
        bg_ok_ = bgfx::isValid(tex_bg_);
        textures_loaded_ = true;
        // Set clear color to match Luna Plus login theme
        if (scene_renderer_) scene_renderer_->SetClearColor(0x887766FF);
    }
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

    // Ensure textures are loaded (safety check if Enter() was not called)
    if (!textures_loaded_) {
        TexturesLoadOnce();
    }

    // Draw background (triangular fade pattern like Luna Plus Old)
    if (bg_ok_ && bgfx::isValid(tex_bg_)) {
        ui.DrawImage(0, 0, lw, lh, tex_bg_);
    } else {
        // Fallback: gradient background instead of magenta
        ui.DrawRect(0, 0, lw, lh, UIColor{80, 50, 30, 255});
    }
    
    // Bottom bar
    if (bgfx::isValid(tex_bar_)) ui.DrawImage(0, lh - 120.0f, lw, 120, tex_bar_);

    // Title
    ui.DrawTextCentered(lh * 0.2f, 0xffffcc88, "LUNA Plus Reborn");
    ui.DrawTextCentered(lh * 0.2f + 25.0f, 0xff888888, "v1.1.0 (GitHub Build)");
    
    // Account buttons
    const char* names[3] = {"admin", "test", "demo"};
    for (int i = 0; i < 3; i++) {
        float by = lh * 0.45f + i * 50;
        bool sel = (i == state_->selected_account);
        if (bgfx::isValid(tex_btn_)) {
            ui.DrawImage(lw * 0.5f - 200, by, 400, 40, tex_btn_, sel ? UIColor{255,255,200,255} : UIColor{200,200,200,255});
        }
        ui.DrawTextCentered(by + 10, sel ? 0xffffffff : 0xffaaaaaa, names[i]);
    }
    
    ui.DrawTextCentered(lh * 0.75f, 0xffffffff, "Press ENTER to Login");
    if (!state_->login_error.empty())
        ui.DrawTextCentered(lh * 0.8f, 0xffff4444, "%s", state_->login_error.c_str());
}

void LoginScreen::TexturesLoadOnce() {
    LoadTexture(tex_bg_, "Launcher_01_01.png");
    if (!bgfx::isValid(tex_bg_)) LoadTexture(tex_bg_, "login.png");
    LoadTexture(tex_bar_, "login_bar00.png");
    LoadTexture(tex_btn_, "login_bar01.png");
    bg_ok_ = bgfx::isValid(tex_bg_);
    textures_loaded_ = true;
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
