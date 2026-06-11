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
#include <cmath>

extern ScreenManager* g_screen_mgr;
static constexpr const char* SAVE_ID_KEY = "login.saved_id";

namespace {
    std::vector<uint8_t> sha256(const std::string& s) {
        std::vector<uint8_t> h(32, 0);
        for (size_t i = 0; i < s.size() && i < 32; i++) h[i] = (uint8_t)s[i];
        return h;
    }

    // Draw an Old-Luna-style raised bevel panel
    void DrawLunaPanel(UIRenderer& ui, float x, float y, float w, float h) {
        // Main dark body
        ui.DrawRect(x, y, w, h, UIColor{10, 12, 28, 230});
        // Top+left highlight
        ui.DrawRect(x, y, w, 1, UIColor{160, 190, 255, 80});
        ui.DrawRect(x, y, 1, h, UIColor{160, 190, 255, 80});
        // Bottom+right shadow
        ui.DrawRect(x, y + h - 1, w, 1, UIColor{0, 0, 0, 180});
        ui.DrawRect(x + w - 1, y, 1, h, UIColor{0, 0, 0, 180});
    }

    // Draw an Old-Luna-style blue gradient title bar (4-step)
    void DrawLunaTitleBar(UIRenderer& ui, float x, float y, float w, float th) {
        float step = th / 4.0f;
        UIColor cols[4] = {
            {60,  90, 180, 240},
            {42,  66, 148, 240},
            {30,  48, 120, 240},
            {18,  28,  80, 240},
        };
        for (int i = 0; i < 4; ++i)
            ui.DrawRect(x, y + step * i, w, step + 1.0f, cols[i]);
        // Gold separator line at bottom of title bar
        ui.DrawRect(x, y + th - 1, w, 1, UIColor{212, 175, 96, 220});
    }

    // Input field background + border
    void DrawInputField(UIRenderer& ui, float x, float y, float w, float h, bool active) {
        ui.DrawRect(x, y, w, h, active ? UIColor{32, 40, 80, 220} : UIColor{15, 18, 40, 210});
        ui.DrawBorder(x, y, w, h,
            active ? UIColor{120, 160, 255, 200} : UIColor{60, 80, 140, 160});
        // Gloss on top half
        ui.DrawRect(x + 1, y + 1, w - 2, h * 0.4f,
            active ? UIColor{255, 255, 255, 25} : UIColor{255, 255, 255, 12});
    }

    // Luna-style button
    void DrawLunaButton(UIRenderer& ui, float x, float y, float w, float h,
                        const char* text, bool hover, bool pressed = false) {
        UIColor base = hover ? UIColor{60, 100, 190, 240} : UIColor{40, 70, 150, 230};
        if (pressed) base = UIColor{25, 45, 100, 240};
        ui.DrawRect(x, y, w, h, base);
        if (!pressed) {
            // Top gloss
            ui.DrawRect(x + 1, y + 1, w - 2, h * 0.45f, UIColor{255, 255, 255, hover ? uint8_t(50) : uint8_t(30)});
        }
        // Outer dark border + inner blue bevel
        ui.DrawBorder(x, y, w, h, UIColor{10, 14, 30, 220});
        ui.DrawBorder(x + 1, y + 1, w - 2, h - 2,
            hover ? UIColor{160, 200, 255, 180} : UIColor{100, 140, 220, 130});
        // Text with shadow
        float tw = ui.MeasureText(text);
        float tx = x + (w - tw) * 0.5f;
        float ty = y + (h - 16.0f) * 0.5f + (pressed ? 1.0f : 0.0f);
        ui.DrawText(tx + 1, ty + 1, 0x88000020, "%s", text);
        ui.DrawText(tx, ty, 0xFFFFFFFF, "%s", text);
    }
}

void LoginScreen::Init(GameState* state, NetworkClient* network) {
    state_ = state; network_ = network;
}

void LoginScreen::Enter() {
    spdlog::info("[LOGIN] Enter");

    // Load background — original Luna Plus used a full-screen 2D image
    if (!bgfx::isValid(tex_bg_)) {
        const char* paths[] = {
            "assets/textures/ui/Launcher/lunaclient.png",
            "assets/textures/ui/lunaclient.png",
            "assets/textures/lunaclient.png",
            "assets/textures/ui/bg_login.png",
        };
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
        strncpy(fields_[0].buf, saved.c_str(), sizeof(fields_[0].buf) - 1);
        fields_[0].pos = (int)saved.size();
    }

    if (scene_renderer_) scene_renderer_->SetClearColor(0x0A0A14FF);
    sent_ = false;
    anim_time_ = 0.0f;
}

void LoginScreen::Exit() {}

void LoginScreen::Update(float dt) {
    anim_time_ += dt;
}

void LoginScreen::Render(UIRenderer& ui) {
    float lw = ui.logicalWidth, lh = ui.logicalHeight;

    // ── Background ────────────────────────────────────────────────────────────
    if (bgfx::isValid(tex_bg_)) {
        ui.DrawImage(0, 0, lw, lh, tex_bg_);
        // Darken overlay so panel is readable
        ui.DrawRect(0, 0, lw, lh, UIColor{0, 0, 0, 100});
    } else {
        // Procedural dark gradient — top dark navy → bottom near-black
        ui.DrawRect(0, 0, lw, lh * 0.55f, UIColor{14, 18, 48, 255});
        ui.DrawRect(0, lh * 0.55f, lw, lh * 0.45f, UIColor{6, 8, 20, 255});
        // Subtle vignette edges
        ui.DrawRect(0, 0, 80, lh, UIColor{0, 0, 0, 60});
        ui.DrawRect(lw - 80, 0, 80, lh, UIColor{0, 0, 0, 60});
    }

    // ── Bottom decorative bar (like original Luna) ────────────────────────────
    float barH = 38.0f;
    ui.DrawRect(0, lh - barH, lw, barH, UIColor{8, 12, 36, 220});
    ui.DrawRect(0, lh - barH, lw, 1, UIColor{212, 175, 96, 160});   // gold top edge
    ui.DrawRect(0, lh - barH + 1, lw, 1, UIColor{60, 80, 160, 80}); // blue accent
    // Version text in bar
    ui.DrawText(lw - 140, lh - barH + 11, 0xFF888888, "LUNA Plus Reborn v1.1");

    // ── Logo / title text (bottom-right, like original) ───────────────────────
    float logo_alpha = (uint8_t)std::min(255.0f, anim_time_ * 280.0f);
    uint32_t logo_col = (uint32_t)(logo_alpha) << 24 | 0x00EED090;
    ui.DrawText(lw - 340, lh - barH - 54, logo_col, "LUNA Plus Reborn");
    ui.DrawText(lw - 300, lh - barH - 30, 0xAA9988AA, "Powered by bgfx + Metal");

    // ── Login panel — centered ─────────────────────────────────────────────────
    float pw = 310.0f, ph = 220.0f;
    float px = (lw - pw) * 0.5f;
    float py = (lh - ph) * 0.5f - 20.0f; // slight offset upward

    // Shadow behind panel
    ui.DrawRect(px + 4, py + 4, pw, ph, UIColor{0, 0, 0, 100});

    // Panel body
    DrawLunaPanel(ui, px, py, pw, ph);
    // Outer bevel
    ui.DrawBorder(px - 1, py - 1, pw + 2, ph + 2, UIColor{8, 10, 24, 220});
    ui.DrawBorder(px, py, pw, ph, UIColor{100, 140, 220, 160});

    // Title bar
    float th = 26.0f;
    DrawLunaTitleBar(ui, px, py, pw, th);

    // Title text with shadow
    float title_x = px + 10.0f, title_y = py + 4.0f;
    ui.DrawText(title_x + 1, title_y + 1, 0x88000000, "Account Login");
    ui.DrawText(title_x, title_y, 0xFFFFE8C0, "Account Login");

    // ── Input fields ─────────────────────────────────────────────────────────
    float row_y = py + th + 20.0f;
    float label_x = px + 16.0f;
    float field_x = px + 66.0f;
    float field_w = pw - 82.0f;
    float field_h = 24.0f;

    // ID row
    ui.DrawText(label_x + 1, row_y + 5, 0x66000000, "ID");
    ui.DrawText(label_x, row_y + 4, 0xFFCCCCCC, "ID");
    DrawInputField(ui, field_x, row_y, field_w, field_h, fields_[0].act);
    if (fields_[0].buf[0])
        ui.DrawText(field_x + 6, row_y + 4, 0xFFFFFFFF, "%s", fields_[0].buf);
    else
        ui.DrawText(field_x + 6, row_y + 4, 0xFF606070, "Enter account ID");

    // PW row
    row_y += 38.0f;
    ui.DrawText(label_x + 1, row_y + 5, 0x66000000, "PW");
    ui.DrawText(label_x, row_y + 4, 0xFFCCCCCC, "PW");
    DrawInputField(ui, field_x, row_y, field_w, field_h, fields_[1].act);
    if (fields_[1].buf[0]) {
        char m[64]; int l = (int)strlen(fields_[1].buf);
        for (int i = 0; i < l && i < 63; i++) m[i] = '*';
        m[std::min(l, 63)] = 0;
        ui.DrawText(field_x + 6, row_y + 4, 0xFFFFFFFF, "%s", m);
    } else
        ui.DrawText(field_x + 6, row_y + 4, 0xFF606070, "Enter password");

    // ── Buttons ───────────────────────────────────────────────────────────────
    row_y += 46.0f;
    float btn_w = 100.0f, btn_h = 28.0f;
    float btn_login_x = px + (pw - btn_w * 2.0f - 12.0f) * 0.5f;
    DrawLunaButton(ui, btn_login_x, row_y, btn_w, btn_h, "LOGIN", false);
    DrawLunaButton(ui, btn_login_x + btn_w + 12.0f, row_y, btn_w, btn_h, "OFFLINE", false);

    // ── Help text ─────────────────────────────────────────────────────────────
    row_y += btn_h + 14.0f;
    ui.DrawText(px + 10, row_y, 0xFF606878, "Tab = Switch field   Enter = Login");
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
        if (len > 0) { f.buf[len - 1] = 0; f.pos = len - 1; }
        return true;
    }
    return true;
}

bool LoginScreen::HandleChar(unsigned int cp) {
    auto& f = fields_[0].act ? fields_[0] : fields_[1];
    if (cp >= 32 && cp <= 126) {
        int len = (int)strlen(f.buf);
        if (len < 60) { f.buf[len] = (char)cp; f.buf[len + 1] = 0; f.pos = len + 1; }
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
    auto req = luna::protocol::CreateLoginRequestDirect(fbb, user.c_str(), &hash,
        "1.0.0", 0, 0, "00-00-00-00-00-00");
    fbb.Finish(req);
    if (network_ && network_->IsConnected())
        network_->SendPacket(luna::protocol::PacketType_MP_USERCONN_LOGIN_SYN,
            fbb.GetBufferPointer(), fbb.GetSize());
    else {
        spdlog::info("[LOGIN] Offline → CharSelect");
        if (g_screen_mgr) g_screen_mgr->SwitchTo("charselect");
    }
    return true;
}
