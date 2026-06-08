# READY-TO-USE PROMPTS — Perfect Plan: Restorasi Total Luna-Plus-Reborn

> **5 Agent Paralel** — Zero file overlap. Berdasarkan analisis "Red Screen" dan gap visual/flow.
> **Target:** Client menampilkan 3D, UI original, shader Metal, path robust, behavior match Old.

---

## Daftar Isi

1. [Agent V — Visual Restoration (3D Login + Shader)](#1-agent-v--visual-restoration-3d-login--shader)
2. [Agent A — Asset Path & VFS](#2-agent-a--asset-path--vfs)
3. [Agent B — Behavior Sync (Hero + Monster + Constants)](#3-agent-b--behavior-sync-hero--monster--constants)
4. [Agent U — UI Original Restoration](#4-agent-u--ui-original-restoration)
5. [Agent T — Thread Safety & Update Loop](#5-agent-t--thread-safety--update-loop)
6. [Ringkasan Paralel](#6-ringkasan-paralel)

---

## 1. Agent V — Visual Restoration (3D Login + Shader)

> **Tujuan:** Login screen menampilkan 3D map background. Shader di-compile untuk Metal.
> **Area:** `client/ui/screens/LoginScreen.cpp`, `shaders/`, `client/rendering/`, `client/main.cpp`
> **Zero conflict:** Tidak sentuh Hero, AI, Network, UI logic, asset path.

```
Kamu adalah AGENT V — Visual Restoration Engineer.

## 🚨 SAFETY RULES
You ONLY modify files in:
  /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/client/ui/screens/LoginScreen.cpp
  /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/client/ui/screens/LoginScreen.hpp
  /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/client/main.cpp
  /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/client/rendering/SceneRenderer.cpp
  /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/client/rendering/SceneRenderer.hpp
  /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/engine/gx_render/RenderDevice.cpp
  /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/shaders/
  /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/client/engine/GraphicEngine.cpp

You NEVER touch game/ecs/, client/gameobjects/Hero*, client/network/, client/ui/dialogs/, tools/.

## TASKS

### Task V1 — Aktifkan 3D Rendering di Login Screen
Edit: /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/client/main.cpp

Hapus skip 3D untuk login/charselect. Di Old, map 51 di-render sebagai background login.

Cari blok:
```cpp
bool is_login = (screenManager.CurrentName() == "login" || screenManager.CurrentName() == "charselect");
if (!is_login) {
    gfx.BeginFrame(...);
    ...
}
```

Ubah jadi:
```cpp
// Render terrain, sky, dan props UNTUK SEMUA screen (termasuk login)
gfx.BeginFrame(cam.GetViewMatrix(), cam.GetProjectionMatrix(), sky_.GetLightDirection());
ambient.Update(dt, 51, 0.5f, g_state.player_x, 0, g_state.player_z);
gfx.Render(&terrain, &props, nullptr, cam.GetViewMatrix(), cam.GetProjectionMatrix());
gfx.RenderCharacters(time, cam.GetViewMatrix(), cam.GetProjectionMatrix());
```

Set camera position untuk login: target=(0,0,0), distance=80, pitch=-30, yaw=-45.

### Task V2 — Compile Shaders for Metal
cd /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/shaders

# Check if shaderc exists
ls /Users/macbookair/PRIBADI/luna-plus-master/external/bgfx/tools/bin/darwin/shaderc 2>/dev/null

# If not, build shaderc:
cd /Users/macbookair/PRIBADI/luna-plus-master/external/bgfx
cmake -B .build/ci -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_OSX_ARCHITECTURES=arm64 -DBGFX_BUILD_TOOLS=ON -DBGFX_BUILD_EXAMPLES=OFF -DBGFX_CONFIG_RENDERER_METAL=ON
ninja -C .build/ci shaderc

# Compile ALL shaders for Metal:
SHADERC=/Users/macbookair/PRIBADI/luna-plus-master/external/bgfx/.build/ci/tools/bin/shaderc
SHADER_DIR=/Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/shaders
OUTPUT_DIR=/Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/build/bin/shaders
mkdir -p $OUTPUT_DIR

for sc in $SHADER_DIR/*.sc; do
  name=$(basename "$sc" .sc)
  type=""
  case $name in
    vs_*) type="vertex" ;;
    fs_*) type="fragment" ;;
    cs_*) type="compute" ;;
  esac
  if [ -n "$type" ]; then
    $SHADERC -f "$sc" -o "$OUTPUT_DIR/${name}.bin" \
      --type $type --platform osx -p metal \
      --varyingdef $SHADER_DIR/varying.def.sc \
      -i $SHADER_DIR -i /Users/macbookair/PRIBADI/luna-plus-master/external/bgfx/src
    echo "Compiled: $name ($type)"
  fi
done

# Copy compiled shaders
cp $OUTPUT_DIR/*.bin $SHADER_DIR/

### Task V3 — Fix RenderDevice Clear Color
Edit: /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/engine/gx_render/RenderDevice.cpp

Ubah clear color dari 0xFF333333 (abu-abu gelap) jadi 0xFF6688AA (biru langit) agar kalau ada frame tanpa render, tidak kelihatan merah.

### Task V4 — Set SceneRenderer untuk Login
Edit: /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/client/ui/screens/LoginScreen.hpp

Tambah method: SetSceneRenderer(SceneRenderer* sr) { scene_renderer_ = sr; }
Tambah member: SceneRenderer* scene_renderer_ = nullptr;

### Task V5 — Build & Test
cd /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn
cmake --build build --target LunaPlusClient 2>&1 | tail -5
./build/bin/LunaPlusClient > /tmp/v_fix.log 2>&1 &
sleep 5
kill %1 2>/dev/null; wait %1 2>/dev/null
grep -i "error\|fail\|warn\|shader\|Terrain\|loaded\|Login" /tmp/v_fix.log | tail -10

## DELIVERABLE
git add client/main.cpp client/ui/screens/LoginScreen.cpp client/ui/screens/LoginScreen.hpp client/rendering/SceneRenderer.cpp engine/gx_render/RenderDevice.cpp shaders/
git commit -m "agent_v: 3D login background, Metal shaders, clear color fix"

## DO NOT
Touch game/ecs/, Hero*, network/, ui/dialogs/, tools/
```

---

## 2. Agent A — Asset Path & VFS

> **Tujuan:** Semua path asset menggunakan base path absolut. Virtual File System mencegah path error.
> **Area:** `client/rendering/`, `client/engine/`, `engine/gx_render/`, `client/ui/skin/`
> **Zero conflict:** Tidak sentuh shader, login screen, Hero, AI, network, UI dialogs.

```
Kamu adalah AGENT A — Asset Path & VFS Engineer.

## 🚨 SAFETY RULES
You ONLY modify files in:
  /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/client/rendering/
  /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/client/engine/
  /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/engine/gx_render/
  /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/client/ui/skin/

You NEVER touch game/ecs/, client/network/, client/ui/dialogs/, shaders/, tools/.

## TASKS

### Task A1 — Buat Virtual File System Header
Create: /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/engine/gx_render/VFS.h

```cpp
#pragma once
#include <string>
#include <vector>
#include <fstream>

// Virtual File System — base path + relative path resolver
class VFS {
public:
    static void Init(const std::string& base_path) {
        base_path_ = base_path;
        if (base_path_.empty() || base_path_.back() != '/')
            base_path_ += '/';
    }
    
    static std::string Resolve(const std::string& path) {
        if (path.empty()) return path;
        if (path[0] == '/') return path;  // absolute already
        if (path.find(base_path_) == 0) return path;  // already resolved
        return base_path_ + path;
    }
    
    static bool Exists(const std::string& path) {
        std::ifstream f(Resolve(path));
        return f.good();
    }
    
    static const std::string& GetBasePath() { return base_path_; }

private:
    static std::string base_path_;
};
```

### Task A2 — Implement VFS Base Path
Create: /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/engine/gx_render/VFS.cpp

```cpp
#include "VFS.h"
std::string VFS::base_path_ = "./";
```

### Task A3 — Update RenderDevice Init
Edit: /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/engine/gx_render/RenderDevice.cpp

Di Inisialisasi, set VFS base path dari executable location:
```cpp
#include "VFS.h"
// Di RenderDevice::Init(), setelah GLFW init:
char exe_path[1024];
uint32_t path_size = sizeof(exe_path);
if (glfwGetWindowAttrib(window_, GLFW_FOCUSED)) {} // dummy
// Get executable path (platform-specific)
#ifdef __APPLE__
    uint32_t size = sizeof(exe_path);
    _NSGetExecutablePath(exe_path, &size);
    std::string ep(exe_path);
    auto pos = ep.find_last_of('/');
    std::string dir = (pos != std::string::npos) ? ep.substr(0, pos + 1) : "./";
    // Navigate up from build/bin/ to project root
    VFS::Init(dir + "../../");
#endif
```

### Task A4 — Wrap All Asset Paths with VFS
Buat helper function di setiap file yang menggunakan hardcoded path:

Edit: Add VFS::Resolve() wrapper di semua fungsi yang membuka file:
```cpp
// Di TerrainRenderer.cpp, EngineSky.cpp, PropRenderer.cpp, UIRenderer.cpp, dll.
// Ubah: std::ifstream f("assets/textures/...")
// Jadi:  std::ifstream f(VFS::Resolve("assets/textures/..."))
```

Gunakan sed untuk replace massal:
```bash
cd /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn

# List file yang perlu diubah
for f in client/rendering/*.cpp client/engine/*.cpp; do
  if grep -q '"assets/' "$f"; then
    echo "NEEDS FIX: $f"
  fi
done
```

Untuk setiap file, wrap string literal `"assets/... "` dengan `VFS::Resolve(...)`:
```cpp
// Pattern: std::ifstream f("assets/path/file")
// Menjadi: std::ifstream f(VFS::Resolve("assets/path/file"))
```

### Task A5 — Build & Verify
cd /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn
cmake --build build --target LunaPlusClient 2>&1 | tail -5
cmake --build build --target MapServer 2>&1 | tail -3

## DELIVERABLE
git add engine/gx_render/VFS.h engine/gx_render/VFS.cpp engine/gx_render/RenderDevice.cpp client/rendering/ client/engine/ client/ui/skin/
git commit -m "agent_a: VFS system — all asset paths normalized"

## DO NOT
Touch game/ecs/, client/network/, client/ui/dialogs/, shaders/, tools/
```

---

## 3. Agent B — Behavior Sync (Hero + Monster + Constants)

> **Tujuan:** Sinkronisasi konstanta movement, state machine, dan timing Hero/Monster dengan Old.
> **Area:** `client/gameobjects/Hero.cpp`, `game/ecs/systems/AISystem.cpp`, `game/ecs/components/`
> **Zero conflict:** Tidak sentuh rendering, shader, UI, network, asset path.

```
Kamu adalah AGENT B — Behavior Sync Engineer.

## 🚨 SAFETY RULES
You ONLY modify files in:
  /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/client/gameobjects/Hero.cpp
  /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/client/gameobjects/Hero.hpp
  /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/client/engine/ServerAuthMovement.cpp
  /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/client/engine/ServerAuthMovement.hpp
  /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/game/ecs/systems/AISystem.cpp
  /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/game/ecs/components/AIComponent.hpp
  /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/game/ecs/systems/CombatSystem.cpp

You NEVER touch client/rendering/, client/ui/, shaders/, tools/, client/network/.

## BACKGROUND
Dari analisis FLOW_ANALYSIS.md, banyak konstanta Old yang tidak cocok dengan Reborn.

## TASKS

### Task B1 — Sinkronisasi Konstanta Hero
Edit: /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/client/gameobjects/Hero.cpp

Berdasarkan Old Hero.h dan HeroMove.cpp:
- Tambah auto-attack: setiap 1.2 detik (ATTACK_TICK dari Old), jika target dalam range, attack otomatis
- Tambah cooldown system: attack, skill, dash masing-masing punya timer
- State machine: pastikan transisi Casting→Skill→Idle punya delay yang sama dengan Old

Cari di Old:
```cpp
// Old HeroMove.cpp:489-543 — OnAttack():
// ATTACK_TICK = 1200ms cooldown
// Damage = ATK * random(0.9, 1.1) - DEF * 0.5
```

Implementasi di Reborn:
```cpp
void Hero::Update(float dt) {
    // Auto-attack
    if (target_entity_ != 0 && IsAlive()) {
        attack_cooldown_ -= dt;
        if (attack_cooldown_ <= 0 && IsInAttackRange()) {
            ExecuteAutoAttack();
            attack_cooldown_ = 1.2f;  // ATTACK_TICK = 1200ms
        }
    }
    // State machine + movement (existing)
    ProcessStateTransitions();
    // Physics
    if (physics_world_ && physics_char_id_ >= 0)
        physics_world_->SetCharacterPosition(physics_char_id_, glm::vec3(x_, y_, z_));
}
```

### Task B2 — Waypoint Movement Interpolation
Edit: /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/client/engine/ServerAuthMovement.cpp

Aktifkan interpolasi waypoint:
```cpp
glm::vec3 ServerAuthMovement::GetRenderPosition() const {
    if (state_history_.size() < 2) return current_state_.position;
    
    // Interpolasi antara 2 state terakhir
    auto& prev = state_history_[state_history_.size() - 2];
    auto& curr = state_history_.back();
    
    float t = (estimated_latency_ / 1000.0f) * 60.0f;  // latency compensation
    t = std::clamp(t, 0.0f, 1.0f);
    
    return glm::mix(prev.position, curr.position, t);
}
```

### Task B3 — Sinkronisasi AI Constants
Edit: /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/game/ecs/systems/AISystem.cpp

Update konstanta AI sesuai Old (Monster.cpp:235-283):
- Aggro scan: setiap 5.000 ms (bukan setiap frame)
- Battle delay: 10.000 ms sebelum state change
- Chase range: 50.0f (bukan 30.0f)
- Attack range: dari DB monster (fallback 3.0f)

```cpp
void AISystem::Update(entt::registry& registry, float dt) {
    auto view = registry.view<AIComponent, Transform, CharacterStats>();
    for (auto entity : view) {
        auto& ai = view.get<AIComponent>(entity);
        ai.state_timer += dt;
        
        // Aggro scan setiap 5 detik (seperti Old)
        if (ai.state_timer >= 5.0f && ai.state == AIComponent::Idle) {
            ScanForTargets(registry, entity, ai);
            ai.state_timer = 0;
        }
        
        // State machine dengan battle delay
        switch (ai.state) {
            case AIComponent::Idle:
                if (ai.aggro_target != 0) TransitionState(ai, AIComponent::Chase);
                break;
            case AIComponent::Chase:
                UpdateChase(registry, entity, ai);
                break;
            case AIComponent::Attack:
                // Battle delay 10 detik sebelum return ke chase
                if (ai.state_timer >= 10.0f) {
                    TransitionState(ai, AIComponent::Chase);
                }
                UpdateAttack(registry, entity, ai);
                break;
            // ... other states
        }
    }
}
```

### Task B4 — Tambah Monster Help Request
Edit: /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/game/ecs/systems/AISystem.cpp

Tambah function: RequestHelp(registry, entity, ai, target_id)
- Cari monster lain dalam radius 15.0f
- Set aggro_target mereka ke target yang sama
- Implementasi dari Old Monster::RequestHelp()

### Task B5 — Build & Verify
cd /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn
cmake --build build --target game_core 2>&1 | tail -5
cmake --build build --target LunaPlusClient 2>&1 | tail -5

## DELIVERABLE
git add client/gameobjects/Hero.cpp client/gameobjects/Hero.hpp client/engine/ServerAuthMovement.cpp client/engine/ServerAuthMovement.hpp game/ecs/systems/AISystem.cpp game/ecs/components/AIComponent.hpp game/ecs/systems/CombatSystem.cpp
git commit -m "agent_b: behavior sync — auto-attack, waypoint interpolation, AI constants, monster help"

## DO NOT
Touch client/rendering/, client/ui/, shaders/, tools/, client/network/
```

---

## 4. Agent U — UI Original Restoration

> **Tujuan:** Load UI layout dari file .bin.txt yang sudah di-decrypt, bukan hardcoded C++.
> **Area:** `client/ui/dialogs/`, `client/ui/UiScriptParser.cpp`, `client/ui/UiScriptParser.hpp`, `client/ui/WindowManager.cpp`
> **Zero conflict:** Tidak sentuh rendering, shader, Hero, AI, network, asset VFS.

```
Kamu adalah AGENT U — UI Original Restoration Engineer.

## 🚨 SAFETY RULES
You ONLY modify files in:
  /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/client/ui/dialogs/
  /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/client/ui/UiScriptParser.cpp
  /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/client/ui/UiScriptParser.hpp
  /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/client/ui/WindowManager.cpp
  /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/client/ui/WindowManager.hpp
  /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/client/ui/ColorPalette.hpp

You NEVER touch client/rendering/, client/gameobjects/, game/ecs/, shaders/, tools/, client/network/.

## TASKS

### Task U1 — Fix UiScriptParser untuk .bin.txt
Edit: /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/client/ui/UiScriptParser.cpp

Perbaiki parser untuk membaca format .bin.txt dengan benar:
```cpp
UiElement UiScriptParser::ParseFile(const std::string& path) {
    ParseContext ctx;
    ctx.file.open(path);
    if (!ctx.file.is_open()) {
        // Fallback: coba tanpa .txt
        std::string alt = path;
        if (alt.size() > 4 && alt.substr(alt.size()-4) == ".txt")
            alt = alt.substr(0, alt.size()-4);
        ctx.file.open(alt);
    }
    if (!ctx.file.is_open()) {
        spdlog::error("UiScriptParser: Failed to open {}", path);
        return {};
    }
    // Parse the .bin.txt format:
    // $DIALOGNAME
    // {
    //    #KEY VALUE
    //    $CHILDNAME { ... }
    // }
    return ParseStream(ctx);
}
```

### Task U2 — Load UI Atlas Pre-Load
Edit: /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/client/ui/WindowManager.cpp

Tambah pre-load UI atlas dari assets/textures/ yang sudah di-decrypt:
```cpp
void WindowManager::PreloadUI(const std::string& interface_path) {
    // Scan assets/interface/Windows/*.bin.txt
    // Parse setiap file untuk tau texture apa yang dibutuhkan
    // Pre-load texture ke atlas
    // Cegah flickering
}
```

### Task U3 — Color Palette Integration
Edit: /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/client/ui/ColorPalette.hpp

Pastikan semua konstanta warna sudah sesuai dengan Old cWindowDef.h:
```cpp
// Verifikasi dari Old:
// RAREITEM     = 0xffffff00  (kuning)
// MAGICITEM    = 0xff4444ff  (biru)
// UNIQUEITEM   = 0xffff8800  (oranye)
// LEGENDARY    = 0xffff4444  (merah)
// QUESTITEM    = 0xff88ff88  (hijau)
// SETITEM      = 0xff00ff00  (hijau terang)
```

Edit setiap dialog yang menggunakan hardcoded color untuk pakai ColorPalette:
```cpp
// Ganti:
// ui.DrawText(..., 0xffffffff);
// Jadi:
// ui.DrawText(..., ColorPalette::TEXT_NORMAL);
```

### Task U4 — Activate LoadFromScript untuk Dialog
Edit: Setiap dialog di /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/client/ui/dialogs/

Pastikan masing-masing dialog memanggil LoadFromScript di konstruktor:
```cpp
// Di InventoryDialog:
window_ = wm->LoadFromScript("assets/interface/Windows/Inven.bin.txt");
if (!window_) {
    spdlog::warn("InventoryDialog: failed to load UI script, using C++ fallback");
    // Fallback: existing C++ code
}
```

Dialog yang perlu diubah:
- InventoryDialog
- SkillDialog
- QuestDialog
- CharacterDialog
- PartyDialog
- GuildDialog
- FriendDialog
- StorageDialog
- TradeDialog
- OptionsDialog
- NPCDialog

### Task U5 — Build & Test
cd /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn
cmake --build build --target LunaPlusClient 2>&1 | tail -5

## DELIVERABLE
git add client/ui/UiScriptParser.cpp client/ui/UiScriptParser.hpp client/ui/WindowManager.cpp client/ui/WindowManager.hpp client/ui/dialogs/ client/ui/ColorPalette.hpp
git commit -m "agent_u: UI restoration — .bin.txt parser, atlas preload, color palette, dialog scripts"

## DO NOT
Touch client/rendering/, client/gameobjects/, game/ecs/, shaders/, tools/, client/network/
```

---

## 5. Agent T — Thread Safety & Update Loop

> **Tujuan:** Fix update loop order, network thread safety, dan render timing.
> **Area:** `client/main.cpp`, `client/network/NetworkClient.cpp`, `client/network/NetworkClient.hpp`
> **Zero conflict:** Tidak sentuh Hero, AI, UI dialogs, rendering, shaders, VFS.

```
Kamu adalah AGENT T — Thread Safety & Update Loop Engineer.

## 🚨 SAFETY RULES
You ONLY modify files in:
  /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/client/main.cpp
  /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/client/network/NetworkClient.cpp
  /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/client/network/NetworkClient.hpp

You NEVER touch client/rendering/, client/gameobjects/, game/ecs/, shaders/, tools/, client/ui/dialogs/.

## TASKS

### Task T1 — Fix Update Loop Order
Edit: /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/client/main.cpp

Pindah game state UPDATE sebelum RENDER:
```cpp
while (!device.ShouldClose() && g_running) {
    // 1. Frame timing
    auto now = clock::now();
    float dt = std::chrono::duration<float>(now - last_time).count();
    last_time = now; time += dt; frame++;

    // 2. Input
    device.BeginFrame();  // GLFW poll events
    input_sys.Update(dt);

    // 3. Network events (thread-safe)
    g_network.ProcessEvents();  // ⬅️ TAMBAH: process network queue

    // 4. Camera
    cam.SetTarget(glm::vec3(g_state.player_x, 0, g_state.player_z));
    cam.Update(dt);

    // 5. ⬅️ GAME STATE UPDATE SEBELUM RENDER
    screenManager.Update(dt);
    
    // 6. Persistence
    persistence.Update(dt);

    // 7. RENDER — setelah game state update
    ui.BeginFrame();
    screenManager.Render(ui, cam.GetViewMatrix(), cam.GetProjectionMatrix());
    
    // 8. 3D rendering
    gfx.BeginFrame(cam.GetViewMatrix(), cam.GetProjectionMatrix());
    gfx.Render(&terrain, &props, nullptr, cam.GetViewMatrix(), cam.GetProjectionMatrix());
    gfx.RenderCharacters(time, cam.GetViewMatrix(), cam.GetProjectionMatrix());
    
    // 9. FPS
    if (frame % 30 == 0) fps = 1.0f / dt;
    ui.DrawText(1200, 2, 0xff888888, "FPS: %.0f", fps);
    
    // 10. Present
    device.EndFrame();
}
```

### Task T2 — Thread-Safe Network Queue
Edit: /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/client/network/NetworkClient.hpp

Tambah event queue + mutex:
```cpp
#include <mutex>
#include <queue>

struct NetworkEvent {
    uint16_t type;
    std::vector<uint8_t> payload;
};

// Private members:
std::queue<NetworkEvent> event_queue_;
std::mutex queue_mutex_;
```

Edit: /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/client/network/NetworkClient.cpp

Tambah method ProcessEvents:
```cpp
void NetworkClient::ProcessEvents() {
    std::lock_guard<std::mutex> lock(queue_mutex_);
    while (!event_queue_.empty()) {
        auto evt = event_queue_.front();
        event_queue_.pop();
        lock.unlock();
        if (handler_) handler_(evt.type, evt.payload);
        lock.lock();
    }
}
```

Modify ReadThread untuk push ke queue:
```cpp
void NetworkClient::ReadThread() {
    std::vector<uint8_t> buf(4096);
    while (connected_) {
        ssize_t n = ::recv(sock_, buf.data(), buf.size(), 0);
        if (n <= 0) { connected_ = false; break; }
        read_buf_.insert(read_buf_.end(), buf.data(), buf.data() + n);
        while (read_buf_.size() >= sizeof(PacketHeader)) {
            PacketHeader hdr;
            std::memcpy(&hdr, read_buf_.data(), sizeof(hdr));
            if (hdr.magic != 0x4C4E50) { read_buf_.clear(); break; }
            size_t total = sizeof(PacketHeader) + hdr.length;
            if (read_buf_.size() < total) break;
            std::vector<uint8_t> payload(read_buf_.begin() + sizeof(PacketHeader),
                                          read_buf_.begin() + total);
            read_buf_.erase(read_buf_.begin(), read_buf_.begin() + total);
            // Push to thread-safe queue instead of calling handler directly
            {
                std::lock_guard<std::mutex> lock(queue_mutex_);
                event_queue_.push({hdr.type, std::move(payload)});
            }
        }
    }
}
```

### Task T3 — Build & Test
cd /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn
cmake --build build --target LunaPlusClient 2>&1 | tail -5
./build/bin/LunaPlusClient > /tmp/t_fix.log 2>&1 &
sleep 4
kill %1 2>/dev/null; wait %1 2>/dev/null
grep -i "error\|fail\|warn\|Network\|Process\|Update\|Render" /tmp/t_fix.log | tail -10

## DELIVERABLE
git add client/main.cpp client/network/NetworkClient.cpp client/network/NetworkClient.hpp
git commit -m "agent_t: thread-safe network queue, fix update→render order"

## DO NOT
Touch client/rendering/, client/gameobjects/, game/ecs/, shaders/, tools/, client/ui/dialogs/
```

---

## 6. Ringkasan Paralel

### Area Kerja — Zero Conflict

| Agent | Direktori | Jenis File | Konflik dengan |
|-------|-----------|-----------|----------------|
| **V** | `client/main.cpp`, `LoginScreen.*`, `shaders/`, `SceneRenderer.*`, `GraphicEngine.cpp`, `RenderDevice.cpp` | .cpp, .hpp, .sc, .bin | ✅ **Tidak ada** |
| **A** | `client/rendering/*.cpp`, `client/engine/*.cpp`, `engine/gx_render/*`, `client/ui/skin/*` | .cpp, .hpp, .h | ✅ **Tidak ada** |
| **B** | `client/gameobjects/Hero.*`, `client/engine/ServerAuthMovement.*`, `game/ecs/systems/AISystem.*`, `game/ecs/components/AIComponent.*`, `CombatSystem.cpp` | .cpp, .hpp | ✅ **Tidak ada** |
| **U** | `client/ui/dialogs/*`, `UiScriptParser.*`, `WindowManager.*`, `ColorPalette.hpp` | .cpp, .hpp, .hpp | ✅ **Tidak ada** |
| **T** | `client/main.cpp`, `client/network/NetworkClient.*` | .cpp, .hpp | ✅ **Tidak ada** |

### Catatan

1. **Agent V dan Agent A sama-sama menyentuh `engine/gx_render/RenderDevice.cpp`** — tapi V hanya edit clear color, A hanya tambah VFS::Init(). Jika conflict, prioritas Agent V (visual fix lebih kritis), lalu Agent A merge manual.

2. **Agent B dan Agent T tidak overlap** dengan siapa pun.

3. **Semua aman dijalankan paralel.**
