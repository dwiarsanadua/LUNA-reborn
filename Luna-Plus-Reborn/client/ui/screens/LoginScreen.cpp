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
#include <bgfx/bgfx.h>
#include <stb_image.h>
#include <cstring>
#include <fstream>
#include <sstream>
#include <vector>
#include <cstdint>

extern ScreenManager* g_screen_mgr;

namespace {
    struct SHA256_CTX {
        uint8_t data[64];
        uint32_t datalen;
        unsigned long long bitlen;
        uint32_t state[8];
    };

    #define SHA256_ROTR(a,b) (((a) >> (b)) | ((a) << (32-(b))))
    #define SHA256_CH(x,y,z) ((x & y) ^ (~x & z))
    #define SHA256_MAJ(x,y,z) ((x & y) ^ (x & z) ^ (y & z))
    #define SHA256_EP0(x) (SHA256_ROTR(x,2) ^ SHA256_ROTR(x,13) ^ SHA256_ROTR(x,22))
    #define SHA256_EP1(x) (SHA256_ROTR(x,6) ^ SHA256_ROTR(x,11) ^ SHA256_ROTR(x,25))
    #define SHA256_SIG0(x) (SHA256_ROTR(x,7) ^ SHA256_ROTR(x,18) ^ (x >> 3))
    #define SHA256_SIG1(x) (SHA256_ROTR(x,17) ^ SHA256_ROTR(x,19) ^ (x >> 10))

    static const uint32_t SHA256_K[64] = {
        0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,
        0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5,
        0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,
        0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,
        0xe49b69c1,0xefbe4786,0x0fc19dc6,0x240ca1cc,
        0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,
        0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,
        0xc6e00bf3,0xd5a79147,0x06ca6351,0x14292967,
        0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,
        0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,
        0xa2bfe8a1,0xa81a664b,0xc24b8b70,0xc76c51a3,
        0xd192e819,0xd6990624,0xf40e3585,0x106aa070,
        0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,
        0x391c0cb3,0x4ed8aa4a,0x5b9cca4f,0x682e6ff3,
        0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,
        0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2
    };

    static void sha256_transform(SHA256_CTX* ctx, const uint8_t* data) {
        uint32_t a,b,c,d,e,f,g,h,i,j,t1,t2,m[64];
        for (i=0,j=0; i<16; i++,j+=4)
            m[i] = (data[j]<<24)|(data[j+1]<<16)|(data[j+2]<<8)|(data[j+3]);
        for (; i<64; i++)
            m[i] = SHA256_SIG1(m[i-2]) + m[i-7] + SHA256_SIG0(m[i-15]) + m[i-16];
        a = ctx->state[0]; b = ctx->state[1]; c = ctx->state[2]; d = ctx->state[3];
        e = ctx->state[4]; f = ctx->state[5]; g = ctx->state[6]; h = ctx->state[7];
        for (i=0; i<64; i++) {
            t1 = h + SHA256_EP1(e) + SHA256_CH(e,f,g) + SHA256_K[i] + m[i];
            t2 = SHA256_EP0(a) + SHA256_MAJ(a,b,c);
            h = g; g = f; f = e; e = d + t1; d = c; c = b; b = a; a = t1 + t2;
        }
        ctx->state[0] += a; ctx->state[1] += b; ctx->state[2] += c; ctx->state[3] += d;
        ctx->state[4] += e; ctx->state[5] += f; ctx->state[6] += g; ctx->state[7] += h;
    }

    static void sha256_init(SHA256_CTX* ctx) {
        ctx->datalen = 0; ctx->bitlen = 0;
        ctx->state[0] = 0x6a09e667; ctx->state[1] = 0xbb67ae85;
        ctx->state[2] = 0x3c6ef372; ctx->state[3] = 0xa54ff53a;
        ctx->state[4] = 0x510e527f; ctx->state[5] = 0x9b05688c;
        ctx->state[6] = 0x1f83d9ab; ctx->state[7] = 0x5be0cd19;
    }

    static void sha256_update(SHA256_CTX* ctx, const uint8_t* data, size_t len) {
        for (size_t i=0; i<len; i++) {
            ctx->data[ctx->datalen] = data[i]; ctx->datalen++;
            if (ctx->datalen == 64) {
                sha256_transform(ctx, ctx->data);
                ctx->bitlen += 512; ctx->datalen = 0;
            }
        }
    }

    static void sha256_final(SHA256_CTX* ctx, uint8_t* digest) {
        uint64_t i = ctx->datalen;
        ctx->data[i++] = 0x80;
        if (i > 56) {
            while (i < 64) ctx->data[i++] = 0;
            sha256_transform(ctx, ctx->data);
            memset(ctx->data, 0, 56);
        } else {
            while (i < 56) ctx->data[i++] = 0;
        }
        ctx->bitlen += ctx->datalen * 8;
        ctx->data[63] = (uint8_t)(ctx->bitlen);
        ctx->data[62] = (uint8_t)(ctx->bitlen >> 8);
        ctx->data[61] = (uint8_t)(ctx->bitlen >> 16);
        ctx->data[60] = (uint8_t)(ctx->bitlen >> 24);
        ctx->data[59] = (uint8_t)(ctx->bitlen >> 32);
        ctx->data[58] = (uint8_t)(ctx->bitlen >> 40);
        ctx->data[57] = (uint8_t)(ctx->bitlen >> 48);
        ctx->data[56] = (uint8_t)(ctx->bitlen >> 56);
        sha256_transform(ctx, ctx->data);
        for (i=0; i<4; i++) {
            digest[i]    = (ctx->state[0] >> (24 - i*8)) & 0x000000ff;
            digest[4+i]  = (ctx->state[1] >> (24 - i*8)) & 0x000000ff;
            digest[8+i]  = (ctx->state[2] >> (24 - i*8)) & 0x000000ff;
            digest[12+i] = (ctx->state[3] >> (24 - i*8)) & 0x000000ff;
            digest[16+i] = (ctx->state[4] >> (24 - i*8)) & 0x000000ff;
            digest[20+i] = (ctx->state[5] >> (24 - i*8)) & 0x000000ff;
            digest[24+i] = (ctx->state[6] >> (24 - i*8)) & 0x000000ff;
            digest[28+i] = (ctx->state[7] >> (24 - i*8)) & 0x000000ff;
        }
    }

    static std::vector<uint8_t> sha256_digest(const std::string& input) {
        SHA256_CTX ctx;
        sha256_init(&ctx);
        sha256_update(&ctx, (const uint8_t*)input.data(), input.size());
        std::vector<uint8_t> hash(32);
        sha256_final(&ctx, hash.data());
        return hash;
    }

    static constexpr const char* SAVE_ID_KEY = "login.saved_id";
}

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
    save_id_ = false;
    error_message_.clear();
    error_timer_ = 0;
    std::string saved = ConfigManager::GetString(SAVE_ID_KEY, "");
    if (!saved.empty()) {
        strncpy(id_field_.buffer, saved.c_str(), sizeof(id_field_.buffer) - 1);
        id_field_.buffer[sizeof(id_field_.buffer) - 1] = '\0';
        id_field_.cursor_pos = (int)saved.size();
        save_id_ = true;
    }
    if (scene_renderer_) scene_renderer_->SetClearColor(0x887766FF);
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

        if (save_id_) {
            ConfigManager::SetString(SAVE_ID_KEY, id_field_.buffer);
        } else {
            ConfigManager::SetString(SAVE_ID_KEY, "");
        }
        ConfigManager::Save();

        flatbuffers::FlatBufferBuilder fbb;
        auto pw_hash_vec = fbb.CreateVector(sha256_digest(pw_field_.buffer));
        auto username = fbb.CreateString(id_field_.buffer);
        auto client_version = fbb.CreateString("1.1.0");
        auto mac_address = fbb.CreateString("00-00-00-00-00-00");

        auto req = luna::protocol::CreateLoginRequest(fbb, username, pw_hash_vec,
            client_version, 0 /*file_crc*/, 0 /*client_timestamp*/, mac_address);
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

    fy = wy + 140;
    ui.DrawRect(wx + 22, fy, 12, 12,
                save_id_ ? UIColor{100, 150, 255, 255} : UIColor{50, 50, 70, 255});
    ui.DrawBorder(wx + 22, fy, 12, 12, UIColor{80, 80, 120, 255});
    if (save_id_)
        ui.DrawText(wx + 24, fy, 0xFFCCCCFF, "%s", "✓");
    ui.DrawText(wx + 40, fy, 0xFFAAAAAA, "Save ID");

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

    if (key == 83) {
        save_id_ = !save_id_;
        return true;
    }

    return false;
}
