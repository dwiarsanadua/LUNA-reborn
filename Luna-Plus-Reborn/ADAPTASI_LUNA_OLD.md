# Rencana Adaptasi Luna Plus Old → Reborn

**Prinsip**: Adaptasi behavior + data, rewrite platform layer. Tidak boleh ada "ngide" — setiap fitur harus punya referensi dari Old client.

---

## Daftar Isi
1. [Arsitektur Target](#1-arsitektur-target)
2. [Fase 0 — Stop Waste, Fix Flow](#2-fase-0--stop-waste-fix-flow)
3. [Fase 1 — Launcher Parity](#3-fase-1--launcher-parity)
4. [Fase 2 — Login Dialog Parity](#4-fase-2--login-dialog-parity)
5. [Fase 3 — Char Select Parity](#5-fase-3--char-select-parity)
6. [Fase 4 — Game World + UI Dialog Parity](#6-fase-4--game-world--ui-dialog-parity)
7. [Lampiran: Mapping Old → Reborn](#7-lampiran-mapping-old--reborn)

---

## 1. Arsitektur Target

### 1.1 Alur Flow (harus sama persis dengan Old)

```
[Launcher Screen]          ← Patch check, logo, news
       │
       ▼
[Login Dialog]             ← ID/Password, OK/Cancel, Save ID
       │
       ▼
[Char Select Screen]       ← List karakter, preview 3D, create/delete
       │
       ▼
[Game World Screen]        ← Full 3D + UI dialogs (inventory, skill, dll)
```

### 1.2 Layering (mirip Old, pakai teknologi baru)

```
┌─────────────────────────────────────────┐
│            ScreenManager                 │  ← Route screens (sudah ada)
│  ┌──────────┐ ┌──────────┐ ┌─────────┐  │
│  │ Launcher  │ │  Login   │ │CharSel  │  │  ← Screen C++ classes
│  │ Screen    │ │  Dialog  │ │ Screen  │  │
│  └──────────┘ └──────────┘ └─────────┘  │
│  ┌──────────────────────────────────┐   │
│  │         GameScreen              │   │  ← 3700 lines (sudah ada)
│  │  ┌──────────────────────────┐   │   │
│  │  │     WindowManager        │   │   │  ← Parse .bin.txt → dialog
│  │  │  37+ dialog C++ classes  │   │   │     213 layouts (sudah di-parse)
│  │  └──────────────────────────┘   │   │
│  └──────────────────────────────────┘   │
├─────────────────────────────────────────┤
│         ClientFlow (state machine)      │  ← Sudah ada, perlu diperkuat
├─────────────────────────────────────────┤
│         RenderDevice (bgfx + GLFW)      │  ← Sudah ada
├─────────────────────────────────────────┤
│         NetworkClient (asio)            │  ← Sudah ada
└─────────────────────────────────────────┘
```

### 1.3 File Structure Target

```
client/
├── main.cpp                          ← Minimal: init, loop, shutdown
├── screens/                          ← Screen classes
│   ├── LauncherScreen.hpp/cpp        ← Baru — patch + logo + news
│   ├── LoginScreen.hpp/cpp           ← Baru — ID/PW dialog via WindowManager
│   ├── CharSelectScreen.hpp/cpp      ← Update — wire char list + preview
│   └── GameScreen.hpp/cpp            ← Existing — perkuat
├── ui/
│   ├── screen_manager.hpp/cpp
│   ├── client_flow.hpp/cpp
│   ├── window_manager.hpp/cpp
│   ├── widgets/                      ← Reusable widgets (existing)
│   └── dialogs/                      ← 37+ dialog C++ classes (existing)
│   └── skin/                         ← UiSkinManager (existing)
├── rendering/                        ← Renderers (existing)
├── engine/                           ← Client engine (existing)
├── gameobjects/                      ← Hero, Monster, dll (existing)
├── network/                          ← Network client (existing)
├── audio/                            ← Audio (existing)
├── input/                            ← Input (existing)
└── config/                           ← Config (existing)

engine/                               ← Core engine (existing, minimal change)
game/                                 ← ECS library (existing, minimal change)
server/                               ← Server (existing, minimal change)
```

---

## 2. Fase 0 — Stop Waste, Fix Flow

**Goal**: Client tidak loading game world sebelum user login.

### 2.1 Masalah Saat Ini

Di `main.cpp`, urutan init:
```
Audio → GraphicEngine → Terrain → EngineMap::Load("51") → ... → ScreenManager → SwitchTo("login")
```

Map 51 di-load dengan terrain, spawn, props, sky — PADAHAL user belum login. Ini:
- Waste RAM + GPU (terrain, props, character renderer)
- Waste waktu startup (~2-3 detik)
- BGM map keputer sebelum login
- Post-FX nutup UI

### 2.2 Fix

Di `main.cpp`:

1. **Hapus loading game world dari init** — comment out:
   - `TerrainRenderer terrain;` (pindah ke GameScreen)
   - `PropRenderer props;` (pindah ke GameScreen)
   - `EngineMap map;` (pindah ke GameScreen)
   - `map.SetTerrain/SetProps/SetAudio/SetSpawnSystem` (pindah)
   - `map.Load("51")` — ini yang paling penting
   - `map.LoadFarmProps`
   - `sky.Init()` — pindah ke GameScreen
   - `g_char_renderer->Init()` — pindah ke GameScreen
   - `gamedb.LoadMonsterTemplates/LoadNPCTemplates/LoadMapData` — pindah ke GameScreen
   - `SpawnSystem` — pindah
   - `entt::registry` — pindah

2. **Yang tetap di main**:
   - `Logger`, `ConfigManager`
   - `Input` systems
   - `RenderDevice` + `GLFW`
   - `Paths` + `VFS`
   - `AudioManager`
   - `GraphicEngine` (minimal, tanpa scene/terrain)
   - `UIRenderer` (untuk UI 2D)
   - `ScreenManager` + registrasi screen
   - `ClientFlow`

3. **GameScreen::Enter()** akan load game world saat pertama kali di-switch.

### 2.3 File yang Diubah

- `client/main.cpp` — pindahkan ~80 lines dari init ke GameScreen
- `client/screens/GameScreen.hpp/cpp` — tambah method `LoadGameWorld()`, `UnloadGameWorld()`

### 2.4 Verifikasi

- Client startup dalam < 1 detik
- Tidak ada terrain, props, spawn, sky, atau karakter render sebelum login
- BGM tidak keputer di login
- Post-FX tidak aktif di login

---

## 3. Fase 1 — Launcher Parity

**Goal**: Layar launcher Old (logo, START, patch bar, news) persis di Reborn.

### 3.1 Old Reference

Old client punya `Launcher.exe` (MFC) yang:
1. Tampilkan logo Luna dari `C_launcher.png` sprite atlas
2. Tampilkan tombol "START" dari atlas
3. Cek patch via HTTP
4. Tampilkan progress bar saat patch
5. Tampilkan news/notice dari web
6. Setelah patch selesai → buka `Game.exe` → login dialog

### 3.2 Reborn Implementation

**Approach**: Bikin `LauncherScreen` C++ class yang:
1. Draw logo dari texture (pakai `C_launcher.png` yang sudah dikonversi)
2. Draw "START" button
3. Patch check via HTTP (pakai `PatchSystem` yang sudah ada)
4. Progress bar dari `UIRenderer::DrawBar`
5. Tombol START → `ClientFlow::StartPatch()` → `screenManager.SwitchTo("login")`

### 3.3 Detail Teknis

#### 3.3.1. Extract C_launcher.png dari legacy PAK

Cari di `Luna-Plus-Old/LEGACY_ASSETS/`:

```bash
# Cari file C_launcher
find /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Old -name "*launcher*" -o -name "*Launcher*" 2>/dev/null
```

Jika tidak ada, alternatif:
- Pakai `assets/textures/ui/Launcher/Launcher_01_01.png` sebagai background (sudah ada)
- Buat logo teks "LUNA Online Plus" via `UIRenderer::DrawText` dengan font besar
- Buat tombol START dari `login_bar01.png` yang sudah ada

#### 3.3.2. LauncherScreen class

**File baru**: `client/screens/LauncherScreen.hpp` dan `client/screens/LauncherScreen.cpp`

```cpp
// LauncherScreen.hpp
#pragma once
#include <ui/ScreenManager.hpp>
#include <rendering/UIRenderer.hpp>

class LauncherScreen : public Screen {
public:
    void Enter() override;
    void Exit() override;
    void Render(UIRenderer& ui) override;
    bool HandleKey(int key, int scancode, int action, int mods) override;
    void HandlePacket(uint16_t type, const std::vector<uint8_t>& payload) override;
    void Update(float dt) override;

private:
    bgfx::TextureHandle tex_bg_ = BGFX_INVALID_HANDLE;
    bgfx::TextureHandle tex_logo_ = BGFX_INVALID_HANDLE;
    bgfx::TextureHandle tex_start_ = BGFX_INVALID_HANDLE;
    
    bool patch_checked_ = false;
    bool patch_available_ = false;
    float patch_progress_ = 0.0f;
    std::string news_text_;
    
    enum class State { Idle, Checking, Downloading, Ready, Error };
    State state_ = State::Idle;
    float state_timer_ = 0;
};
```

**Render** (harus sama persis dengan Old):
```cpp
void LauncherScreen::Render(UIRenderer& ui) {
    float lw = ui.logicalWidth;
    float lh = ui.logicalHeight;
    
    // 1. Background — stretch ke fullscreen
    if (bgfx::isValid(tex_bg_))
        ui.DrawImage(0, 0, lw, lh, tex_bg_);
    
    // 2. Logo — center-top, dari C_launcher.png atau text fallback
    ui.DrawTextCentered(lh * 0.15f, 0xFFFFCC88, "LUNA Online Plus");
    ui.DrawTextCentered(lh * 0.15f + 30, 0xFF888888, "v1.1.0");
    
    // 3. START button — center
    float btn_x = lw * 0.5f - 100;
    float btn_y = lh * 0.5f;
    float btn_w = 200;
    float btn_h = 50;
    
    bool hover = false; // hitung dari mouse position nanti
    ui.DrawButton(btn_x, btn_y, btn_w, btn_h, "START", hover);
    
    // 4. Patch progress bar
    if (state_ == State::Downloading) {
        ui.DrawBar(lw * 0.3f, lh * 0.7f, lw * 0.4f, 20, patch_progress_,
                   UIColor{100, 200, 255, 255}, UIColor{50, 50, 80, 200});
        ui.DrawTextCentered(lh * 0.75f, 0xFFFFFFFF, "Downloading patch... %d%%", 
                           (int)(patch_progress_ * 100));
    }
    
    // 5. News text
    if (!news_text_.empty()) {
        ui.DrawText(lw * 0.1f, lh * 0.85f, 0xFFAAAAAA, "Notice: %s", 
                   news_text_.c_str());
    }
    
    // 6. Version + copyright di bottom
    ui.DrawTextCentered(lh * 0.95f, 0xFF666666, "LUNA Plus Reborn © 2024");
}
```

### 3.4 Files

| File | Action |
|------|--------|
| `client/screens/LauncherScreen.hpp` | New |
| `client/screens/LauncherScreen.cpp` | New |
| `client/main.cpp` | Register LauncherScreen + SwitchTo("launcher") |
| `client/ui/ClientFlow.hpp/cpp` | Tambah method `StartPatch()`, `PatchComplete()` |

### 3.5 Verifikasi

- Client startup → launcher screen dengan logo + START button
- Tekan START → patch check (atau skip ke login)
- Progress bar visible saat patch
- Layout identik dengan Old

---

## 4. Fase 2 — Login Dialog Parity

**Goal**: Dialog login ID/Password persis seperti Old.

### 4.1 Old Reference

Old client pakai `WebLauncherIDPass.bin.txt` yang di-render oleh `cWindowManager`:
- Window title: "LUNA" atau "Login"
- ID field: edit box dengan label "Account"
- Password field: edit box dengan label "Password", masked
- OK button
- Cancel button
- Save ID checkbox
- Error message area

**Window layout (dari .bin.txt parser)**:
```
WebLauncherIDPass.bin.txt:
  - Window: 400x300, centered
  - Label "Account ID": (50, 80)
  - EditBox ID: (150, 75, 200, 25)
  - Label "Password": (50, 120)
  - EditBox PW: (150, 115, 200, 25), password=true
  - CheckBox "Save ID": (150, 155, 100, 20)
  - Button "OK": (150, 200, 80, 30)
  - Button "Cancel": (270, 200, 80, 30)
  - Error text: (50, 250)
```

### 4.2 Reborn Implementation

**Approach**: Dua opsi:

**Opsi A (Cepat, direkomendasikan)**: Update `LoginScreen` existing untuk match Old:
- Ganti 3 hardcoded akun dengan ID/PW input fields
- Pakai `Keyboard::GetTextInput()` untuk input
- Render window frame via `UiSkinManager::DrawWindow()`
- Render fields via `UIRenderer::DrawRect` + `UIRenderer::DrawText`
- Render button via `UiSkinManager::DrawButton()`

**Opsi B (Lengkap, lebih lama)**: Wire `WindowManager` untuk render `WebLauncherIDPass.bin.txt` langsung.

Untuk Fase 2, pilih **Opsi A** — lebih cepat, kontrol penuh.

### 4.3 Detail LoginScreen Update

```cpp
// LoginScreen.hpp — tambah
struct InputField {
    char buffer[64];
    int cursor_pos = 0;
    bool active = false;
    bool masked = false;
    std::string label;
    float x, y, w, h;
};

InputField id_field_;
InputField pw_field_;
bool save_id_ = false;
std::string error_message_;
float error_timer_ = 0;

void DrawField(UIRenderer& ui, const InputField& field, bool focus);
bool HandleChar(unsigned int codepoint);
bool HandleKey(int key);
```

**Render**:
```cpp
void LoginScreen::Render(UIRenderer& ui) {
    float lw = ui.logicalWidth;
    float lh = ui.logicalHeight;
    
    // Background
    if (bgfx::isValid(tex_bg_))
        ui.DrawImage(0, 0, lw, lh, tex_bg_);
    
    // Window frame — match Old position
    float wx = lw * 0.5f - 200;
    float wy = lh * 0.35f;
    float ww = 400, wh = 300;
    UiSkinManager::DrawWindow(ui, wx, wy, ww, wh, "main", "Account Login");
    
    // ID Field
    DrawField(ui, id_field_, id_field_.active);
    
    // PW Field
    DrawField(ui, pw_field_, pw_field_.active);
    
    // Save ID checkbox
    // ...
    
    // OK / Cancel buttons
    UiSkinManager::DrawButton(ui, wx + 50, wy + 200, 130, 35, "default", "OK", ...);
    UiSkinManager::DrawButton(ui, wx + 220, wy + 200, 130, 35, "default", "Cancel", ...);
    
    // Error message
    if (!error_message_.empty() && error_timer_ > 0) {
        ui.DrawText(wx + 50, wy + 250, 0xFFFF4444, "%s", error_message_.c_str());
    }
    
    // Bottom bar (from Old)
    if (bgfx::isValid(tex_bar_))
        ui.DrawImage(0, lh - 120, lw, 120, tex_bar_);
}
```

**Input**:
```cpp
bool LoginScreen::HandleKey(int key, int scancode, int action, int mods) {
    if (action != 1) return false;
    
    if (key == 259) { // Backspace
        if (id_field_.active && id_field_.cursor_pos > 0)
            id_field_.buffer[--id_field_.cursor_pos] = '\0';
        if (pw_field_.active && pw_field_.cursor_pos > 0)
            pw_field_.buffer[--pw_field_.cursor_pos] = '\0';
        return true;
    }
    
    if (key == 258) { // Tab — switch focus
        id_field_.active = !id_field_.active;
        pw_field_.active = !pw_field_.active;
        return true;
    }
    
    if (key == 257) { // Enter — login
        return DoLogin();
    }
    
    return false;
}

bool LoginScreen::HandleChar(unsigned int codepoint) {
    InputField* field = id_field_.active ? &id_field_ : 
                        pw_field_.active ? &pw_field_ : nullptr;
    if (!field) return false;
    if (codepoint < 32 || codepoint > 126) return false;
    if (field->cursor_pos >= 63) return false;
    field->buffer[field->cursor_pos++] = (char)codepoint;
    field->buffer[field->cursor_pos] = '\0';
    return true;
}
```

### 4.4 Files

| File | Action |
|------|--------|
| `client/screens/LoginScreen.hpp` | Update — tambah InputField, ID/PW, DrawField |
| `client/screens/LoginScreen.cpp` | Update — render + input handler baru |
| `client/main.cpp` | Switch ke "login" setelah launcher |

### 4.5 Verifikasi

- Login dialog muncul dengan window frame (skin)
- ID field bisa diisi keyboard
- Password field masked (****)
- Tab pindah field
- Enter → kirim login packet ke AgentServer
- Cancel → kembali ke launcher
- Error message muncul jika login gagal

---

## 5. Fase 3 — Char Select Parity

**Goal**: Layar select karakter persis Old.

### 5.1 Old Reference

Old client pakai `SelectCharScene.bin.txt`:
- Background 3D scene (map tertentu dengan lighting)
- Karakter 3D preview (model + animasi idle)
- Slot karakter (max 3-5)
- Tombol Create / Delete
- Tombol Enter Game
- Nama + level + class di setiap slot

### 5.2 Reborn Implementation

**Approach**: Update `CharSelectScreen` yang sudah ada.

**Yang perlu**:
1. 3D scene ringan (atau background textur)
2. Karakter preview — load model + idle animation via `CharacterRenderer`
3. Slot grid — dari `UiSkinManager::DrawSlot()`
4. Nama + level + class — text
5. Create/Delete/Enter buttons

```cpp
// CharSelectScreen.hpp — tambah
struct CharSlot {
    bool exists = false;
    std::string name;
    int level = 0;
    int char_class = 0;
    uint32_t model_id = 0;
    bgfx::TextureHandle preview = BGFX_INVALID_HANDLE;
};

std::vector<CharSlot> slots_;
int selected_slot_ = 0;
int preview_entity_id_ = -1;
```

**Render**:
```cpp
void CharSelectScreen::Render(UIRenderer& ui) {
    float lw = ui.logicalWidth;
    float lh = ui.logicalHeight;
    
    // 1. Clear to dark background (scene akan di-render oleh 3D camera)
    ui.DrawRect(0, 0, lw, lh, UIColor{10, 10, 20, 255});
    
    // 2. Title
    ui.DrawTextCentered(lh * 0.08f, 0xFFFFCC88, "Select Character");
    
    // 3. Character slots — horizontal grid
    float slot_start_x = lw * 0.5f - (slots_.size() * 120) * 0.5f;
    for (size_t i = 0; i < slots_.size(); i++) {
        float sx = slot_start_x + i * 130;
        float sy = lh * 0.25f;
        
        // Slot background
        bool sel = (i == selected_slot_);
        UiSkinManager::DrawSlot(ui, sx, sy, 100, "inventory", sel);
        
        // Character name
        if (slots_[i].exists) {
            ui.DrawTextCentered(sy + 110, 0xFFFFFFFF, "%s", slots_[i].name.c_str());
            ui.DrawTextCentered(sy + 130, 0xFFAAAAAA, "Lv.%d %s", 
                slots_[i].level, GetClassName(slots_[i].char_class));
        } else {
            ui.DrawTextCentered(sy + 110, 0xFF666666, "Empty Slot");
        }
    }
    
    // 4. Buttons
    UiSkinManager::DrawButton(ui, lw * 0.5f - 200, lh * 0.7f, 120, 35, 
                              "default", "Create", false);
    UiSkinManager::DrawButton(ui, lw * 0.5f - 60, lh * 0.7f, 120, 35,
                              "default", "Delete", false);
    UiSkinManager::DrawButton(ui, lw * 0.5f + 80, lh * 0.7f, 120, 35,
                              "default", "Enter Game", selected_slot_ >= 0);
}
```

### 5.3 Karakter 3D Preview

Di `GameScreen::Enter()`, kita perlu:
1. Load map khusus untuk background (map 0 atau scene kosong)
2. Set camera angle ke karakter
3. Load model idle animation
4. Render via `CharacterRenderer`

```cpp
void CharSelectScreen::Enter() {
    // Init lightweight 3D scene
    EngineCamera cam;
    cam.SetTarget(glm::vec3(0, 0, 0));
    cam.SetDistance(5.0f);
    cam.SetPitch(-15.0f);
    
    // Load character model (preview)
    preview_entity_id_ = g_char_renderer->Spawn(1, "models/character/example.glb", 
                                                0, 0, 0, 0xFFFFFFFF);
    g_char_renderer->Move(preview_entity_id_, 0, 0, 0, false, CharAnim::Idle);
}
```

### 5.4 Files

| File | Action |
|------|--------|
| `client/screens/CharSelectScreen.hpp` | Update — tambah CharSlot, preview logic |
| `client/screens/CharSelectScreen.cpp` | Update — render slot, handle packet untuk char list |
| `client/network/NetworkClient.hpp/cpp` | Pastikan char list packet di-handle |

### 5.5 Verifikasi

- Char select muncul setelah login sukses
- Slot karakter dari server
- Karakter preview 3D (idle animasi)
- Pilih slot → Enter Game → GameScreen
- Create/Delete button → dialog konfirmasi

---

## 6. Fase 4 — Game World + UI Dialog Parity

**Goal**: Game world + semua UI dialog berfungsi seperti Old.

### 6.1 Yang sudah ada

| Dialog | File | Status |
|--------|------|--------|
| Inventory | `InventoryDialog.cpp` | ✅ Ada |
| Skill | `SkillDialog.cpp` | ✅ Ada |
| Quest | `QuestDialog.cpp` | ✅ Ada |
| NPC | `NPCDialog.cpp` | ✅ Ada |
| Chat | `ChatPanel.cpp` | ✅ Ada |
| Party | `PartyDialog.cpp` | ✅ Ada |
| Guild | `GuildDialog.cpp` | ✅ Ada |
| ... 30+ lainnya | `dialogs/*.cpp` | ✅ Ada |

### 6.2 Yang perlu diperbaiki

1. **Dialog wiring**: Pastikan setiap dialog punya entry point dari `.bin.txt` layout atau dari game event
2. **Skin texture**: Fix `UiSkinManager::LoadTexture()` find paths — pastikan `close.png`, `min.png` dll ada
3. **Toggle visibility**: Hotkey untuk buka/tutup dialog (I=inventory, K=skill, dll)
4. **Mouse interaction**: Klik item, drag, tooltip

### 6.3 Prioritas Dialog Wiring

| Priority | Dialog | Old Hotkey | Reborn Status |
|----------|--------|-----------|---------------|
| P0 | Chat | Enter | ✅ ChatPanel ada |
| P0 | Inventory | I | ✅ InventoryDialog ada |
| P0 | Skill | K | ✅ SkillDialog ada |
| P0 | Quest | Q | ✅ QuestDialog ada |
| P1 | Character | C | ✅ CharacterDialog ada |
| P1 | NPC Shop | Click NPC | ✅ NPCDialog ada |
| P1 | MiniMap | M | ✅ MiniMapDlg ada |
| P1 | Party | P | ✅ PartyDialog ada |
| P2 | Guild | G | ✅ GuildDialog ada |
| P2 | Pet | T | ✅ PetDialog ada |
| P2 | Mount | R | ✅ MountDialog ada |
| P3 | Housing | H | ✅ HousingDialog ada |
| P3 | Farm | F | ✅ FarmSystem ada |
| P3 | Fishing | - | ✅ FishingDialog ada |
| P3 | Cooking | - | ✅ CookingDialog ada |

### 6.4 Dialog Activation Flow (seperti Old)

```
GameScreen::Update()
  → Check hotkey pressed
  → ToggleDialog("inventory")  // muncul/sembunyikan
  → WindowManager::Render()    // render semua dialog aktif
  → Handle mouse click → dispatch ke dialog aktif
  → Handle keyboard → dispatch ke dialog aktif
```

### 6.5 Files

| File | Action |
|------|--------|
| `client/ui/WindowManager.hpp/cpp` | Update — method `ToggleDialog()`, `IsDialogOpen()` |
| `client/screens/GameScreen.cpp` | Wiring hotkey → dialog toggle |
| `client/gameobjects/Hero.cpp` | Pastikan inventory/skill/quest sync dengan server |

---

## 7. Lampiran: Mapping Old → Reborn

### 7.1 Screen Flow

```
Old (Launcher.exe)              Reborn (LunaPlusClient)
─────────────────               ────────────────────
Launcher.exe start              LauncherScreen::Enter()
  ├─ Logo + START               ├─ Draw logo + START button
  ├─ Patch check HTTP           ├─ PatchSystem::Check()
  ├─ Progress bar               ├─ DrawBar()
  └─ Launch Game.exe            └─ screenManager.SwitchTo("login")

Old (Game.exe)                  Reborn (LunaPlusClient)
─────────────────               ────────────────────
Game.exe start                  LoginScreen::Enter()
  ├─ Login dialog               ├─ Draw ID/PW fields
  ├─ Char select                ├─ screenManager.SwitchTo("charselect")
  ├─ Loading screen             ├─ LoadingScreen::Enter()
  └─ Game world                 └─ GameScreen::Enter() + LoadGameWorld()
```

### 7.2 UI Component Mapping

| Old (MFC/COM) | Reborn (C++20) |
|---------------|----------------|
| `cWindowManager` | `WindowManager` (existing) |
| `.bin` script encrypted | `.bin.txt` parsed (213 layouts) |
| `C*Dialog` MFC classes | `*Dialog.cpp` (37 classes) |
| `CLauncherDlg` | `LauncherScreen` (baru) |
| `CLoginDlg` | `LoginScreen` (update) |
| `CSelectCharDlg` | `CharSelectScreen` (update) |
| `CGamingMain` | `GameScreen` (existing) |
| Windows GDI/DX9 | bgfx + UIRenderer |
| MFC message loop | GLFW callbacks |

### 7.3 Asset Mapping

| Old Asset | Reborn Asset | Status |
|-----------|-------------|--------|
| `C_launcher.png` | Belum di-extract | 🔴 Missing |
| `Launcher_01_01.png` | `assets/textures/ui/Launcher/` | ✅ Ada |
| `WebLauncherIDPass.bin.txt` | `assets/interface/Windows/` | ✅ Ada (213 layouts) |
| `SelectCharScene.bin.txt` | `assets/interface/Windows/` | ✅ Ada |
| Skin textures (close, min) | Belum di-extract | 🔴 Missing |
| `.mod` models | `.glb` / `.obj` | ✅ 15,577 models |
| `.anm` animations | `.anm.json` | ✅ 7,033 animations |
| `.dds` textures | `.png` | ✅ 11,752 textures |

### 7.4 Protocol Mapping

| Old Packet | Reborn FlatBuffers | Status |
|-----------|-------------------|--------|
| `MP_USERCONN_LOGIN_SYN` | `LoginRequest` | ✅ Wired |
| `MP_USERCONN_LOGIN_ACK` | `LoginResponse` | ✅ Wired |
| `MP_USERCONN_CHARLIST_REQ` | `CharacterListRequest` | ✅ Schema |
| `MP_USERCONN_CHARLIST_ACK` | `CharacterListResponse` | ✅ Schema |
| `MP_USERCONN_CHARSEL_REQ` | `CharacterSelectRequest` | ✅ Schema |
| `MP_USERCONN_CHARSEL_ACK` | `CharacterSelectResponse` | ✅ Schema |
| `MP_CHAR_MOVE_REQ` | `MovementRequest` | ✅ Wired |
| `MP_COMBAT_ATTACK_REQ` | `CombatRequest` | ✅ Wired |
| 51 lainnya | Schema ada, handler missing | 🟡 Partial |
| 23 kategori | Belum dibuat | 🔴 Missing |

---

## Ringkasan Eksekusi

| Fase | Durasi | Output |
|------|--------|--------|
| **Fase 0** — Stop waste | 1 hari | Client startup < 1s, no world before login |
| **Fase 1** — Launcher | 2-3 hari | LauncherScreen dengan logo + START + patch |
| **Fase 2** — Login dialog | 2-3 hari | ID/PW input, error handling, login flow |
| **Fase 3** — Char select | 3-5 hari | Slot karakter, preview 3D, pilih/enter |
| **Fase 4** — Game world | 2-4 minggu | Full dialog wiring, gameplay loop |

**Estimasi total**: ~6-8 minggu (1 developer full-time) untuk mencapai parity login-to-gameplay yang memuaskan.
