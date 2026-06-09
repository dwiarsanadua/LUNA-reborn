# Prompt Paralel Adaptasi Luna Old → Reborn

Lokasi proyek: `/Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn`
Build: `cmake --preset macos-debug && cmake --build --preset macos-debug -j$(sysctl -n hw.ncpu)`
Shaders: `cp build/macos-debug/bin/shaders/*.bin build/bin/shaders/`

---

## PROMPT A: Fase 0 + Fase 1 — Stop Waste + Launcher Screen

**Files affected**: `client/main.cpp`, `client/screens/LauncherScreen.hpp` (new), `client/screens/LauncherScreen.cpp` (new), `client/ui/ClientFlow.hpp/cpp`

### Fix 0a: Hapus loading game world dari init

Buka `client/main.cpp`. Comment out semua game world loading di init section:

1. `TerrainRenderer terrain;` dan semua kode terkait terrain (lines ~122-164)
2. `PropRenderer props;` — pindah instantiasi ke GameScreen
3. `EngineMap map;` — pindah ke GameScreen
4. `map.SetTerrain/SetProps/SetAudio/SetSpawnSystem/SetRegistry/SetGameDataDB`
5. `map.Load("51")` — INI YANG PALING PENTING
6. `map.LoadFarmProps`
7. `sky.Init()` — pindah ke GameScreen
8. `g_char_renderer = new CharacterRenderer(); g_char_renderer->Init()` — pindah ke GameScreen
9. `gamedb.LoadMonsterTemplates/LoadNPCTemplates/LoadMapData` — pindah ke GameScreen
10. `SpawnSystem spawn_sys;` — pindah
11. `entt::registry registry;` — pindah

**Yang TETAP di main**:
- Logger, ConfigManager ✅
- Input systems ✅
- RenderDevice + GLFW ✅
- Paths + VFS ✅
- AudioManager ✅
- GraphicEngine (minimal, tanpa scene setup) ✅
- UIRenderer ✅
- ScreenManager ✅
- ClientFlow ✅

### Fix 0b: Buat LauncherScreen

Buat file baru `client/screens/LauncherScreen.hpp`:

```cpp
#pragma once
#include <ui/ScreenManager.hpp>
#include <rendering/UIRenderer.hpp>

class LauncherScreen : public Screen {
public:
    void Enter() override;
    void Exit() override;
    void Render(UIRenderer& ui) override;
    bool HandleKey(int key, int scancode, int action, int mods) override;
    void Update(float dt) override;

private:
    bgfx::TextureHandle tex_bg_ = BGFX_INVALID_HANDLE;
    bool textures_loaded_ = false;
    float patch_progress_ = 0.0f;
    
    enum class State { Logo, Checking, Ready, Error };
    State state_ = State::Logo;
    float state_timer_ = 0;
    
    void TexturesLoadOnce();
};
```

Buat file baru `client/screens/LauncherScreen.cpp`:

```cpp
#include "LauncherScreen.hpp"
#include <config/Paths.hpp>
#include <engine/gx_render/VFS.h>
#include <engine/gx_render/Shader.h>
#include <spdlog/spdlog.h>

void LauncherScreen::Enter() {
    spdlog::info("Entering Launcher Screen");
    TexturesLoadOnce();
    state_ = State::Logo;
    state_timer_ = 0;
}

void LauncherScreen::Exit() {
    spdlog::info("Exiting Launcher Screen");
}

void LauncherScreen::TexturesLoadOnce() {
    if (textures_loaded_) return;
    
    std::string bg_paths[] = {
        VFS::Find("assets/textures/ui/Launcher/Launcher_01_01.png"),
        VFS::Find("assets/textures/Launcher_01_01.png"),
    };
    for (auto& p : bg_paths) {
        if (p.empty()) continue;
        int w, h, n;
        unsigned char* d = stbi_load(p.c_str(), &w, &h, &n, 4);
        if (d) {
            tex_bg_ = bgfx::createTexture2D((uint16_t)w, (uint16_t)h, false, 1,
                bgfx::TextureFormat::RGBA8, 0, bgfx::copy(d, w*h*4));
            stbi_image_free(d);
            if (bgfx::isValid(tex_bg_)) break;
        }
    }
    textures_loaded_ = true;
}

void LauncherScreen::Render(UIRenderer& ui) {
    float lw = ui.logicalWidth;
    float lh = ui.logicalHeight;
    
    // Background — fullscreen
    if (bgfx::isValid(tex_bg_))
        ui.DrawImage(0, 0, lw, lh, tex_bg_);
    else
        ui.DrawRect(0, 0, lw, lh, UIColor{20, 20, 40, 255});
    
    // Logo text
    ui.DrawTextCentered(lh * 0.15f, 0xFFFFCC88, "LUNA Online Plus");
    ui.DrawTextCentered(lh * 0.15f + 28, 0xFF888888, "Reborn v1.1.0");
    
    // START button
    float bx = lw * 0.5f - 100;
    float by = lh * 0.48f;
    ui.DrawButton(bx, by, 200, 48, "  START", false);
    ui.DrawTextCentered(by + 14, 0xFFFFFFFF, "PRESS ENTER");
    
    // Patch status
    if (state_ == State::Checking) {
        ui.DrawTextCentered(lh * 0.65f, 0xFFAAAAAA, "Checking for updates...");
    } else if (state_ == State::Error) {
        ui.DrawTextCentered(lh * 0.65f, 0xFFFF4444, "Patch check failed, continuing...");
    }
    
    // Copyright
    ui.DrawTextCentered(lh * 0.93f, 0xFF666666, "LUNA Plus Reborn");
    ui.DrawTextCentered(lh * 0.96f, 0xFF444444, "Based on LUNA Online by Eyasoft / GG");
}

bool LauncherScreen::HandleKey(int key, int scancode, int action, int mods) {
    if (action != 1) return false;
    if (key == 257 || key == 32) { // Enter or Space
        ClientFlow::StartLogin();
        return true;
    }
    return false;
}

void LauncherScreen::Update(float dt) {
    state_timer_ += dt;
    if (state_ == State::Logo && state_timer_ > 0.5f) {
        state_ = State::Ready;
    }
}
```

### Fix 0c: Update main.cpp — register LauncherScreen as default

Di `main.cpp`, setelah screen manager init, ganti `screenManager.SwitchTo("login")` jadi `screenManager.SwitchTo("launcher")`:

```cpp
// Register screens
screenManager.Register("launcher", std::make_unique<LauncherScreen>());
screenManager.Register("login", std::make_unique<LoginScreen>());
// ... other screens ...

// Masuk launcher screen (bukan login)
screenManager.SwitchTo("launcher");
```

### Fix 0d: Update ClientFlow

Di `client/ui/ClientFlow.hpp`, tambah:
```cpp
static void StartLogin();  // dari launcher → login
```

Di `client/ui/ClientFlow.cpp`:
```cpp
void ClientFlow::StartLogin() {
    if (g_screen_mgr) {
        g_screen_mgr->SwitchTo("login");
    }
}
```

Build dan test: `cmake --build build/macos-debug -j$(sysctl -n hw.ncpu)`

---

## PROMPT B: Fase 2 — Login Dialog ID/PW

**Files affected**: `client/screens/LoginScreen.hpp`, `client/screens/LoginScreen.cpp`

### Goal
Ganti 3 akun hardcoded (admin/test/demo) dengan ID/Password input fields seperti Old.

### Yang harus dilakukan

BACA file `client/screens/LoginScreen.hpp` dan `LoginScreen.cpp`.

#### 2a. Update LoginScreen.hpp

TAMBAHKAN ke class LoginScreen:

```cpp
struct InputField {
    char buffer[64] = {0};
    int cursor_pos = 0;
    bool active = false;
    bool masked = false;
    std::string label;
    float x = 0, y = 0, w = 200, h = 28;
};

InputField id_field_;
InputField pw_field_;
bool save_id_ = false;
std::string error_message_;
float error_timer_ = 0;

void DrawField(UIRenderer& ui, const InputField& field, bool focus);
bool DoLogin();
```

#### 2b. Update LoginScreen.cpp

UBAH `Enter()` — init fields:
```cpp
void LoginScreen::Enter() {
    TexturesLoadOnce();
    id_field_.label = "Account";
    id_field_.active = true;
    id_field_.masked = false;
    pw_field_.label = "Password";
    pw_field_.active = false;
    pw_field_.masked = true;
    error_message_.clear();
}
```

UBAH `Render()` — draw fields alih-alih 3 akun hardcoded:
```cpp
void LoginScreen::Render(UIRenderer& ui) {
    float lw = ui.logicalWidth;
    float lh = ui.logicalHeight;
    
    // Background
    if (bgfx::isValid(tex_bg_))
        ui.DrawImage(0, 0, lw, lh, tex_bg_);
    else
        ui.DrawRect(0, 0, lw, lh, UIColor{20, 20, 40, 255});
    
    // Window
    float wx = lw * 0.5f - 200;
    float wy = lh * 0.3f;
    
    // Window background
    ui.DrawRect(wx, wy, 400, 280, UIColor{30, 30, 50, 220});
    ui.DrawBorder(wx, wy, 400, 280, UIColor{100, 120, 180, 200});
    ui.DrawText(wx + 10, wy + 8, 0xFFFFCC88, "Account Login");
    
    // ID field
    float fy = wy + 50;
    ui.DrawText(wx + 20, fy, 0xFFCCCCCC, "%s", id_field_.label.c_str());
    ui.DrawRect(wx + 120, fy - 2, id_field_.w, id_field_.h, 
                id_field_.active ? UIColor{60, 60, 100, 255} : UIColor{40, 40, 70, 255});
    ui.DrawBorder(wx + 120, fy - 2, id_field_.w, id_field_.h, 
                  id_field_.active ? UIColor{150, 150, 255, 255} : UIColor{80, 80, 120, 255});
    // Show ID text or placeholder
    if (strlen(id_field_.buffer) > 0)
        ui.DrawText(wx + 125, fy + 2, 0xFFFFFFFF, "%s", id_field_.buffer);
    else
        ui.DrawText(wx + 125, fy + 2, 0xFF666666, "Enter account ID");
    
    // PW field
    fy = wy + 100;
    ui.DrawText(wx + 20, fy, 0xFFCCCCCC, "%s", pw_field_.label.c_str());
    ui.DrawRect(wx + 120, fy - 2, pw_field_.w, pw_field_.h,
                pw_field_.active ? UIColor{60, 60, 100, 255} : UIColor{40, 40, 70, 255});
    ui.DrawBorder(wx + 120, fy - 2, pw_field_.w, pw_field_.h,
                  pw_field_.active ? UIColor{150, 150, 255, 255} : UIColor{80, 80, 120, 255});
    // Show masked password
    if (strlen(pw_field_.buffer) > 0) {
        char masked[64]; int len = strlen(pw_field_.buffer);
        for (int i = 0; i < len && i < 63; i++) masked[i] = '*';
        masked[len] = '\0';
        ui.DrawText(wx + 125, fy + 2, 0xFFFFFFFF, "%s", masked);
    } else {
        ui.DrawText(wx + 125, fy + 2, 0xFF666666, "Enter password");
    }
    
    // OK / Cancel buttons
    ui.DrawButton(wx + 50, wy + 170, 130, 35, "OK", false);
    ui.DrawButton(wx + 220, wy + 170, 130, 35, "Cancel", false);
    
    // Error message
    if (error_timer_ > 0 && !error_message_.empty()) {
        ui.DrawText(wx + 20, wy + 230, 0xFFFF4444, "%s", error_message_.c_str());
    }
    
    // Bottom bar
    if (bgfx::isValid(tex_bar_))
        ui.DrawImage(0, lh - 120, lw, 120, tex_bar_);
    
    // Online button hints
    ui.DrawTextCentered(lh * 0.82f, 0xFF888888, "Tab=Switch Field  Enter=Login  Esc=Back");
}
```

UBAH `HandleKey()` — keyboard input untuk ID/PW:
```cpp
bool LoginScreen::HandleKey(int key, int scancode, int action, int mods) {
    if (action != 1) return false;
    
    if (key == 258) { // Tab — switch field focus
        id_field_.active = !id_field_.active;
        pw_field_.active = !pw_field_.active;
        return true;
    }
    
    if (key == 256) { // Escape — back to launcher
        ClientFlow::StartLogin(); // actually back
        return true;
    }
    
    if (key == 257) { // Enter — login
        return DoLogin();
    }
    
    if (key == 259) { // Backspace
        InputField* f = id_field_.active ? &id_field_ : &pw_field_;
        if (f->cursor_pos > 0) f->buffer[--f->cursor_pos] = '\0';
        return true;
    }
    
    return false;
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
```

TAMBAHKAN method `Update()`:
```cpp
void LoginScreen::Update(float dt) {
    if (error_timer_ > 0) error_timer_ -= dt;
}
```

JANGAN lupa update header dengan deklarasi method baru.

---

## PROMPT C: Fase 3 — Char Select dengan Slot + Preview

**Files affected**: `client/screens/CharSelectScreen.hpp`, `client/screens/CharSelectScreen.cpp`

### Goal
Char select screen dengan slot karakter + preview 3D + create/delete/enter.

### Yang harus dilakukan

BACA `client/screens/CharSelectScreen.hpp` dan `CharSelectScreen.cpp`.

#### 3a. Update CharSelectScreen.hpp

TAMBAHKAN:
```cpp
struct CharSlotInfo {
    bool exists = false;
    uint32_t entity_id = 0;
    std::string name;
    int level = 0;
    int char_class = 0;
    std::string model_path;
};

std::vector<CharSlotInfo> slots_;
int selected_slot_ = 0;
bool waiting_for_list_ = true;

void RequestCharList();
void SendCharSelect(int slot);
```

#### 3b. Update CharSelectScreen.cpp

```cpp
void CharSelectScreen::Enter() {
    spdlog::info("Entering Char Select Screen");
    waiting_for_list_ = true;
    selected_slot_ = 0;
    slots_.clear();
    RequestCharList();
}

void CharSelectScreen::RequestCharList() {
    if (!g_network.IsConnected()) return;
    flatbuffers::FlatBufferBuilder fbb;
    auto req = luna::protocol::CreateCharacterListRequest(fbb);
    fbb.Finish(req);
    g_network.SendPacket(luna::protocol::PacketType_MP_USERCONN_CHARLIST_REQ,
                         fbb.GetBufferPointer(), fbb.GetSize());
}

void CharSelectScreen::Render(UIRenderer& ui) {
    float lw = ui.logicalWidth;
    float lh = ui.logicalHeight;
    
    // Dark background
    ui.DrawRect(0, 0, lw, lh, UIColor{10, 10, 20, 255});
    
    // Title
    ui.DrawTextCentered(lh * 0.06f, 0xFFFFCC88, "Select Character");
    
    // Waiting / loading
    if (waiting_for_list_) {
        ui.DrawTextCentered(lh * 0.5f, 0xFF888888, "Loading characters...");
        return;
    }
    
    // Character slots grid
    int cols = 4;
    float slot_size = 110;
    float gap = 20;
    float total_w = slots_.size() * (slot_size + gap) - gap;
    float start_x = lw * 0.5f - total_w * 0.5f;
    
    for (size_t i = 0; i < slots_.size(); i++) {
        float sx = start_x + i * (slot_size + gap);
        float sy = lh * 0.18f;
        
        bool sel = (i == (size_t)selected_slot_);
        
        // Slot background
        ui.DrawRect(sx, sy, slot_size, slot_size * 1.4f,
                    sel ? UIColor{60, 60, 100, 200} : UIColor{30, 30, 50, 200});
        ui.DrawBorder(sx, sy, slot_size, slot_size * 1.4f,
                      sel ? UIColor{150, 150, 255, 255} : UIColor{80, 80, 120, 150});
        
        if (slots_[i].exists) {
            ui.DrawTextCentered(sy + slot_size * 1.4f + 10, 0xFFFFFFFF,
                               "%s", slots_[i].name.c_str());
            ui.DrawTextCentered(sy + slot_size * 1.4f + 28, 0xFFAAAAAA,
                               "Lv.%d", slots_[i].level);
        } else {
            ui.DrawTextCentered(sy + slot_size * 0.6f, 0xFF666666, "Empty");
            ui.DrawTextCentered(sy + slot_size * 1.4f + 10, 0xFF666666, "---");
        }
    }
    
    // Buttons
    float by = lh * 0.72f;
    UiSkinManager::DrawButton(ui, lw * 0.5f - 250, by, 130, 36, "default", "Create", false);
    UiSkinManager::DrawButton(ui, lw * 0.5f - 100, by, 130, 36, "default", "Delete", false);
    UiSkinManager::DrawButton(ui, lw * 0.5f + 50, by, 200, 36, "default",
                              "Enter Game", selected_slot_ >= 0 && slots_[selected_slot_].exists);
}

bool CharSelectScreen::HandleKey(int key, int scancode, int action, int mods) {
    if (action != 1) return false;
    
    if (key == 262) { // Right
        selected_slot_ = (selected_slot_ + 1) % std::max(1, (int)slots_.size());
        return true;
    }
    if (key == 263) { // Left
        selected_slot_ = (selected_slot_ - 1 + slots_.size()) % slots_.size();
        return true;
    }
    if (key == 257) { // Enter
        if (selected_slot_ >= 0 && selected_slot_ < (int)slots_.size() && slots_[selected_slot_].exists) {
            SendCharSelect(selected_slot_);
        }
        return true;
    }
    if (key == 256) { // Escape
        ClientFlow::StartLogin();
        return true;
    }
    return false;
}

void CharSelectScreen::SendCharSelect(int slot) {
    flatbuffers::FlatBufferBuilder fbb;
    auto req = luna::protocol::CreateCharacterSelectRequest(fbb, slot);
    fbb.Finish(req);
    g_network.SendPacket(luna::protocol::PacketType_MP_USERCONN_CHARSEL_REQ,
                         fbb.GetBufferPointer(), fbb.GetSize());
}

void CharSelectScreen::HandlePacket(uint16_t type, const std::vector<uint8_t>& payload) {
    if (type == luna::protocol::PacketType_MP_USERCONN_CHARLIST_ACK) {
        auto resp = luna::protocol::GetCharacterListResponse(payload.data());
        slots_.clear();
        for (int i = 0; i < (int)resp->characters()->size() && i < 8; i++) {
            auto c = resp->characters()->Get(i);
            CharSlotInfo slot;
            slot.exists = true;
            slot.name = c->name()->str();
            slot.level = c->level();
            slot.char_class = c->class_id();
            slots_.push_back(slot);
        }
        // Fill remaining slots
        while ((int)slots_.size() < 4) {
            slots_.push_back(CharSlotInfo{});
        }
        waiting_for_list_ = false;
    }
    if (type == luna::protocol::PacketType_MP_USERCONN_CHARSEL_ACK) {
        auto resp = luna::protocol::GetCharacterSelectResponse(payload.data());
        if (resp->success()) {
            g_state.map_id = resp->map_id();
            if (g_screen_mgr) g_screen_mgr->SwitchTo("loading");
        }
    }
}
```

JANGAN lupa update header dengan deklarasi method baru (`Update`, `HandlePacket` override).

---

## PROMPT D: Fase 4a — Wire Dialog Hotkeys di GameScreen

**Files affected**: `client/screens/GameScreen.hpp`, `client/screens/GameScreen.cpp`

### Goal
Wire hotkey untuk toggle dialog (I=Inventory, K=Skill, Q=Quest, C=Character, M=Minimap, P=Party, dll) seperti Old.

### Yang harus dilakukan

BACA `client/screens/GameScreen.hpp` dan `GameScreen.cpp`.

#### 4a. Update GameScreen.hpp

TAMBAHKAN:
```cpp
std::unordered_map<std::string, bool> dialog_visibility_;
void ToggleDialog(const std::string& name);
bool IsDialogOpen(const std::string& name) const;
```

#### 4b. Update GameScreen.cpp — handle hotkey di Update atau HandleKey

TAMBAHKAN method:
```cpp
void GameScreen::ToggleDialog(const std::string& name) {
    dialog_visibility_[name] = !dialog_visibility_[name];
}

bool GameScreen::IsDialogOpen(const std::string& name) const {
    auto it = dialog_visibility_.find(name);
    return it != dialog_visibility_.end() && it->second;
}
```

Di `HandleKey` atau `Update`, tambahkan hotkey mapping:
```cpp
// Di GameScreen::HandleKey() atau HandleHotkey()
switch (key) {
    case 73:  ToggleDialog("inventory"); break;  // I
    case 75:  ToggleDialog("skill"); break;      // K
    case 81:  ToggleDialog("quest"); break;      // Q
    case 67:  ToggleDialog("character"); break;   // C
    case 77:  ToggleDialog("minimap"); break;     // M
    case 80:  ToggleDialog("party"); break;       // P
    case 71:  ToggleDialog("guild"); break;       // G
    case 84:  ToggleDialog("pet"); break;         // T
    case 82:  ToggleDialog("mount"); break;       // R
    default: break;
}
```

Di `Render()`, render dialog yang visible:
```cpp
void GameScreen::Render(UIRenderer& ui) {
    // ... render world ...
    
    // Render active dialogs
    for (auto& [name, visible] : dialog_visibility_) {
        if (!visible) continue;
        if (name == "inventory") inventory_dlg_.Render(ui);
        else if (name == "skill") skill_dlg_.Render(ui);
        else if (name == "quest") quest_dlg_.Render(ui);
        // ... etc
    }
    
    // Chat always visible
    chat_.Render(ui);
}
```

---

## PROMPT E: Extract Missing Assets (Skin Textures + C_launcher)

**Files affected**: None (script-only), output ke `assets/textures/ui/`

### Goal
Extract skin textures (`close.png`, `min.png`, dll) dan `C_launcher.png` dari legacy PAK file.

### Yang harus dilakukan

Buat script Python `tools/asset_pipeline/extract_skin_textures.py`:

```python
#!/usr/bin/env python3
"""
Extract missing UI skin textures from legacy PAK files.
Looks for C_launcher.png, close.png, min.png, and other skin textures.
"""
import os, sys, struct, glob
from pathlib import Path

# Cari legacy assets
LEGACY_DIRS = [
    "/Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Old/LEGACY_ASSETS/legacy_unpacked/raw_originals/assets/unpacked",
    "/Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Old/LEGACY_ASSETS",
]
DST = "/Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/assets/textures/ui/"

# Texture names that the UI skin system needs
NEEDED_TEXTURES = [
    "close", "close_f", "min", "min_f",
    "C_launcher", "Launcher_01_01",
    "login_bar00", "login_bar01",
    "btn_normal", "btn_hover", "btn_pressed",
    "title_left", "title_center", "title_right",
    "border_left", "border_right", "border_top", "border_bottom",
    "corner_tl", "corner_tr", "corner_bl", "corner_br",
    "body_bg",
    "scroll_track", "scroll_thumb",
    "tab_active", "tab_inactive", "tab_hover",
    "slot_normal", "slot_hover", "slot_highlight",
]

def main():
    os.makedirs(DST, exist_ok=True)
    found = 0
    missing = []
    
    # Search through all legacy directories recursively for PNG files
    for base in LEGACY_DIRS:
        for root, dirs, files in os.walk(base):
            for f in files:
                name_lower = f.lower()
                # Check if this file matches a needed texture
                for needed in NEEDED_TEXTURES:
                    if needed.lower() in name_lower and name_lower.endswith(('.png', '.dds', '.tga', '.tif')):
                        src = os.path.join(root, f)
                        # Convert to PNG if needed
                        dst = os.path.join(DST, needed + ".png")
                        if not os.path.exists(dst):
                            if f.endswith('.png'):
                                import shutil
                                shutil.copy2(src, dst)
                            else:
                                # Convert via sips
                                os.system(f'sips -s format png "{src}" --out "{dst}" 2>/dev/null')
                            if os.path.exists(dst):
                                print(f"  FOUND: {needed} -> {dst}")
                                found += 1
                        break
    
    print(f"\nFound {found} textures")
    
    # Generate placeholder for truly missing textures
    for needed in NEEDED_TEXTURES:
        dst = os.path.join(DST, needed + ".png")
        if not os.path.exists(dst):
            missing.append(needed)
            # Create a simple colored placeholder
            os.system(f'python3 -c "
from PIL import Image
img = Image.new(\'RGBA\', (32, 32), (100, 100, 150, 200))
img.save(\'{dst}\')
" 2>/dev/null || echo "skip placeholder"')
    
    if missing:
        print(f"Missing (placeholders created): {', '.join(missing)}")

if __name__ == "__main__":
    main()
```

Jalankan: `python3 tools/asset_pipeline/extract_skin_textures.py`

Test: jalankan client, cek log `UiSkin: cannot load texture` — seharusnya sudah tidak ada.
