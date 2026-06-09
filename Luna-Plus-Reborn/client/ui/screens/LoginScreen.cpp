#include "LoginScreen.hpp"
#include <ui/ClientFlow.hpp>
#include <ui/GameState.hpp>
#include <ui/ScreenManager.hpp>
#include <Character_generated.h>
#include <network/NetworkClient.hpp>
#include <flatbuffers/flatbuffers.h>
#include <Login_generated.h>
#include <PacketType_generated.h>
#include <spdlog/spdlog.h>
#include <bgfx/bgfx.h>
#include <stb_image.h>
#include <cstring>

extern ScreenManager* g_screen_mgr;

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
    TexturesLoadOnce();
    id_field_.label = "Account";
    id_field_.active = true;
    id_field_.masked = false;
    pw_field_.label = "Password";
    pw_field_.active = false;
    pw_field_.masked = true;
    error_message_.clear();
    error_timer_ = 0;
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
    } else {
        state_->login_error = "Invalid credentials";
        sent_ = false;
    }
    return true;
}

void LoginScreen::Update(float dt) {
    if (error_timer_ > 0) error_timer_ -= dt;
}

bool LoginScreen::DoLogin() {
    if (strlen(id_field_.buffer) == 0) {
        error_message_ = "Please enter account ID";
        error_timer_ = 3.0f;
        return true;
    }
    if (strlen(pw_field_.buffer) == 0) {
        error_message_ = "Please enter password";
        error_timer_ = 3.0f;
        return true;
    }

    if (!sent_) {
        if (!network_->IsConnected()) {
            if (!network_->Connect("127.0.0.1", 8100)) {
                error_message_ = "Server unavailable";
                error_timer_ = 3.0f;
                return true;
            }
            state_->current_state = ClientState::Connect;
        }
        flatbuffers::FlatBufferBuilder fbb;
        auto req = luna::protocol::CreateLoginRequestDirect(fbb, id_field_.buffer, {});
        fbb.Finish(req);
        network_->SendPacket(luna::protocol::PacketType_MP_USERCONN_LOGIN_SYN,
                             fbb.GetBufferPointer(), fbb.GetSize());
        sent_ = true;
        state_->current_state = ClientState::Title;
    }
    return true;
}

bool LoginScreen::HandleChar(unsigned int codepoint) {
    if (codepoint < 32 || codepoint > 126) return false;
    InputField* f = id_field_.active ? &id_field_ : &pw_field_;
    if (f->cursor_pos < 63) {
        f->buffer[f->cursor_pos++] = (char)codepoint;
        f->buffer[f->cursor_pos] = '\0';
    }
    return true;
}

void LoginScreen::DrawField(UIRenderer& ui, const InputField& field, bool focus) {
    (void)ui;
    (void)field;
    (void)focus;
}

void LoginScreen::Render(UIRenderer& ui) {
    float lw = ui.logicalWidth;
    float lh = ui.logicalHeight;

    if (!textures_loaded_) {
        TexturesLoadOnce();
    }

    if (bgfx::isValid(tex_bg_))
        ui.DrawImage(0, 0, lw, lh, tex_bg_);
    else
        ui.DrawRect(0, 0, lw, lh, UIColor{20, 20, 40, 255});

    float wx = lw * 0.5f - 200;
    float wy = lh * 0.3f;

    ui.DrawRect(wx, wy, 400, 280, UIColor{30, 30, 50, 220});
    ui.DrawBorder(wx, wy, 400, 280, UIColor{100, 120, 180, 200});
    ui.DrawText(wx + 10, wy + 8, 0xFFFFCC88, "Account Login");

    float fy = wy + 50;
    ui.DrawText(wx + 20, fy, 0xFFCCCCCC, "%s", id_field_.label.c_str());
    ui.DrawRect(wx + 120, fy - 2, id_field_.w, id_field_.h,
                id_field_.active ? UIColor{60, 60, 100, 255} : UIColor{40, 40, 70, 255});
    ui.DrawBorder(wx + 120, fy - 2, id_field_.w, id_field_.h,
                  id_field_.active ? UIColor{150, 150, 255, 255} : UIColor{80, 80, 120, 255});
    if (strlen(id_field_.buffer) > 0)
        ui.DrawText(wx + 125, fy + 2, 0xFFFFFFFF, "%s", id_field_.buffer);
    else
        ui.DrawText(wx + 125, fy + 2, 0xFF666666, "Enter account ID");

    fy = wy + 100;
    ui.DrawText(wx + 20, fy, 0xFFCCCCCC, "%s", pw_field_.label.c_str());
    ui.DrawRect(wx + 120, fy - 2, pw_field_.w, pw_field_.h,
                pw_field_.active ? UIColor{60, 60, 100, 255} : UIColor{40, 40, 70, 255});
    ui.DrawBorder(wx + 120, fy - 2, pw_field_.w, pw_field_.h,
                  pw_field_.active ? UIColor{150, 150, 255, 255} : UIColor{80, 80, 120, 255});
    if (strlen(pw_field_.buffer) > 0) {
        char masked[64]; int len = (int)strlen(pw_field_.buffer);
        for (int i = 0; i < len && i < 63; i++) masked[i] = '*';
        masked[len] = '\0';
        ui.DrawText(wx + 125, fy + 2, 0xFFFFFFFF, "%s", masked);
    } else {
        ui.DrawText(wx + 125, fy + 2, 0xFF666666, "Enter password");
    }

    ui.DrawButton(wx + 50, wy + 170, 130, 35, "OK", false);
    ui.DrawButton(wx + 220, wy + 170, 130, 35, "Cancel", false);

    if (error_timer_ > 0 && !error_message_.empty()) {
        ui.DrawText(wx + 20, wy + 230, 0xFFFF4444, "%s", error_message_.c_str());
    }

    if (bgfx::isValid(tex_bar_))
        ui.DrawImage(0, lh - 120, lw, 120, tex_bar_);

    ui.DrawTextCentered(lh * 0.82f, 0xFF888888, "Tab=Switch Field  Enter=Login  Esc=Back");
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

    if (key == 258) {
        id_field_.active = !id_field_.active;
        pw_field_.active = !pw_field_.active;
        return true;
    }

    if (key == 256) {
        if (g_screen_mgr) g_screen_mgr->SwitchTo("launcher");
        return true;
    }

    if (key == 257) {
        return DoLogin();
    }

    if (key == 259) {
        InputField* f = id_field_.active ? &id_field_ : &pw_field_;
        if (f->cursor_pos > 0) f->buffer[--f->cursor_pos] = '\0';
        return true;
    }

    return false;
}
