# Deep Comparison: Luna-Plus-Old vs Luna-Plus-Reborn

> **Tanggal:** Juni 2026 (Updated: sync with REBORN_ULTIMATE_PLAN.md)
> **Tujuan:** Mengetahui posisi Reborn vs Old secara detail — apa yang sudah, apa yang belum, dan berapa besar gap-nya.

---

## 1. Source Code

### 1.1 Size

| Metrik | Luna-Plus-Old | Luna-Plus-Reborn | Rasio |
|--------|---------------|-------------------|-------|
| **.cpp files** | 3.384 | 240 | **7%** |
| **.h/.hpp files** | 4.414 | 380 (non-generated) | **9%** |
| **Total own code (LOC)** | ~2.922.486 | ~28.183 | **1%** |
| **+ vendored headers** | — | + ~148.000 (imgui, stb, miniaudio) | — |
| **FlatBuffers schemas** | 0 | 311 .fbs | — |

### 1.2 Per Component

| Komponen | Old (LOC) | Reborn (LOC) | Coverage |
|----------|-----------|-------------|----------|
| **Engine Rendering** | 113.186 | 997 | **0.9%** |
| **Engine Geometry** | 21.999 | 1.847 (gx_geom) | **8%** |
| **Engine Physics** | 0 (D3D9 built-in) | 500 (Jolt wrapper) | **NEW** |
| **Engine Exec/Scene** | 10.856 | 487 | **4%** |
| **Engine Network** | 6.590 | 400 | **6%** |
| **Engine Resource** | 0 (built-in) | 300 | **NEW** |
| **Engine Scripting** | 0 (built-in) | 300 | **NEW** |
| **Client UI/Dialogs** | ~80.000 | ~7.439 | **9%** |
| **Client Game Objects** | ~30.000 | ~5.221 | **17%** |
| **Client Rendering** | ~20.000 | ~1.994 | **10%** |
| **Client Audio** | 1.157 | 467 | **40%** |
| **Client Effects** | 7.147 | 523 | **7%** |
| **Client Input/Config** | ~3.000 | ~1.000 | **33%** |
| **Server (all 3)** | ~147.000 | ~1.010 | **0.7%** |
| **ECS (Reborn-only)** | 0 | ~3.652 | **NEW** |
| **Tools** | ~121.000 | ~2.403 | **2%** |
| **Libraries** | ~39.000 | 0 (vcpkg) | **0% (replaced)** |

---

## 2. Asset

| Kategori | Old | Reborn | Coverage | Gap |
|----------|-----|--------|----------|-----|
| **Textures** | 20.228 (DDS/PNG/TIF/TGA) | 11.752 (PNG) | **58%** | ~8.476 DDS duplikat |
| **Models (.glb)** | 8.815 (.mod) | 8.788 (.glb) | **99%** ✅ | ~27 |
| **Animations** | 7.105 (.anm) | 7.033 (.anm.json) | **99%** ✅ | ~72 |
| **Audio** | 978 (WAV/MP3) | 978 (WAV/MP3) | **100%** ✅ | 0 |
| **Heightmaps** | 51 (.hfl) | 51 (.hgt) | **100%** ✅ | 0 |
| **Scene Maps** | 53 (.map) | 53 (.json) | **100%** ✅ | 0 |
| **Char Defs** | 1.542 (.chx) | 1.457 (.json) | **94%** | 85 (accessories) |
| **UI Scripts** | 4.199 (.bin) | 0 | **0%** | 4.199 (encrypted) |
| **Shaders** | 5 (D3D9 .psh/.vsh) | 17 (.sc) + 9 (.bin) | **100%** ✅ | 0 |
| **Databases** | 3 (.db) | 4 (.db) | **100%** ✅ | 0 |

---

## 3. Feature Completeness

### 3.1 Engine Features

| Feature | Old | Reborn | Note |
|---------|-----|--------|------|
| 3D Rendering | Direct3D 9 | bgfx (Metal) | Reborn cross-platform ✅ |
| Shadow Mapping | ❌ | ✅ 1024x1024 | Reborn unggul |
| Sky Dome | ❌ | ✅ Day/night cycle | Reborn unggul |
| Model Loading | .mod proprietary | .glb/.obj via Assimp | Reborn standard ✅ |
| Skeletal Animation | ✅ CPU skinning | ✅ GPU skinning | Reborn lebih modern |
| Animation Blending | ❌ | ✅ Cross-fade | Reborn unggul |
| Physics Engine | ❌ (D3D9 built-in) | ✅ Jolt Physics | Reborn unggul |
| Collision | ✅ Heightfield + BV | ✅ Jolt raycast | Reborn lebih akurat |
| Resource Cache | ✅ Reference counted | ✅ Thread-safe template | Sama |
| Network Layer | IOCP (Windows only) | Asio (cross-platform) | Reborn unggul |
| Scripting | ❌ (hardcoded) | ✅ LuaJIT + sol2 | Reborn unggul |
| ECS | ❌ (manual OOP) | ✅ EnTT 18+19 | Reborn unggul |

### 3.2 Client Features

| Feature | Old | Reborn | Status |
|---------|-----|--------|--------|
| Login Screen | ✅ | ✅ | ✅ |
| Character Select | ✅ | ✅ | ✅ |
| Character Creation | ✅ | ✅ CharMakeDlg | ✅ P2.3 |
| In-Game UI | ✅ 29+ dialogs | ✅ 35 dialog types | ✅ |
| MiniMap | ✅ | ✅ MiniMapDlg | ✅ P2.2 |
| WorldMap | ✅ | ✅ WorldMapDlg | ✅ P2.2 |
| Chat System | ✅ Full | ✅ Basic | ⚠️ Basic |
| Party | ✅ Full | ✅ ECS-based | ✅ |
| Guild | ✅ Full | ✅ ECS-based | ✅ |
| Quest | ✅ Full | ✅ ECS-based | ✅ |
| Inventory | ✅ Full | ✅ ECS-based | ✅ |
| Skills | ✅ Full | ✅ ECS-based | ✅ |
| NPC Dialog | ✅ Full | ✅ Basic | ⚠️ Basic |
| Housing | ✅ Full | ✅ Basic | ⚠️ Basic |
| Siege | ✅ Full | ✅ Basic | ⚠️ Basic |
| Fishing | ✅ Full | ✅ Basic | ✅ |
| Cooking | ✅ Full | ✅ Basic | ✅ |
| Vehicle/Mount | ✅ Full | ✅ ECS-based | ✅ |
| Trading | ✅ Full | ✅ ECS-based | ✅ |
| PK/PvP | ✅ Full | ✅ PKManagerDlg | ✅ P2.5 |
| Tutorial | ✅ Full | ✅ TutorialDlg | ✅ P2.4 |
| Fade/Transition | ✅ | ✅ FadeDlg | ✅ P2.7 |
| Helper/Mascot | ✅ | ✅ HelperDlg | ✅ P2.6 |
| Gamepad Support | ❌ | ✅ InputSystem | ✅ P2.10 |
| Audio 3D | ✅ EAX | ✅ AudioManager | ✅ P2.12 |
| Config UI | ✅ Full | ✅ ConfigManager | ✅ P2.11 |

### 3.3 Server Features

| Feature | Old (LOC) | Reborn (LOC) | Status |
|---------|-----------|-------------|--------|
| Agent Server | 21.662 | 238 | ✅ P3.2 — login, session, char CRUD |
| Map Server | 117.534 | 622 | ✅ P3.3 — ECS simulation, grid |
| Distribute Server | 7.863 | 150 | ✅ P3.17 — channel, chat, routing |
| Combat System | ~11.000 | ~500 | ✅ P3.4 — full formula, damage, buff |
| Monster AI | ~5.000 | ~400 | ✅ P3.5 — 8 states + boss phases |
| Item/Inventory | ~11.047 | ~500 | ✅ P3.6 — loot, upgrade, equip |
| Quest System | ~5.000 | ~300 | ✅ P3.7 — state tracking, rewards |
| Party/Guild | ~5.000 | ~400 | ✅ P3.8 — XP share, guild bank, war |
| FSM Engine | ~10.000 (61 files) | ✅ | ✅ P3.15 — Lua quest/event scripting |
| Street Stall | ~5.000 | ✅ | ✅ P3.19 — player shops, browsing |
| Weather Server | ~2.000 | ✅ | ✅ P3.20 — dynamic weather server-side |
| Database Layer | ~4.000 | ~300 | ✅ P3.18 — SQLite pool, async |
| Housing | ~3.000 | ✅ | ✅ P3.9 — instance, furniture, farming |
| Siege | ~2.000 | ✅ | ✅ P3.10 — castle siege, flag, guild war |
| Dungeon | ~2.000 | ✅ | ✅ P3.11 — instance, boss, reward |
| Fishing/Cooking | ~1.000 | ✅ | ✅ P3.12 — mini-game, recipe, buff |
| Vehicle/Mount | ~1.000 | ✅ | ✅ P3.13 — mount, speed buff |
| Trading | ~2.000 | ✅ | ✅ P3.14 — player trade, consignment, mail |
| Trigger System | ~1.000 | ✅ | ✅ P3.16 — area/kill/item/time triggers |

---

## 4. Architecture

| Aspek | Old | Reborn |
|-------|-----|--------|
| **Bahasa** | C++98 (MSVC 7.1) | C++23 (Clang 19) |
| **Build** | Visual Studio .sln | CMake 4.0 + Ninja |
| **Dependencies** | Manual DLL/vendor | vcpkg manifest |
| **Graphics** | Direct3D 9 (Windows only) | bgfx (Metal/D3D12/Vulkan) |
| **Windowing** | Win32 API | GLFW |
| **Audio** | Miles Sound System (komersial) | miniaudio (open source) |
| **Physics** | None (D3D9 built-in) | Jolt Physics |
| **Networking** | IOCP (Windows) | Asio (cross-platform) |
| **Entity System** | Manual OOP | EnTT ECS |
| **Scripting** | Hardcoded C++ | LuaJIT + sol2 |
| **Serialization** | Binary .pak | flatbuffers |
| **UI** | Custom win32-like | bgfx + ImGui |
| **Math** | Custom VECTOR3/MATRIX4 | glm |
| **Logging** | OutputDebugString | spdlog + fmt |
| **CI/CD** | ❌ | ✅ GitHub Actions |

---

## 5. Phase Progress (8-Phase Roadmap)

| Phase | Status | Progress | Catatan |
|-------|--------|----------|---------|
| **P0: Asset** | ⚠️ Hampir | **75%** | Models/Anim/Audio/Maps 99-100% ✅, Textures 58% ⬅ bottleneck |
| **P1: Engine** | ✅ Selesai | **100%** | 13/13 tasks — geom, physics, network, scripting |
| **P2: Client** | ✅ Selesai | **100%** | 12/12 tasks — 35 dialogs, input, efek, audio 3D |
| **P3: Server** | ✅ Selesai | **100%** | 20/20 tasks — ECS combat, AI, quest, party, FSM, stall, weather |
| **P4: Content** | 🟡 ~25% | **~25%** | Data 58K+ records ✅, runtime ✅ fixed. Quest 12/504, boss 0%, economy 0% |
| **P5: Distribution** | 🟢 ~20% | **~20%** | 10 binary ✅, tools ready. Installer/updater/Windows ⬜ |
| **P6: Mobile** | ⬜ | **0%** | Belum dimulai |
| **P7: Polish** | ⬜ | **0%** | Belum dimulai |

---

## 6. Yang Old Punya, Reborn Tidak

### Critical Missing
| Item | Detail | Dampak |
|------|--------|--------|
| **Quest chains** | Old: ~500+ quests / Reborn: 12 quests | Content campaign sangat terbatas |
| **Boss mechanics** | 5 boss types + 34 field bosses — 0% implemented | No endgame PvE |
| **Economy balance** | Drop rates, exp curves, shop prices — all default | Progression tidak balance |
| **4.199 UI scripts** (.bin encrypted, 0% converted) | UI layouts harus hardcoded di C++ | Maintenance berat |
| **8.476 DDS textures** (58% coverage) | Belum dikonversi ke PNG | Visual variety kurang |
| **Effect scripts** | 4.138 .beff — 0% converted | Skill visual terbatas |

### Minor Missing
| Item | Dampak |
|------|--------|
| Windows build (MSVC) | Belum di-test di Windows |
| Automated testing (CI/CD) | 5 test tools ada tapi belum automated |
| Audio integration (full) | 978 files perlu verifikasi mapping |
| UI polish | Beberapa dialog belum seamless |

---

## 7. Yang Reborn Punya, Old Tidak

| Item | Keunggulan |
|------|-----------|
| **Cross-platform** (macOS + Windows) | Old Windows-only |
| **ECS Architecture** (EnTT) | Kode lebih terstruktur, performa lebih baik |
| **GPU Skinning** | Animasi lebih smooth |
| **Animation Blending** | Transisi animasi lebih natural |
| **Physics Engine** (Jolt) | Collision akurat, rigid body |
| **Scripting** (LuaJIT) | Quest/event bisa hot-reload |
| **Shadow Mapping** | Visual lebih modern |
| **Sky Dome** (day/night) | Atmospheric |
| **Post-processing shaders** | Bloom, blur, water |
| **Dear ImGui** | Debug tools, editor overlay |
| **vcpkg + CMake** | Dependency management modern |
| **GitHub Actions CI** | Build otomatis |
| **FlatBuffers** | Network serialization zero-copy |
| **spdlog logging** | Debugging jauh lebih mudah |

---

## 8. Coverage Graph

```
PHASE PROGRESS:
  P0 Asset          ████████████████████░░  75%  (textures 58% ⬅ bottleneck)
  P1 Engine         ██████████████████████ 100% ✅
  P2 Client         ██████████████████████ 100% ✅
  P3 Server         ██████████████████████ 100% ✅
  P4 Content        █████░░░░░░░░░░░░░░░░░  25%  (data ✅, runtime ✅, quest 12/504)
  P5 Distribution   ████░░░░░░░░░░░░░░░░░░  20%  (10 binary ✅)
  P6 Mobile         ░░░░░░░░░░░░░░░░░░░░░░   0%
  P7 Polish         ░░░░░░░░░░░░░░░░░░░░░░   0%
───
SOURCE CODE:        █░░░░░░░░░░░░░░░░░░░   7%
ENGINE:             ██░░░░░░░░░░░░░░░░░░  10%
CLIENT:             █████░░░░░░░░░░░░░░░  25%
SERVER (LOC):       █░░░░░░░░░░░░░░░░░░░   1%
ECS:                ████████████████████ 100%  (NEW)
───
ASSET COVERAGE:
  Textures          ███████░░░░░░░░░░░░░  58%
  Models            ████████████████████  99%
  Animations        ████████████████████  99%
  Audio             ████████████████████ 100%
  Heightmaps        ████████████████████ 100%
  Scene Maps        ████████████████████ 100%
  Char Defs         ██████████████████░░  94%
  Shaders           ████████████████████ 100%
───
OVERALL ESTIMATE:   ██████░░░░░░░░░░░░░░  35%
```

---

## 9. Kesimpulan

**Luna-Plus-Reborn adalah rewrite modern yang:**
- ✅ Framework Engine + Client + Server: **100%** selesai (P1-P3)
- ✅ Asset: models 99%, animasi 99%, audio 100%, maps 100%, shaders 100%
- ✅ Client: 35 dialog types ✅, MiniMap ✅, Character Creation ✅, Tutorial ✅, PK ✅, dll.
- ✅ Server: 20/20 tasks ✅ — ECS combat, AI, quest, party, guild, FSM, stall, weather
- ✅ Runtime DB connection: **FIXED** ✅ — 58K+ records siap di-load
- ❌ **Content integration masih ~25%** — quest chains 12/504, boss mechanics 0%, economy 0%
- ❌ **Textures masih 58%** (8.476 DDS belum dikonversi)
- ❌ **Server code size masih ~1% dari Old** (1K LOC vs 147K LOC)
- ❌ **4.199 UI scripts + 4.138 effect scripts** — 0% terkonversi
- ⬜ Windows build, automated testing, installer/updater — belum

**Status:** Playable single-player ✅, Multiplayer content masih terbatas ⬜
**Reborn unggul di teknologi (C++23, bgfx, ECS, Jolt, Lua). Old unggul di konten dan server maturity.
Overall estimated completion: ~35%**
