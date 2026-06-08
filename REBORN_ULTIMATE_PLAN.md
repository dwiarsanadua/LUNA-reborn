# 📗 BLUEPRINT FINAL — Luna-Plus-Reborn: The Ultimate Resurrection

> **Misi:** Membangkitkan Luna-Online-Plus sebagai game modern cross-platform (macOS Silicon + Windows) menggunakan teknologi paling mutakhir, dengan menggunakan 100% asset asli yang sudah dibedah total dari Luna-Plus-Old. Fondasi siap untuk ekspansi Android/iOS di masa depan.
>
> **Pendekatan:** Bukan porting. Bukan rewrite. **Reinkarnasi** — mengambil jiwa dan konten Luna-Plus-Old, membungkusnya dengan arsitektur dan teknologi paling modern.

---

## Daftar Isi

1. [Tech Stack Final](#1-tech-stack-final)
2. [Arsitektur Sistem](#2-arsitektur-sistem)
3. [Asset Pipeline — Bedah Total](#3-asset-pipeline--bedah-total)
4. [8 Phase Roadmap](#4-8-phase-roadmap)
5. [Target Platform](#5-target-platform)
6. [Estimasi Timeline](#6-estimasi-timeline)

---

## 1. Tech Stack Final

### Core Language & Build

| Teknologi | Versi | Fungsi | Status |
|-----------|-------|--------|--------|
| **C++23** | `-std=c++23` | Bahasa utama | ✅ Existing |
| **CMake** | 4.0+ | Build system | ✅ Existing |
| **Ninja** | 1.13+ | Build backend | ✅ Existing |
| **vcpkg** | — | Dependency management | ✅ Forge |
| **Clang 19+** | macOS | Compiler | ✅ |
| **MSVC 2022+** | Windows | Compiler | ⏳ |
| **GitHub Actions** | — | CI/CD | ✅ Forge |

### Rendering & Graphics

| Teknologi | macOS | Windows | Android/iOS | Status |
|-----------|-------|---------|-------------|--------|
| **bgfx** | ✅ Metal | ✅ D3D12/Vulkan | ✅ Vulkan/GLES | ✅ Existing |
| **GLFW** | ✅ Cocoa | ✅ Win32 | ❌ → SDL3 nanti | ✅ Existing |
| **glm** | ✅ | ✅ | ✅ | ✅ Existing |
| **Dear ImGui** | ✅ | ✅ | ❌ | ✅ Added |

### 3D, Physics & Audio

| Teknologi | Fungsi | Status |
|-----------|--------|--------|
| **Assimp** | Model loading (glTF/OBJ) | ✅ Existing |
| **AnmParser** | Legacy .ANM parsing | ✅ Existing |
| **Jolt Physics** | Physics engine | ✅ Titan |
| **meshoptimizer** | Mesh optimization | ⏳ |
| **miniaudio** | Audio playback | ✅ Existing |

### ECS, Scripting & Network

| Teknologi | Fungsi | Status |
|-----------|--------|--------|
| **EnTT 4.x** | Entity Component System | ✅ Existing |
| **sol2/LuaJIT** | Scripting engine | ✅ Nexus |
| **flatbuffers** | Network serialization | ✅ Existing |
| **Asio** | Async TCP networking | ✅ Nexus |
| **AES-256-GCM** | Packet encryption | ✅ Existing |

### Data & Serialization

| Teknologi | Fungsi | Status |
|-----------|--------|--------|
| **SQLite3** | Database | ✅ Existing |
| **nlohmann/json** | JSON parsing | ⏳ |
| **yaml-cpp** | YAML config | ✅ Existing |
| **spdlog + fmt** | Logging | ✅ Existing |

---

## 2. Arsitektur Sistem

### Layer Architecture Final

```
┌──────────────────────────────────────────────────────────────────────┐
│                         CLIENT LAYER                                  │
│  ┌────────────┐ ┌──────────┐ ┌──────────┐ ┌──────────────────────┐   │
│  │ Screens    │ │ Dialogs  │ │ Widgets  │ │ Game Objects         │   │
│  │ Login,     │ │ 29 dialog│ │ 11 types │ │ Hero, Monster, NPC,  │   │
│  │ CharSelect,│ │ types    │ │          │ │ Pet, Vehicle, dll    │   │
│  │ Game, Load │ │          │ │          │ │                      │   │
│  └─────┬──────┘ └────┬─────┘ └────┬─────┘ └──────────┬───────────┘   │
│        └─────────────┴─────┬──────┴────────────────────┘              │
│                            │                                         │
│              ┌─────────────┴──────────────┐                          │
│              │     UIRenderer + Skin       │                          │
│              │     (bgfx TransientVB)      │                          │
│              └─────────────┬──────────────┘                          │
├────────────────────────────┼──────────────────────────────────────────┤
│                  GAME ENGINE LAYER                                    │
│  ┌───────────┐ ┌──────────┐ ┌─────────┐ ┌──────────┐ ┌──────────┐   │
│  │ Engine    │ │ Engine   │ │ Engine  │ │ Engine   │ │ Engine   │   │
│  │ Camera    │ │ Map      │ │ Sky     │ │ Effect   │ │ Sound    │   │
│  │ (orbit)   │ │(terrain, │ │(d/n     │ │(damage,  │ │(BGM, SFX,│   │
│  │           │ │ props)   │ │ cycle)  │ │ particles│ │ 3D)      │   │
│  └─────┬─────┘ └────┬─────┘ └────┬────┘ └────┬─────┘ └────┬─────┘   │
│        └────────────┴──────┬─────┴────────────┴────────────┘         │
│                            │                                         │
├────────────────────────────┼──────────────────────────────────────────┤
│                   ENGINE CORE LAYER                                   │
│  ┌───────────┐ ┌──────────┐ ┌──────────┐ ┌─────────┐ ┌──────────┐   │
│  │ Renderer  │ │ Geometry │ │ Physics  │ │ Resource│ │ Network  │   │
│  │ (bgfx)    │ │ (Model,  │ │ (Jolt)   │ │ (Cache) │ │ (Asio)   │   │
│  │           │ │  Anim)   │ │          │ │         │ │          │   │
│  └─────┬─────┘ └────┬─────┘ └────┬─────┘ └────┬────┘ └────┬─────┘   │
│        └────────────┴──────┬─────┴────────────┴────────────┘         │
│                            │                                         │
│  ┌─────────────────────────┴──────────────────────────────────────┐  │
│  │              bgfx (Metal / D3D12 / Vulkan)                      │  │
│  │   Views: Shadow→Clear→Terrain→Props→Char→Particle→UI→Debug     │  │
│  └─────────────────────────────────────────────────────────────────┘  │
├──────────────────────────────────────────────────────────────────────┤
│                       SERVER LAYER                                    │
│  ┌──────────────────┐ ┌──────────────────┐ ┌──────────────────────┐  │
│  │  Agent Server    │ │   Map Server     │ │ Distribute Server   │  │
│  │  - Login/Auth    │ │ - ECS Simulation │ │ - Channel routing   │  │
│  │  - Session mgmt  │ │ - Combat/AI      │ │ - Chat (world/party)│  │
│  │  - Character CRUD│ │ - Item/Inventory │ │ - Party matching    │  │
│  │  - Map assign    │ │ - Quest/Party    │ │ - Server list       │  │
│  │                  │ │ - Housing/Siege  │ │                     │  │
│  │                  │ │ - Dungeon/Fishing│ │                     │  │
│  │                  │ │ - Monster/NPC    │ │                     │  │
│  └────────┬─────────┘ └────────┬─────────┘ └──────────┬──────────┘  │
│           └───────────────────┼───────────────────────┘              │
│                               │                                      │
│  ┌────────────────────────────┴──────────────────────────────────┐  │
│  │               EnTT ECS (server-side)                          │  │
│  │   18 Components + 19 Systems                                  │  │
│  └───────────────────────────────────────────────────────────────┘  │
│                               │                                      │
│  ┌────────────────────────────┴──────────────────────────────────┐  │
│  │  Database Layer (SQLite via connection pooling)               │  │
│  └───────────────────────────────────────────────────────────────┘  │
├──────────────────────────────────────────────────────────────────────┤
│                     NETWORK LAYER                                     │
│  ┌───────────────────────────────────────────────────────────────┐  │
│  │  Asio async TCP · flatbuffers · AES-256-GCM · CRC32          │  │
│  │  Packet: [magic:4][length:4][type:2][seq:4][crc:4][payload]  │  │
│  └───────────────────────────────────────────────────────────────┘  │
└──────────────────────────────────────────────────────────────────────┘
```

### Dependency Chain

```
Phase 0 ─── Asset Pipeline ───→ 100% asset siap
               │
               ▼
Phase 1 ─── Engine Foundation ──→ Model, Animation, Physics, Network
               │
               ▼
Phase 2 ─── Client Complete ──→ Semua fitur client Old
               │
               ▼
Phase 3 ─── Server Complete ──→ Semua game logic server
               │
               ▼
Phase 4 ─── Content Integrate ──→ Semua map, monster, quest, item, skill
               │
               ▼
Phase 5 ─── Polish & Release ──→ macOS + Windows release, mobile prep
```

---

## 3. Asset Pipeline — Bedah Total

### Sumber Asset: Luna-Plus-Old

| Kategori | Jumlah di Old | Status di Reborn (%) | Target |
|----------|--------------|---------------------|--------|
| **Textures** | 20.228 file (.dds/.png/.tif/.tga) | 353 (2%) | 20.000+ |
| **Models 3D** | 8.832 file (.mod/.obj) | 809 (9%) | 8.800+ |
| **Animations** | 7.105 file (.anm) | 7.033 (99%) | 7.100 |
| **Audio** | 978 file (.wav/.mp3) | 978 (100%) | 978 ✅ |
| **Character Defs** | 1.542 file (.chx) | 1.457 (94%) | 1.500 |
| **Heightmaps** | 51 file (.hfl) | 34 (67%) | 51 |
| **Scene Maps** | 53 file (.map) | 53 (100%) | 53 ✅ |
| **UI Scripts** | 4.199 file (.bin) | 262 (6%) | ~300 |
| **Databases** | 3 file (.db) | 3 (100%) | 3 ✅ |
| **Fonts** | 1 file (.ttf) | 1 (100%) | 1 ✅ |
| **Shaders** | 5 file (.psh/.vsh) | 17 .sc (100%) | 17 ✅ |

### Konversi Pipeline — Batch Command

Semua tools sudah siap di `tools/asset_pipeline/`. Cukup jalankan:

```bash
cd Luna-Plus-Reborn

# Batch 1: Textures (19.875 file) — ~3-4 jam
python3 tools/asset_pipeline/convert_textures.py \
  --input ../Luna-Plus-Old/LEGACY_ASSETS/legacy_unpacked/raw_originals/assets/ \
  --output assets/textures/ \
  --workers 8

# Batch 2: Models (8.000 file) — ~2-3 jam
python3 tools/asset_pipeline/convert_models.py \
  --input ../Luna-Plus-Old/LEGACY_ASSETS/legacy_unpacked/raw_originals/assets/unpacked/ \
  --output assets/models/ \
  --optimize

# Batch 3: Heightmaps (17 file) — ~10 menit
python3 tools/asset_pipeline/convert_heightmaps.py \
  --input ../Luna-Plus-Old/LEGACY_ASSETS/legacy_unpacked/raw_originals/assets/unpacked/map/ \
  --output assets/maps/
```

### Struktur Asset Final

```
assets/
├── textures/ (20.000+ .png)
│   ├── character/     — karakter, costume, armor, hair, face
│   ├── monster/       — monster textures
│   ├── map/           — terrain tiles, ground, rock, grass
│   ├── ui/            — icons, buttons, windows, frames
│   ├── item/          — item icons
│   └── effect/        — particles, smoke, light, magic
│
├── models/ (8.800+ .glb)
│   ├── character/     — body, hair, costume parts
│   ├── monster/       — 340+ monster types
│   ├── npc/           — 115+ NPCs
│   ├── pet/           — pet models
│   ├── vehicle/       — mounts, vehicles
│   ├── prop/          — environment props
│   └── weapon/        — weapons, shields
│
├── animations/ (7.100+ .anm.json)
├── audio/ (978 file)
│   ├── BGM/           — 48 map tracks
│   ├── Character/     — voices, footsteps
│   ├── Monster/       — monster sounds
│   ├── Weapon/        — weapon swings
│   └── Interface/     — UI sounds
│
├── maps/ (51 scene.json + 51 .hgt)
├── characters/ (1.500+ .json)
├── shaders/ (17 .sc + 9 .bin)
├── fonts/ (1 .ttf)
├── data/ (3 .db)
├── interface/ (255 .txt)
└── scripts/ (262 .bin.txt)
```

---

## 4. 8 Phase Roadmap

### Phase 0 — Asset Complete (2-3 minggu)

> **Tujuan:** 100% asset Old tersedia di Reborn. Pipeline siap untuk regenerasi kapan saja.

| # | Task | Detail | Tools |
|---|------|--------|-------|
| P0.1 | **Batch Convert Textures** | 19.875 DDS/TIF/TGA → PNG (8 workers, multiprocessing) | `convert_textures.py` |
| P0.2 | **Batch Convert Models** | 8.000 MOD/OBJ → GLB (optimasi mesh) | `convert_models.py` |
| P0.3 | **Complete Heightmaps** | 17 HFL → HGT | `convert_heightmaps.py` |
| P0.4 | **Complete Character Defs** | 85 .chx → .json | `convert_chardefs.py` |
| P0.5 | **Validation & Inventory** | Verifikasi completeness, generate `INVENTORY.json` | `validate_assets.py` |

**Deliverable:** 100% asset coverage. `python3 run.py --all` = full regenerate.

---

### Phase 1 — Engine Complete (3-4 minggu)

> **Tujuan:** Semua engine system berfungsi penuh — geometry, animation, physics, resource, network, scripting.

| # | Task | File | Keterangan |
|---|------|------|------------|
| P1.1 | **Geometry Engine** | `engine/gx_geom/Model.h/.cpp` | Assimp-based model loader. ✅ Done |
| P1.2 | **Animation Runtime** | `engine/gx_geom/AnimationSystem.h/.cpp` | JSON + ANM keyframe playback. ✅ Done |
| P1.3 | **Animation Blending** | `engine/gx_geom/AnimationSystem.cpp` | Cross-fade, additive blend. ✅ Done |
| P1.4 | **MeshObject GPU** | `engine/gx_geom/MeshObject.h/.cpp` | bgfx VB/IB wrapper. ✅ Done |
| P1.5 | **Skeleton** | `engine/gx_geom/Skeleton.h/.cpp` | Bone hierarchy. ✅ Done |
| P1.6 | **Physics Engine** | `engine/physics/PhysicsWorld.h/.cpp` | Jolt wrapper. ✅ Done |
| P1.7 | **Resource Cache** | `engine/resource/ResourceCache.h/.cpp` | Thread-safe template. ✅ Done |
| P1.8 | **Network Layer** | `engine/network/NetworkLayer.h/.cpp` | Asio async TCP. ✅ Done |
| P1.9 | **Scripting Engine** | `engine/scripting/LuaEngine.h/.cpp` | sol2/LuaJIT. ✅ Done |
| P1.10 | **Integrate Collision** | `client/engine/CollisionSystem.hpp` | Jolt integration. ✅ Done |
| P1.11 | **Integrate CharacterRenderer** | `client/rendering/CharacterRenderer.cpp` | Hubungkan Model + AnimationSystem. ⬜ |
| P1.12 | **Integrate Physics** | `client/gameobjects/Hero.cpp`, `Monster.cpp` | PhysicsWorld ke game object. ⬜ |
| P1.13 | **Integrate Network** | `server/agent/AgentServer.cpp` | NetworkLayer ke server. ⬜ |

**Deliverable:** Karakter bisa jalan dengan animasi, physics collision, network layer siap.

---

### Phase 2 — Client Feature Complete (5-6 minggu)

> **Tujuan:** Semua fitur client Old tersedia di Reborn dengan UI modern.

| # | Task | File | Fitur |
|---|------|------|-------|
| P2.1 | **Complete UI Dialogs** | `client/ui/dialogs/*` | Polish 29 dialog: tooltip, drag-drop, animasi, suara |
| P2.2 | **MiniMap / WorldMap** | `MiniMapDlg`, `WorldMapDlg` | Terrain minimap, entity dots, fog of war |
| P2.3 | **Character Creation** | `CharMakeDlg` | Class select, appearance, 3D preview |
| P2.4 | **Tutorial System** | `TutorialDlg`, `TutorialManager` | Event-triggered popup, step tracking |
| P2.5 | **PK / PvP** | `PKManager`, `PKLootingDlg` | PK flag, penalty, looting |
| P2.6 | **Helper / Mascot** | `HelperDlg` | Companion NPC |
| P2.7 | **Fade / Transition** | `FadeDlg` | Screen fade in/out |
| P2.8 | **Effect System** | `EffectManager.cpp` | 7 legacy unit types: Particle, Billboard, Light, Sound, CameraShake, Animation, Model |
| P2.9 | **Particle System** | `ParticleRenderer.cpp` | GPU particles, emitters, attractors |
| P2.10 | **Input: Gamepad** | `InputSystem.cpp` | GLFW gamepad API, analog → camera |
| P2.11 | **Config: Settings UI** | `ConfigManager.cpp` | Video/audio/gameplay settings |
| P2.12 | **Audio 3D** | `AudioManager.cpp` | SetSoundPosition, distance attenuation, doppler |

**Deliverable:** Client paritas fitur dengan Old. UI lengkap, efek, input.

---

### Phase 3 — Server Complete (8-10 minggu)

> **Tujuan:** Server game fully functional — semua game logic Old diimplementasikan dengan ECS modern + scripting.

| # | Task | Files | Fitur |
|---|------|-------|-------|
| P3.1 | **Network Layer Integration** | `server/*/` | Asio TCP + flatbuffers routing |
| P3.2 | **Agent Server** | `AgentServer.h/.cpp` | Login, session, character CRUD |
| P3.3 | **Map Server Core** | `MapServer.h/.cpp` | ECS simulation, entity mgmt, grid |
| P3.4 | **Combat System (Server)** | `systems/CombatSystem.cpp` | Damage, skills, buff/debuff, status, XP |
| P3.5 | **AI System (Server)** | `systems/AISystem.cpp` | Monster AI: patrol → aggro → chase → attack → flee → respawn. Boss phases |
| P3.6 | **Item & Inventory** | `systems/ItemSystem.cpp` | Generation, inventory, equipment, drop, upgrade |
| P3.7 | **Quest System (Server)** | `systems/QuestSystem.cpp` | State tracking, conditions, rewards |
| P3.8 | **Party & Guild** | — | Formation, XP share, guild bank, guild war |
| P3.9 | **Housing** | — | Instance, furniture, farming |
| P3.10 | **Siege** | — | Castle siege, flag, guild war |
| P3.11 | **Dungeon** | — | Instance, boss, reward |
| P3.12 | **Fishing & Cooking** | — | Mini-game, recipe, buff food |
| P3.13 | **Vehicle / Mount** | — | Mount system, speed buff |
| P3.14 | **Trading** | — | Player trade, consignment, mail |
| P3.15 | **FSM Engine** | — | Quest/event scripting via Lua |
| P3.16 | **Trigger System** | — | Event triggers: area, kill, item, time |
| P3.17 | **Distribute Server** | `DistributeServer.h/.cpp` | Channel, chat, party matching |
| P3.18 | **Database Layer** | `Database.h/.cpp` | SQLite pool, async queries |
| P3.19 | **Street Stall** | — | Player shops, browsing |
| P3.20 | **Weather System** | — | Dynamic weather on server |

**Deliverable:** Server paritas fitur dengan Old. Client bisa login, combat, quest, party, guild, PvP.

---

### Phase 4 — Content Complete (4-5 minggu)

> **Tujuan:** Semua konten game dari Old tersedia — map, monster, NPC, quest, item, skill.

| # | Task | Detail |
|---|------|--------|
| P4.1 | **All 54 Maps Playable** | Load terrain + props + NPC + monster spawns. Pathfinding per map |
| P4.2 | **All 340+ Monsters** | AI, loot table, spawn points, stats dari `game_data_legacy.db` |
| P4.3 | **All 115+ NPCs** | Dialog, quest, shop, teleport |
| P4.4 | **All Quests** | Quest chains dari `[CC]Quest/` — kondisi, event, reward |
| P4.5 | **All Skills** | Active + buff per class. Skill tree dari `[CC]Skill/` |
| P4.6 | **All Items** | Equipment, consumables, materials, recipe dari DB |
| P4.7 | **Boss Encounters** | Field bosses + dungeon bosses dengan mechanics |
| P4.8 | **Economy Balance** | Drop rates, shop prices, crafting costs |

**Deliverable:** Game lengkap dari level 1 sampai end-game. Semua konten original.

---

### Phase 5 — Distribution (3-4 minggu)

> **Tujuan:** Game siap didistribusikan dengan installer, auto-updater, dan tooling lengkap.

| # | Task | Detail |
|---|------|--------|
| P5.1 | **Performance Optimization** | GPU instancing, occlusion culling, texture atlas, audio streaming |
| P5.2 | **Map Editor** | ImGui-based editor: terrain, object, NPC/monster placement |
| P5.3 | **GM Tools** | Admin panel: player, item, quest debugging |
| P5.4 | **Monitor Server** | Live monitoring: player count, metrics, alerts |
| P5.5 | **Auto-Updater** | Differential patching via SHA-256 manifest |
| P5.6 | **macOS DMG** | Code signing, notarization, Sparkle auto-update |
| P5.7 | **Windows Installer** | MSI/NSIS installer, signed binaries |
| P5.8 | **Player Guide** | Documentation, FAQ, known issues |

**Deliverable:** Game siap rilis publik.

---

### Phase 6 — Future: Android/iOS Prep (3-4 minggu, terpisah)

> **Tujuan:** Fondasi untuk porting ke mobile — tanpa menjalankan di mobile saat ini.

| # | Task | Detail |
|---|------|--------|
| P6.1 | **Migrasi GLFW → SDL3** | SDL3 support touch input, multiple windows, game controller |
| P6.2 | **Touch Input Layer** | Gesture recognition, virtual joystick, tap-to-click |
| P6.3 | **UI Scaling** | Dynamic DPI scaling untuk berbagai ukuran layar |
| P6.4 | **Mobile Profile** | GPU profile untuk mobile (reduce draw calls, texture size) |
| P6.5 | **Android Build Target** | CMake + NDK cross-compile. bgfx Vulkan/GLES backend |
| P6.6 | **iOS Build Target** | Xcode project. bgfx Metal backend |

**Deliverable:** Kode siap di-compile untuk Android/iOS. Testing lanjutan di masa depan.

---

### Phase 7 — Final Polish (2-3 minggu)

> **Tujuan:** Quality of life, fitur tambahan yang tidak ada di Old.

| # | Task | Detail |
|---|------|--------|
| P7.1 | **Controller Support** | Full gamepad UI navigation |
| P7.2 | **Achievements** | Achievement system dengan rewards |
| P7.3 | **Photo Mode** | Free camera, filters, screenshot |
| P7.4 | **Localization** | Multi-language support |
| P7.5 | **Bug Fixing** | Dari hasil testing publik |

---

## 5. Target Platform

| Platform | Phase | Graphics | Status |
|----------|-------|----------|--------|
| **macOS Apple Silicon** | P0 (sekarang) | bgfx + Metal | ✅ **Active** |
| **macOS Intel** | P0 (sekarang) | bgfx + Metal | ✅ **Supported** |
| **Windows 10/11 x64** | P1 | bgfx + D3D12 | ⚠️ Perlu setup |
| **Linux** | Future | bgfx + Vulkan | ⏳ |
| **Android** | P6 | bgfx + Vulkan/GLES | ⏳ Future |
| **iOS** | P6 | bgfx + Metal | ⏳ Future |

---

## 6. Estimasi Timeline

### 8 Phase — 12 Bulan Total

```
Phase 0: Asset Complete        ████████░░░░░░░░░░░░░░░░░░░░░░  2-3 minggu
Phase 1: Engine Complete       ░░░░████████████░░░░░░░░░░░░░░  3-4 minggu
Phase 2: Client Complete       ░░░░░░░░░░████████████████░░░░  5-6 minggu
Phase 3: Server Complete       ░░░░░░░░░░░░░░░░░░████████████  8-10 minggu
Phase 4: Content Complete      ░░░░░░░░░░░░░░░░░░░░░░░░█████  4-5 minggu
Phase 5: Distribution          ░░░░░░░░░░░░░░░░░░░░░░░░░░░░██  3-4 minggu
Phase 6: Mobile Prep           ░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░  (terpisah)
Phase 7: Final Polish          ░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░  2-3 minggu

Bulan:  1   2   3   4   5   6   7   8   9  10  11  12
```

### Milestone

| M | Waktu | Capaian |
|---|-------|---------|
| **M0** | Bulan 1 | 100% asset siap. Pipeline bisa regenerate kapan saja. |
| **M1** | Bulan 2 | Engine complete. Karakter dengan animasi, physics, network layer siap. |
| **M2** | Bulan 3-4 | Client alpha. Login → character select → masuk game → move → chat → basic combat. |
| **M3** | Bulan 6 | Server alpha. Multiplayer: party, quest, inventory, NPC. Client-server playable. |
| **M4** | Bulan 8 | Content beta. Semua map, monster, quest, skill, item. Level 1-100 playable. |
| **M5** | Bulan 10 | Release candidate. Semua fitur Old available. macOS + Windows. |

---

## Ringkasan: Yang SUDAH vs yang PERLU

### ✅ Sudan Selesai (Tidak perlu diulang)

| Area | Komponen |
|------|----------|
| **Engine Rendering** | RenderDevice, Texture, Shader, VertexBuffer, Font, Sprite3D — bgfx wrapper |
| **Engine Geometry** | Model, MeshObject, AnimationSystem, Skeleton — loading + runtime |
| **Engine Physics** | PhysicsWorld — Jolt Physics wrapper |
| **Engine Resource** | ResourceCache — thread-safe cache |
| **Engine Network** | NetworkLayer — Asio async TCP |
| **Engine Scripting** | LuaEngine — sol2/LuaJIT wrapper |
| **Client Rendering** | SceneRenderer, TerrainRenderer, CharacterRenderer, UIRenderer, WorldRenderer, PropRenderer, ParticleRenderer |
| **Client UI Framework** | Window, Widget (11 types), ScreenManager, SkinManager, UiScriptParser |
| **Client UI Dialogs** | 29 dialog types (Inventory, Skill, Quest, NPC, Chat, Party, Guild, dll.) |
| **Client UI Screens** | LoginScreen, CharSelectScreen, GameScreen, LoadingScreen |
| **Client Game Objects** | Hero, Monster, NPC, Pet, BuffSystem, ProjectileSystem, TriggerSystem, dll. |
| **Client Audio** | AudioManager, AmbientSystem, AnimationSfxSync |
| **Client Effects** | EffectManager, EftParser, WeatherSystem |
| **Client Input** | InputSystem, Keyboard, Mouse, MouseCursor, UserInput |
| **Client Config** | ConfigManager, KeyBindings |
| **Client Network** | NetworkClient, PacketCrypto, Launcher |
| **Client Engine** | EngineCamera, EngineMap, EngineSky, GraphicEngine, ServerAuthMovement, CollisionSystem |
| **Server Agent** | AgentServer — login, session, character management |
| **Server Map** | MapServer — ECS world, systems (Combat, AI, Item, Quest, Movement) |
| **Server Distribute** | DistributeServer — channel, chat, routing |
| **Server Database** | Database — SQLite pool |
| **ECS** | 18 Components + 19 Systems |
| **Shaders** | 17 .sc + 9 .bin (default, lit, unlit, terrain, UI, sky, skinned, post-process) |
| **Build System** | CMake + Ninja + vcpkg + CI/CD |
| **Tools** | Asset pipeline scripts, converters, 311 flatbuffers schemas |
| **Databases** | game_data_legacy.db, luna_char.db, luna_member.db |
| **Asset Audio** | 978 file (100%) |
| **Asset Animations** | 7.033 .anm.json (99%) |
| **Asset Character Defs** | 1.457 .json (94%) |
| **Asset Scene Maps** | 53 .json (100%) |
| **Asset Fonts** | 1 .ttf (100%) |

### ⬜ Perlu Dikerjakan

| Prioritas | Area | Detail |
|-----------|------|--------|
| 🔴 **P0** | **Asset Conversion** | 19.875 textures, 8.000 models, 17 heightmaps |
| 🔴 **P1** | **Engine Integration** | CharacterRenderer → Model, Hero → Physics, Server → Network |
| 🔴 **P2** | **Client Polish** | 7 missing dialogs, effect units, gamepad, settings UI, audio 3D |
| 🔴 **P3** | **Server Complete** | Party/Guild, Housing, Siege, Dungeon, Trading, FSM, Trigger, Weather, StreetStall |
| 🟡 **P4** | **Content** | All 54 maps, 340 monsters, 115 NPCs, quests, skills, items |
| 🟢 **P5** | **Distribution** | Map editor, GM tools, monitoring, auto-updater, installer |
| ⏳ **P6** | **Mobile Prep** | SDL3 migration, touch input, UI scaling |
| 🟢 **P7** | **Polish** | Controller, achievements, photo mode, localization |

---

*"Kita tidak mem-porting kode. Kita membangkitkan jiwa Luna dengan teknologi masa depan."*
