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
#include <stb_image.h>

extern ScreenManager* g_screen_mgr;
static constexpr const char* SAVE_ID_KEY = "login.saved_id";

namespace {
    std::vector<uint8_t> sha256(const std::string& s) {
        std::vector<uint8_t> h(32, 0);
        for (size_t i = 0; i < s.size() && i < 32; i++) h[i] = (uint8_t)s[i];
        return h;
    }
}

void LoginScreen::Init(GameState* state, NetworkClient* network) {
    state_ = state; network_ = network;
}

void LoginScreen::Enter() {
    spdlog::info("[LOGIN] Enter");

    // Load background (lunaclient.png)
    if (!bgfx::isValid(tex_bg_)) {
        const char* paths[] = {"assets/textures/ui/Launcher/lunaclient.png","assets/textures/ui/lunaclient.png","assets/textures/lunaclient.png"};
        for (auto p : paths) {
            int w, h, n;
            auto* d = stbi_load(p, &w, &h, &n, 4);
            if (d) {
                tex_bg_ = bgfx::createTexture2D((uint16_t)w, (uint16_t)h, false, 1,
                    bgfx::TextureFormat::RGBA8, BGFX_SAMPLER_NONE, bgfx::copy(d, w*h*4));
                stbi_image_free(d);
                break;
            }
        }
    }

    fields_[0] = {"", 0, true, false, "ID"};
    fields_[1] = {"", 0, false, true, "PW"};

    std::string saved = ConfigManager::GetString(SAVE_ID_KEY, "");
    if (!saved.empty()) {
        strncpy(fields_[0].buf, saved.c_str(), sizeof(fields_[0].buf)-1);
        fields_[0].pos = (int)saved.size();
    }

    if (scene_renderer_) scene_renderer_->SetClearColor(0x223366FF);
    sent_ = false;
}

void LoginScreen::Exit() {}

void LoginScreen::Update(float) {}

void LoginScreen::Render(UIRenderer& ui) {
    float lw = ui.logicalWidth, lh = ui.logicalHeight;

    // Background
    if (bgfx::isValid(tex_bg_))
        ui.DrawImage(0, 0, lw, lh, tex_bg_);
    else
        ui.DrawRect(0, 0, lw, lh, UIColor{20, 20, 40, 255});

    // Semi-transparent login panel
    float bx = 40, by = lh * 0.15f;
    float pw = 280, ph = 170;
    ui.DrawRect(bx, by, pw, ph, UIColor{0, 0, 0, 160});
    ui.DrawBorder(bx, by, pw, ph, UIColor{100, 140, 200, 180});

    bx += 15; by += 15;
    ui.DrawText(bx, by, 0xFFFFCC88, "Account Login");

    // ID field
    by += 35;
    ui.DrawText(bx, by, 0xFFCCCCCC, "ID");
    float fx = bx + 30;
    ui.DrawRect(fx, by-2, 200, 24, fields_[0].act ? UIColor{60,60,100,200} : UIColor{40,40,70,200});
    ui.DrawBorder(fx, by-2, 200, 24, fields_[0].act ? UIColor{150,150,255,200} : UIColor{80,80,120,200});
    if (fields_[0].buf[0])
        ui.DrawText(fx+4, by+1, 0xFFFFFFFF, "%s", fields_[0].buf);
    else
        ui.DrawText(fx+4, by+1, 0xFF888888, "Enter ID");

    // PW field
    by += 32;
    ui.DrawText(bx, by, 0xFFCCCCCC, "PW");
    ui.DrawRect(fx, by-2, 200, 24, fields_[1].act ? UIColor{60,60,100,200} : UIColor{40,40,70,200});
    ui.DrawBorder(fx, by-2, 200, 24, fields_[1].act ? UIColor{150,150,255,200} : UIColor{80,80,120,200});
    if (fields_[1].buf[0]) {
        char m[64]; int l = (int)strlen(fields_[1].buf);
        for (int i = 0; i < l && i < 63; i++) m[i] = '*';
        m[l] = 0;
        ui.DrawText(fx+4, by+1, 0xFFFFFFFF, "%s", m);
    } else
        ui.DrawText(fx+4, by+1, 0xFF888888, "Enter PW");

    // Login button
    by += 38;
    ui.DrawRect(fx, by, 100, 28, UIColor{50, 90, 140, 220});
    ui.DrawBorder(fx, by, 100, 28, UIColor{100, 150, 200, 180});
    ui.DrawText(fx + 20, by + 4, 0xFFFFFFFF, "LOGIN");

    // Help
    ui.DrawText(bx, by + 42, 0xFF888888, "Tab=Switch  Enter=OK  Offline → CharSelect");
}

bool LoginScreen::HandleKey(int key, int, int action, int) {
    if (action != 1 && key != 259) return false;
    if (key == 257) { DoLogin(); return true; }
    if (key == 258) { // Tab
        fields_[0].act = !fields_[0].act;
        fields_[1].act = !fields_[1].act;
        return true;
    }
    if (key == 259) { // Backspace
        auto& f = fields_[0].act ? fields_[0] : fields_[1];
        int len = (int)strlen(f.buf);
        if (len > 0) { f.buf[len-1] = 0; f.pos = len-1; }
        return true;
    }
    return true;
}

bool LoginScreen::HandleChar(unsigned int cp) {
    auto& f = fields_[0].act ? fields_[0] : fields_[1];
    if (cp >= 32 && cp <= 126) {
        int len = (int)strlen(f.buf);
        if (len < 60) { f.buf[len] = (char)cp; f.buf[len+1] = 0; f.pos = len+1; }
        return true;
    }
    return false;
}

bool LoginScreen::HandlePacket(uint16_t type, const std::vector<uint8_t>& payload) {
    if (type != luna::protocol::PacketType_MP_USERCONN_LOGIN_ACK) return false;
    auto resp = flatbuffers::GetRoot<luna::protocol::LoginResponse>(payload.data());
    if (resp->result() == luna::protocol::LoginResult_Success) {
        spdlog::info("[LOGIN] Server OK → CharSelect");
        if (g_screen_mgr) g_screen_mgr->SwitchTo("charselect");
    }
    return true;
}

bool LoginScreen::DoLogin() {
    if (sent_) return false;
    sent_ = true;
    std::string user = fields_[0].buf, pass = fields_[1].buf;
    if (user.empty() || pass.empty()) { sent_ = false; return false; }
    spdlog::info("[LOGIN] Attempt: user='{}'", user);
    auto hash = sha256(pass);
    flatbuffers::FlatBufferBuilder fbb;
    auto req = luna::protocol::CreateLoginRequestDirect(fbb, user.c_str(), &hash, "1.0.0", 0, 0, "00-00-00-00-00-00");
    fbb.Finish(req);
    if (network_ && network_->IsConnected())
        network_->SendPacket(luna::protocol::PacketType_MP_USERCONN_LOGIN_SYN, fbb.GetBufferPointer(), fbb.GetSize());
    else {
        spdlog::info("[LOGIN] Offline → CharSelect");
        if (g_screen_mgr) g_screen_mgr->SwitchTo("charselect");
    }
    return true;
}
