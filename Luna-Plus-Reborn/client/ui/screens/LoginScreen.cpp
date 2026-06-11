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
#include <cstring>

extern ScreenManager* g_screen_mgr;
static constexpr const char* SAVE_ID_KEY = "login.saved_id";

namespace {
    std::vector<uint8_t> sha256(const std::string& s) {
        std::vector<uint8_t> h(32, 0);
        for (size_t i = 0; i < s.size() && i < 32; i++) h[i] = (uint8_t)s[i];
        return h;
    }

    // Procedural fallback: Old-Luna-style raised bevel panel
    void DrawFallbackPanel(UIRenderer& ui, float x, float y, float w, float h) {
        ui.DrawRect(x, y, w, h, UIColor{10, 12, 28, 230});
        ui.DrawRect(x, y, w, 1, UIColor{160, 190, 255, 80});
        ui.DrawRect(x, y, 1, h, UIColor{160, 190, 255, 80});
        ui.DrawRect(x, y + h - 1, w, 1, UIColor{0, 0, 0, 180});
        ui.DrawRect(x + w - 1, y, 1, h, UIColor{0, 0, 0, 180});
    }

    void DrawFallbackTitleBar(UIRenderer& ui, float x, float y, float w, float th) {
        float step = th / 4.0f;
        UIColor cols[4] = {{60,90,180,240},{42,66,148,240},{30,48,120,240},{18,28,80,240}};
        for (int i = 0; i < 4; ++i)
            ui.DrawRect(x, y + step * i, w, step + 1.0f, cols[i]);
        ui.DrawRect(x, y + th - 1, w, 1, UIColor{212, 175, 96, 220});
    }

    void DrawFallbackInput(UIRenderer& ui, float x, float y, float w, float h, bool active) {
        ui.DrawRect(x, y, w, h, active ? UIColor{32,40,80,220} : UIColor{15,18,40,210});
        ui.DrawBorder(x, y, w, h, active ? UIColor{120,160,255,200} : UIColor{60,80,140,160});
        ui.DrawRect(x+1, y+1, w-2, h*0.4f, UIColor{255,255,255,(uint8_t)(active?25:12)});
    }

    void DrawFallbackButton(UIRenderer& ui, float x, float y, float w, float h,
                            const char* text, bool hover, bool pressed=false) {
        UIColor base = pressed ? UIColor{25,45,100,240} : (hover ? UIColor{60,100,190,240} : UIColor{40,70,150,230});
        ui.DrawRect(x, y, w, h, base);
        if (!pressed) ui.DrawRect(x+1, y+1, w-2, h*0.45f, UIColor{255,255,255,(uint8_t)(hover?50:30)});
        ui.DrawBorder(x, y, w, h, UIColor{10,14,30,220});
        ui.DrawBorder(x+1, y+1, w-2, h-2, hover ? UIColor{160,200,255,180} : UIColor{100,140,220,130});
        float tw = ui.MeasureText(text);
        float tx = x + (w - tw) * 0.5f, ty = y + (h - 16.0f) * 0.5f + (pressed?1.0f:0.0f);
        ui.DrawText(tx+1, ty+1, 0x88000020, "%s", text);
        ui.DrawText(tx, ty, 0xFFFFFFFF, "%s", text);
    }
}

bgfx::TextureHandle LoginScreen::LoadTex(const char** paths, int count) {
    for (int i = 0; i < count; i++) {
        // Try both raw path and VFS-resolved path
        std::string resolved = VFS::Resolve(paths[i]);
        for (const std::string& p : {std::string(paths[i]), resolved}) {
            int w, h, n;
            auto* d = stbi_load(p.c_str(), &w, &h, &n, 4);
            if (d) {
                auto tex = bgfx::createTexture2D((uint16_t)w, (uint16_t)h, false, 1,
                    bgfx::TextureFormat::RGBA8, BGFX_SAMPLER_NONE,
                    bgfx::copy(d, w * h * 4));
                stbi_image_free(d);
                spdlog::debug("LoginScreen: loaded texture '{}'", p);
                return tex;
            }
        }
    }
    return BGFX_INVALID_HANDLE;
}

void LoginScreen::Init(GameState* state, NetworkClient* network) {
    state_ = state; network_ = network;
}

void LoginScreen::Enter() {
    spdlog::info("[LOGIN] Enter");

    // ── Load Launcher sprite sheet textures ───────────────────────────────────
    // Background: C_launcher.png (946×539) is the full-screen launcher BG
    {
        const char* paths[] = {
            "assets/textures/ui/Launcher/C_launcher.png",
            "assets/textures/ui/Launcher/lunaclient.png",
            "assets/textures/ui/lunaclient.png",
        };
        if (!bgfx::isValid(tex_bg_))
            tex_bg_ = LoadTex(paths, 3);
    }

    // Main panel frame: Launcher_01_01.png (595×480)
    {
        const char* paths[] = {"assets/textures/ui/Launcher/Launcher_01_01.png"};
        if (!bgfx::isValid(tex_panel_))
            tex_panel_ = LoadTex(paths, 1);
    }

    // Buttons: _07=normal(90×24) _08=hover _09=pressed
    {
        const char* n[] = {"assets/textures/ui/Launcher/Launcher_01_07.png"};
        const char* h[] = {"assets/textures/ui/Launcher/Launcher_01_08.png"};
        const char* p[] = {"assets/textures/ui/Launcher/Launcher_01_09.png"};
        if (!bgfx::isValid(tex_btn_n_)) tex_btn_n_ = LoadTex(n, 1);
        if (!bgfx::isValid(tex_btn_h_)) tex_btn_h_ = LoadTex(h, 1);
        if (!bgfx::isValid(tex_btn_p_)) tex_btn_p_ = LoadTex(p, 1);
    }

    // Input fields: _11=normal(112×51) _12=active
    {
        const char* fn[] = {"assets/textures/ui/Launcher/Launcher_01_11.png"};
        const char* fa[] = {"assets/textures/ui/Launcher/Launcher_01_12.png"};
        if (!bgfx::isValid(tex_field_n_)) tex_field_n_ = LoadTex(fn, 1);
        if (!bgfx::isValid(tex_field_a_)) tex_field_a_ = LoadTex(fa, 1);
    }

    // Divider bar: _05.png (472×9)
    {
        const char* paths[] = {"assets/textures/ui/Launcher/Launcher_01_05.png"};
        if (!bgfx::isValid(tex_div_)) tex_div_ = LoadTex(paths, 1);
    }

    // Close button: _02=normal(17×17) _03=hover
    {
        const char* cn[] = {"assets/textures/ui/Launcher/Launcher_01_02.png"};
        const char* ch[] = {"assets/textures/ui/Launcher/Launcher_01_03.png"};
        if (!bgfx::isValid(tex_close_n_)) tex_close_n_ = LoadTex(cn, 1);
        if (!bgfx::isValid(tex_close_h_)) tex_close_h_ = LoadTex(ch, 1);
    }

    fields_[0] = {"", 0, true,  false, "ID"};
    fields_[1] = {"", 0, false, true,  "PW"};

    std::string saved = ConfigManager::GetString(SAVE_ID_KEY, "");
    if (!saved.empty()) {
        strncpy(fields_[0].buf, saved.c_str(), sizeof(fields_[0].buf) - 1);
        fields_[0].pos = (int)saved.size();
    }

    if (scene_renderer_) scene_renderer_->SetClearColor(0x0A0A14FF);
    sent_       = false;
    anim_time_  = 0.0f;
    btn_login_hover_   = false;
    btn_offline_hover_ = false;
}

void LoginScreen::Exit() {}

void LoginScreen::Update(float dt) {
    anim_time_ += dt;
}

void LoginScreen::Render(UIRenderer& ui) {
    float lw = ui.logicalWidth, lh = ui.logicalHeight;

    // ── Full-screen background ────────────────────────────────────────────────
    if (bgfx::isValid(tex_bg_)) {
        ui.DrawImage(0, 0, lw, lh, tex_bg_);
    } else {
        // Procedural dark gradient
        ui.DrawRect(0, 0, lw, lh * 0.55f, UIColor{14, 18, 48, 255});
        ui.DrawRect(0, lh * 0.55f, lw, lh * 0.45f, UIColor{6, 8, 20, 255});
        ui.DrawRect(0, 0, 80, lh, UIColor{0, 0, 0, 60});
        ui.DrawRect(lw - 80, 0, 80, lh, UIColor{0, 0, 0, 60});
    }

    // ── Panel layout ──────────────────────────────────────────────────────────
    // Luna Plus had the login panel at center-right area, about 595×480 px at native res.
    // We scale proportionally so it fits any resolution.
    const float PANEL_W = 595.0f, PANEL_H = 480.0f;
    float scale = std::min(lw / 1024.0f, lh / 768.0f);
    float pw = PANEL_W * scale;
    float ph = PANEL_H * scale;
    float px = (lw - pw) * 0.5f;
    float py = (lh - ph) * 0.5f;

    if (bgfx::isValid(tex_panel_)) {
        // Use the actual panel frame sprite
        ui.DrawImage(px, py, pw, ph, tex_panel_);
    } else {
        // Procedural fallback
        ui.DrawRect(px + 4, py + 4, pw, ph, UIColor{0, 0, 0, 100}); // shadow
        DrawFallbackPanel(ui, px, py, pw, ph);
        ui.DrawBorder(px-1, py-1, pw+2, ph+2, UIColor{8,10,24,220});
        ui.DrawBorder(px, py, pw, ph, UIColor{100,140,220,160});
    }

    // ── Title bar (inside panel) ──────────────────────────────────────────────
    float th = 28.0f * scale;
    if (!bgfx::isValid(tex_panel_)) {
        DrawFallbackTitleBar(ui, px, py, pw, th);
    }

    // Title text
    float title_x = px + 16.0f * scale, title_y = py + 5.0f * scale;
    ui.DrawText(title_x + 1, title_y + 1, 0x88000000, "Account Login");
    ui.DrawText(title_x, title_y, 0xFFFFE8C0, "Account Login");

    // Close button (top-right of panel)
    float cb_s = 17.0f * scale;
    float cb_x = px + pw - cb_s - 4.0f * scale;
    float cb_y = py + 4.0f * scale;
    if (bgfx::isValid(tex_close_n_))
        ui.DrawImage(cb_x, cb_y, cb_s, cb_s, tex_close_n_);

    // ── Input field area ──────────────────────────────────────────────────────
    // Positioned within the panel frame — roughly centered vertically
    float field_area_y = py + th + (bgfx::isValid(tex_panel_) ? ph * 0.28f : 30.0f * scale);
    float label_x  = px + 30.0f * scale;
    float field_x  = px + 90.0f * scale;
    float field_w  = 200.0f * scale;
    float field_h  = 26.0f * scale;

    // Divider above inputs
    if (bgfx::isValid(tex_div_)) {
        float div_w = std::min(pw - 40.0f * scale, 472.0f * scale);
        ui.DrawImage(px + (pw - div_w) * 0.5f, field_area_y - 8.0f * scale, div_w, 9.0f * scale, tex_div_);
    }

    // ID field
    ui.DrawText(label_x + 1, field_area_y + 5, 0x66000000, "ID");
    ui.DrawText(label_x, field_area_y + 4, 0xFFCCCCCC, "ID");
    if (bgfx::isValid(fields_[0].act ? tex_field_a_ : tex_field_n_))
        ui.DrawImage(field_x, field_area_y, field_w, field_h,
                     fields_[0].act ? tex_field_a_ : tex_field_n_);
    else
        DrawFallbackInput(ui, field_x, field_area_y, field_w, field_h, fields_[0].act);

    if (fields_[0].buf[0])
        ui.DrawText(field_x + 8, field_area_y + 5, 0xFFFFFFFF, "%s", fields_[0].buf);
    else
        ui.DrawText(field_x + 8, field_area_y + 5, 0xFF606070, "Enter account ID");

    // PW field
    float pw_y = field_area_y + field_h + 12.0f * scale;
    ui.DrawText(label_x + 1, pw_y + 5, 0x66000000, "PW");
    ui.DrawText(label_x, pw_y + 4, 0xFFCCCCCC, "PW");
    if (bgfx::isValid(fields_[1].act ? tex_field_a_ : tex_field_n_))
        ui.DrawImage(field_x, pw_y, field_w, field_h,
                     fields_[1].act ? tex_field_a_ : tex_field_n_);
    else
        DrawFallbackInput(ui, field_x, pw_y, field_w, field_h, fields_[1].act);

    if (fields_[1].buf[0]) {
        char m[64]; int l = (int)strlen(fields_[1].buf);
        for (int i = 0; i < l && i < 63; i++) m[i] = '*'; m[std::min(l,63)] = 0;
        ui.DrawText(field_x + 8, pw_y + 5, 0xFFFFFFFF, "%s", m);
    } else
        ui.DrawText(field_x + 8, pw_y + 5, 0xFF606070, "Enter password");

    // ── Buttons ───────────────────────────────────────────────────────────────
    const float BTN_W = 90.0f * scale, BTN_H = 24.0f * scale;
    float btn_y = pw_y + field_h + 18.0f * scale;
    float btn_login_x   = px + pw * 0.5f - BTN_W - 8.0f * scale;
    float btn_offline_x = px + pw * 0.5f + 8.0f * scale;

    // LOGIN button
    auto btn_login_tex = btn_login_hover_ ? tex_btn_h_ : tex_btn_n_;
    if (bgfx::isValid(btn_login_tex))
        ui.DrawImage(btn_login_x, btn_y, BTN_W, BTN_H, btn_login_tex);
    else
        DrawFallbackButton(ui, btn_login_x, btn_y, BTN_W, BTN_H, "LOGIN", btn_login_hover_);
    // Button label (always draw on top)
    {
        float tw = ui.MeasureText("LOGIN");
        float tx = btn_login_x + (BTN_W - tw) * 0.5f, ty = btn_y + (BTN_H - 16.0f) * 0.5f;
        ui.DrawText(tx + 1, ty + 1, 0x88000000, "LOGIN");
        ui.DrawText(tx, ty, 0xFFFFFFFF, "LOGIN");
    }

    // OFFLINE (or START SERVER / 회원가입 like original) button
    auto btn_off_tex = btn_offline_hover_ ? tex_btn_h_ : tex_btn_n_;
    if (bgfx::isValid(btn_off_tex))
        ui.DrawImage(btn_offline_x, btn_y, BTN_W, BTN_H, btn_off_tex);
    else
        DrawFallbackButton(ui, btn_offline_x, btn_y, BTN_W, BTN_H, "OFFLINE", btn_offline_hover_);
    {
        float tw = ui.MeasureText("OFFLINE");
        float tx = btn_offline_x + (BTN_W - tw) * 0.5f, ty = btn_y + (BTN_H - 16.0f) * 0.5f;
        ui.DrawText(tx + 1, ty + 1, 0x88000000, "OFFLINE");
        ui.DrawText(tx, ty, 0xFFFFFFFF, "OFFLINE");
    }

    // ── Bottom bar ────────────────────────────────────────────────────────────
    float barH = 36.0f;
    ui.DrawRect(0, lh - barH, lw, barH, UIColor{8, 12, 36, 220});
    ui.DrawRect(0, lh - barH, lw, 1, UIColor{212, 175, 96, 160});
    ui.DrawRect(0, lh - barH + 1, lw, 1, UIColor{60, 80, 160, 80});

    // Fade-in version text
    float logo_alpha = std::min(255.0f, anim_time_ * 280.0f);
    uint8_t la = (uint8_t)logo_alpha;
    ui.DrawText(lw - 340, lh - barH - 40, (uint32_t(la) << 24) | 0x00EED090, "LUNA Plus Reborn");
    ui.DrawText(lw - 140, lh - barH + 10, 0xFF666677, "LUNA Plus Reborn v1.1");
    ui.DrawText(px + 10, btn_y + BTN_H + 12.0f, 0xFF505868, "Tab = Switch field   Enter = Login");
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
