# 📗 BLUEPRINT — LUNA Plus Reborn: The Ultimate Plan

> **Misi:** Membangkitkan LUNA Online Plus sebagai game modern cross-platform (macOS Silicon + Windows) menggunakan teknologi paling mutakhir, dengan fondasi untuk ekspansi ke Android/iOS di masa depan.
>
> **Pendekatan:** Bukan porting. Bukan rewrite ulang. Tapi **reinkarnasi** — mengambil jiwa dan konten Luna-Plus-Old, membungkusnya dengan arsitektur dan teknologi paling modern.

---

## Daftar Isi

1. [Tech Stack Ultimate](#1-tech-stack-ultimate)
2. [Arsitektur Sistem](#2-arsitektur-sistem)
3. [Asset Pipeline](#3-asset-pipeline)
4. [Roadmap 5 Phases](#4-roadmap-5-phases)
5. [Target Platform & Build System](#5-target-platform--build-system)
6. [Estimasi Timeline](#6-estimasi-timeline)

---

## 1. Tech Stack Ultimate

### Core Language & Build

| Teknologi | Alasan |
|-----------|--------|
| **C++23** (`-std=c++23`) | `std::expected`, `std::print`, `std::mdspan`, `import std;` modules, better constexpr, stacktrace |
| **CMake 4.0+** | `FetchContent` + presets + toolchain files |
| **Ninja** | Build tercepat untuk semua platform |
| **vcpkg** | Manifest mode (`vcpkg.json`) — dependency management cross-platform yang mature |
| **Clang 19+ / MSVC 2022+** | Compiler terbaru untuk C++23 |
| **GitHub Actions** | CI/CD untuk macOS + Windows, future Android/iOS |

### Rendering & Graphics

| Teknologi | Platform | Alasan |
|-----------|----------|--------|
| **bgfx** (existing) | ✅ macOS (Metal) / ✅ Windows (D3D12/Vulkan) / ✅ Android (Vulkan/GLES) / ✅ iOS (Metal) | **SUDAH. Dipertahankan.** Satu API untuk semua platform — renderer abstraction paling mature di C++ |
| **GLFW** (existing) | ✅ macOS / ✅ Windows | Dipertahankan untuk desktop. Nanti migrasi ke **SDL3** untuk mobile support |
| **glm** (existing) | ✅ All | Dipertahankan, bertahap migrasi ke math custom |
| **Dear ImGui** (planned) | ✅ All | Debug tools, editor overlay, developer console |

### 3D & Physics

| Teknologi | Alasan |
|-----------|--------|
| **Assimp** (existing) | Load glTF/OBJ — standard industry |
| **AnmParser** (existing) | Parser legacy .ANM animation → runtime |
| **Jolt Physics** (planned) | Cross-platform, performa tinggi, digunakan di Horizon Forbidden West |
| **meshoptimizer** (new) | Optimasi mesh: LOD generation, vertex cache, quantize |

### ECS & Game Logic

| Teknologi | Alasan |
|-----------|--------|
| **EnTT 4.x** (existing, upgrade) | ECS terbaik di C++. Upgrade ke versi terbaru untuk performance improv |
| **sol2** / **LuaJIT** (new) | Scripting untuk quest, event, AI logic — hot-reloadable |
| **flatbuffers** (existing) | Networking protocol — zero-copy serialization |

### Audio

| Teknologi | Alasan |
|-----------|--------|
| **miniaudio** (existing) | Single-file, cross-platform, semua format. **Dipertahankan.** |

### Networking

| Teknologi | Alasan |
|-----------|--------|
| **Asio** (replace libuv) | Standar C++ (Boost.Asio atau standalone Asio). Cross-platform, support coroutine C++20 |
| **flatbuffers** (existing) | Packet protocol — pertahankan |
| **AES-256-GCM** (existing) | Packet crypto — pertahankan |

### Data & Serialization

| Teknologi | Alasan |
|-----------|--------|
| **SQLite3** (existing) | Database — pertahankan |
| **nlohmann/json** (new) | JSON parsing untuk config, asset metadata, map data |
| **yaml-cpp** (existing) | YAML config/server config |

### Tooling

| Teknologi | Alasan |
|-----------|--------|
| **Python 3.12+** | Asset pipeline scripting |
| **Pillow** | Batch konversi texture DDS/TIF/TGA → PNG/KTX2 |
| **assimp CLI** | Batch konversi .mod/.obj → .glb |
| **shaderc** (bgfx) | Kompilasi shader .sc → .bin |
| **flatc** (flatbuffers) | Generate C++ dari .fbs schema |

---

## 2. Arsitektur Sistem

### Layer Architecture

```
┌─────────────────────────────────────────────────────────────────┐
│                      CLIENT LAYER                               │
│  ┌──────────┐ ┌──────────┐ ┌──────────┐ ┌──────────────────┐   │
│  │ Screens  │ │ Dialogs  │ │ Widgets  │ │   Game Objects   │   │
│  │ (Login,  │ │ (30+     │ │ (Button, │ │ (Hero, Monster,  │   │
│  │  CharSel,│ │  types)  │ │  Grid,   │ │  NPC, Pet, dll)  │   │
│  │  Game,   │ │          │ │  Tab,..) │ │                  │   │
│  │  Load)   │ │          │ │          │ │                  │   │
│  └────┬─────┘ └────┬─────┘ └────┬─────┘ └────────┬─────────┘   │
│       └────────────┴─────┬──────┴─────────────────┘             │
│                          │                                      │
│              ┌───────────┴─────────────┐                        │
│              │     UIRenderer + Skin    │                        │
│              │     (bgfx TransientVB)   │                        │
│              └───────────┬─────────────┘                        │
├──────────────────────────┼──────────────────────────────────────┤
│              GAME ENGINE LAYER         │                        │
│  ┌──────────┐┌──────────┐┌───────────┐┌──────────┐┌─────────┐  │
│  │ Engine   ││ Engine   ││ Engine    ││ Engine   ││ Engine  │  │
│  │ Camera   ││ Map      ││ Sky       ││ Effect   ││ Sound   │  │
│  │ (orbit)  ││(terrain, ││ (day/night││ (damage, ││ (BGM,   │  │
│  │          ││ props)   ││  cycle)   ││  shake,  ││  SFX,   │  │
│  │          ││          ││           ││  legacy) ││ 3D)     │  │
│  └────┬─────┘└────┬─────┘└─────┬─────┘└────┬─────┘└────┬────┘  │
│       └───────┬───┴──────┬─────┴────────────┴───────────┘      │
│               │          │                                      │
├───────────────┼──────────┼──────────────────────────────────────┤
│     RENDERER LAYER       │                                      │
│  ┌──────────┐┌─────────┐┌──────────┐┌─────────┐┌──────────┐   │
│  │ Terrain  ││ Char    ││ Prop     ││ Particle││ Sky      │   │
│  │ Renderer ││ Renderer││ Renderer ││Renderer ││ Dome     │   │
│  │ (LOD)    ││(skinned)││ (OBJ)    ││(billbrd)││(daycycl) │   │
│  └────┬─────┘└────┬────┘└────┬─────┘└────┬────┘└────┬─────┘   │
│       └─────┬─────┴──────────┴────────────┴──────────┘         │
│             │                                                    │
│  ┌──────────┴────────────────────────────────────────────────┐  │
│  │              bgfx (Metal / D3D12 / Vulkan)                │  │
│  │   7 Views: Clear → Terrain → Props → Char → Part → UI    │  │
│  └───────────────────────────────────────────────────────────┘  │
├─────────────────────────────────────────────────────────────────┤
│                      SERVER LAYER                               │
│  ┌────────────────┐ ┌────────────────┐ ┌──────────────────┐    │
│  │  Agent Server  │ │   Map Server   │ │ Distribute Server│    │
│  │ (auth, session,│ │ (game world,   │ │ (channel, chat,  │    │
│  │  login, char)  │ │  combat, AI,   │ │  party, guild)   │    │
│  │                │ │  quest, item)  │ │                  │    │
│  └───────┬────────┘ └───────┬────────┘ └────────┬─────────┘    │
│          └──────────────────┼───────────────────┘               │
│                             │                                   │
│  ┌──────────────────────────┴─────────────────────────────┐    │
│  │              EnTT ECS (server-side)                     │    │
│  │  17 Components: Transform, Movement, CharacterStats,   │    │
│  │  CombatState, Inventory, SkillBook, AIComponent, ...   │    │
│  │  19 Systems: MovementSystem, CombatSystem, AISystem,   │    │
│  │  SkillSystem, ItemSystem, QuestSystem, ...            │    │
│  └────────────────────────────────────────────────────────┘    │
│                             │                                   │
│  ┌──────────────────────────┴─────────────────────────────┐    │
│  │  Database Layer (SQLite via DBThread pattern)          │    │
│  │  game_data.db + luna_char.db + luna_member.db          │    │
│  └────────────────────────────────────────────────────────┘    │
├─────────────────────────────────────────────────────────────────┤
│                  NETWORK LAYER (TCP + flatbuffers)               │
│  ┌──────────────────────────────────────────────────────────┐  │
│  │  Asio (coroutine-based async TCP) + AES-256-GCM + CRC32  │  │
│  │  Packet: [magic:4][length:4][type:2][seq:4][crc:4][data]│  │
│  └──────────────────────────────────────────────────────────┘  │
└─────────────────────────────────────────────────────────────────┘
```

### Struktur Direktori Final

```
Luna-Plus-Reborn/
├── CMakeLists.txt              # Root build
├── vcpkg.json                  # Dependency manifest
├── CMakePresets.json           # Build presets
├── README.md                   # Project info
│
├── engine/                     # Core engine library
│   ├── gx_render/              # bgfx rendering wrapper
│   ├── gx_geom/                # *** NEW *** Geometry/model engine
│   ├── gx_exec/                # Scene graph / object management
│   ├── grx_common/             # Types, math, enums
│   ├── physics/                # *** NEW *** Jolt Physics wrapper
│   ├── audio/                  # miniaudio wrapper
│   ├── network/                # *** NEW *** Asio network layer
│   ├── scripting/              # *** NEW *** Lua scripting engine
│   └── resource/               # *** NEW *** Resource manager
│
├── client/                     # Game client
│   ├── main.cpp                # Entry point
│   ├── gameobjects/            # Hero, Monster, NPC, Pet, etc.
│   ├── rendering/              # Scene, Terrain, Character, UI renderers
│   ├── audio/                  # AudioManager, AmbientSystem, SFX Sync
│   ├── effects/                # EffectManager, EftParser, Weather
│   ├── input/                  # InputSystem, Keyboard, Mouse
│   ├── config/                 # ConfigManager, KeyBindings
│   ├── network/                # NetworkClient, PacketCrypto, Launcher
│   ├── engine/                 # EngineCamera, EngineMap, EngineSky
│   └── ui/                     # Screens, Dialogs, Widgets, Skin
│
├── game/                       # Shared game logic (ECS)
│   ├── ecs/components/         # Transform, Movement, Stats, etc.
│   ├── ecs/systems/            # Combat, AI, Movement, Skill, etc.
│   ├── network/                # TcpServer, LoginHandler, CharacterDB
│   └── network/protocol/       # flatbuffers .fbs schemas
│
├── server/                     # Game servers
│   ├── agent/                  # AgentServer
│   ├── map/                    # MapServer (world simulation)
│   └── distribute/             # DistributeServer
│
├── lib/                        # Optional libraries
│   └── csoundlib/              # miniaudio wrapper (existing)
│
├── tools/                      # Development tools
│   ├── asset_pipeline/         # *** NEW *** Complete asset converter
│   ├── map_editor/             # *** NEW *** Qt/ImGui map editor
│   └── generated_fbs/          # flatbuffers generated code
│
├── assets/                     # Game assets (read-only)
│   ├── textures/               # .png / .ktx2 (future)
│   ├── models/                 # .glb
│   ├── animations/             # .anm.json
│   ├── audio/                  # .wav / .mp3
│   ├── maps/                   # .json + .hgt
│   ├── interface/              # UI layout scripts
│   ├── scripts/                # Game data scripts
│   ├── shaders/                # .sc + .bin
│   ├── fonts/                  # .ttf
│   └── data/                   # SQLite databases
│
├── shaders/                    # Shader source files (.sc)
├── config/                     # Server configs (.yaml)
├── database/                   # SQL schema files
├── cmake/                      # CMake modules
├── scripts/                    # CI scripts
└── .github/workflows/          # CI/CD
```

---

## 3. Asset Pipeline

### Complete Asset Conversion Flow

```
┌─────────────────────────────────────────────────────────────────┐
│                    LUNA-PLUS-OLD RAW ASSETS                      │
│  (LEGACY_ASSETS/ + LunaPlus/ + NEW_LUNA/)                       │
│                                                                 │
│  Textures: 20,276 files (DDS/PNG/TIF/TGA)                      │
│  Models:   10,573 files (MOD/OBJ)                               │
│  Anim:     7,105 files (ANM binary)                             │
│  Audio:    978 files (WAV/MP3)                                  │
│  Maps:     104 files (.map + .hfl)                              │
│  CharDef:  1,833 files (.chx)                                   │
│  Scripts:  4,199 files (.bin encrypted)                         │
│  DB:       3 SQLite databases                                   │
└───────────────────────────┬─────────────────────────────────────┘
                            │
                            ▼
┌─────────────────────────────────────────────────────────────────┐
│              PHASE 0: MASTER CONVERSION PIPELINE                 │
│                                                                  │
│  Step 1: Unpack PAK Archives                                    │
│    - unpack_pak.py / unpack_pak_v2.py                           │
│    - Output: decrypted .bin (.bin.txt)                          │
│                                                                  │
│  Step 2: Decrypt .bin files                                     │
│    - decrypt_luna.py (XOR + AES-256-ECB)                        │
│    - Output: .bin.txt (decrypted text/scripts)                  │
│                                                                  │
│  Step 3: Convert Textures (BATCH)                               │
│    - DDS → PNG: via wand/Pillow (preserve alpha, mipmaps)       │
│    - TIF → PNG: via Pillow                                      │
│    - TGA → PNG: via Pillow                                      │
│    - Organize: assets/textures/{category}/                      │
│    - ✅ SUDAH: 8,927 PNG files                                  │
│    - ⬜ TERSISA: ~11,000 files (DDS/TIF/TGA)                    │
│                                                                  │
│  Step 4: Convert Models (BATCH)                                 │
│    - .mod + .obj → .glb via assimp CLI + Python wrapper         │
│    - Optimize: meshoptimizer untuk vertex cache                 │
│    - Organize: assets/models/{category}/                        │
│    - ✅ SUDAH: 2,988 GLB files                                  │
│    - ⬜ TERSISA: ~7,500 files                                   │
│                                                                  │
│  Step 5: Prepare Animations                                     │
│    - .anm → .anm.json via anm_to_json.py                        │
│    - ✅ SUDAH: 7,039 JSON files (99%)                           │
│                                                                  │
│  Step 6: Convert Character Definitions                          │
│    - .chx → Parse: model_list + motion_list + material_list     │
│    - Output: character_desc.json per character                  │
│    - ✅ SUDAH: chx_to_gltf tool (parsial)                       │
│    - ⬜ TERSISA: 1,833 character defs → format baru             │
│                                                                  │
│  Step 7: Complete Terrain Conversion                            │
│    - .hfl → .hgt via hfl_to_raw.py                              │
│    - .map → scene.json via map_converter.py                     │
│    - ✅ SUDAH: 53 scene JSON + 27 HGT                           │
│    - ⬜ TERSISA: 27 HGT heightmaps                              │
│                                                                  │
│  Step 8: Extract UI Scripts                                     │
│    - Decrypt .bin → .bin.txt                                    │
│    - Parse .bin.txt → structured JSON layout                    │
│    - ✅ SUDAH: 262 scripts di Reborn                            │
│    - ⬜ TERSISA: ~3,900 scripts (mayoritas duplikat/redundan)   │
│                                                                  │
│  Step 9: Copy Audio & Databases                                 │
│    - ✅ SUDAH: 978 audio files (100%)                           │
│    - ✅ SUDAH: 3 SQLite databases (100%)                        │
│    - ✅ SUDAH: Shaders .sc (100%)                               │
└───────────────────────────┬─────────────────────────────────────┘
                            │
                            ▼
┌─────────────────────────────────────────────────────────────────┐
│              LUNA-PLUS-REBORN ASSETS (read-only)                 │
│                                                                  │
│  assets/textures/    ~20,000 .png files (100% coverage)         │
│  assets/models/      ~10,500 .glb files (100% coverage)         │
│  assets/animations/  ~7,100 .anm.json files (100% coverage)     │
│  assets/audio/       978 files (100% coverage)                  │
│  assets/maps/        54 scene.json + 54 .hgt (100% coverage)    │
│  assets/characters/  1,833 .json defs (100% coverage)           │
│  assets/interface/   ~290 UI script files                       │
│  assets/scripts/     ~260 game data scripts                     │
│  assets/data/        3 SQLite databases                         │
│  assets/shaders/     11 .sc + 9 .bin shaders                    │
│  assets/fonts/       1 .ttf font                                │
└─────────────────────────────────────────────────────────────────┘
```

---

## 4. Roadmap 5 Phases

### Phase 0 — Foundation & Asset Complete (Estimated: 2-3 bulan)

**Tujuan:** Semua asset dari Old 100% tersedia di Reborn. Build system solid. Tooling siap.

| # | Task | Teknologi | Detail |
|---|------|-----------|--------|
| P0.1 | **Asset Pipeline Tool** | Python + Pillow + assimp CLI | Script master yang batch semua konversi: textures (11K), models (7.5K), heightmaps (27), char defs (1.8K) |
| P0.2 | **Complete Texture Convert** | Python + Pillow | DDS/TIF/TGA → PNG. Preserve alpha channel. Generate thumbnail cache. |
| P0.3 | **Complete Model Convert** | Python + assimp CLI | MOD/OBJ → glb. Optimize via meshoptimizer. Generate LOD. |
| P0.4 | **Complete Heightmaps** | hfl_to_raw.py | Convert remaining 27 .hfl → .hgt |
| P0.5 | **Character Def Format** | JSON schema | Design format baru untuk character definitions (menggantikan .chx). Contoh: model parts, material slots, animation mapping, attachment points. |
| P0.6 | **Complete Char Defs** | Python + custom converter | Convert semua 1,833 .chx → format baru. |
| P0.7 | **vcpkg Integration** | vcpkg.json | Pindah dari find_package manual ke vcpkg manifest. Tambah: jolt, asio, sol2, nlohmann_json, meshoptimizer. |
| P0.8 | **C++23 Upgrade** | CMake + Clang 19 | Set `CMAKE_CXX_STANDARD 23`. Migrasi kode existing ke C++23 (std::expected, std::print, dll). |
| P0.9 | **CI/CD Complete** | GitHub Actions | Build otomatis macOS + Windows. Unit test + integration test. Asset validation. |

**Deliverable:** Semua asset 100% siap. Build system solid di macOS + Windows.

---

### Phase 1 — Core Engine Runtime (Estimated: 4-5 bulan)

**Tujuan:** Engine runtime yang lengkap — model loading, animation blending, physics, collision, audio 3D.

| # | Task | Teknologi | File Baru |
|---|------|-----------|-----------|
| P1.1 | **Geometry Engine** | C++23 custom | `engine/gx_geom/Model.h/cpp`, `MeshObject.h/cpp`, `CoGeometry.h/cpp` |
| P1.2 | **Animation Runtime** | C++23 + AnmParser | `engine/gx_geom/AnimationSystem.h/cpp`, `Skeleton.h/cpp`, `Skinning.h/cpp` |
| P1.3 | **Animation Blending** | C++23 | Cross-fade, additive animation, bone mask blending |
| P1.4 | **Resource Manager** | C++23 + EnTT | `engine/resource/ResourceCache.h/cpp` — Thread-safe async loading, refcount, hot-reload |
| P1.5 | **Physics Engine** | Jolt Physics | `engine/physics/PhysicsWorld.h/cpp` — Rigid body, character controller, ray cast |
| P1.6 | **Collision System** | Jolt + custom | Update existing CollisionSystem — heightfield collider, mesh collider, trigger volumes |
| P1.7 | **Character Renderer Complete** | bgfx + Assimp + AnmParser | Integrasi AnmParser + CharacterRenderer. Skeletal animation GPU skinning via `vs_skinned.sc`. |
| P1.8 | **Audio 3D** | miniaudio | Implementasi 3D spatial audio (position-based panning, distance attenuation, doppler) |
| P1.9 | **Scene Graph** | C++23 + EnTT | Entity hierarchy, transform propagation, bounding volume updates |
| P1.10 | **LOD System** | meshoptimizer | Generate LOD meshes untuk model. Distance-based LOD selection. |

**Deliverable:** Karakter berjalan, berlari, menyerang dengan animasi mulus. Fisika dan collision berfungsi. Audio 3D.

---

### Phase 2 — Client Feature Complete (Estimated: 5-6 bulan)

**Tujuan:** Semua fitur client Old tersedia di Reborn — UI, game objects, effects, input, config.

| # | Task | Files |
|---|------|-------|
| P2.1 | **Complete UI Dialogs** | Selesaikan semua 30+ dialog types. Polish: tooltip, drag-drop, keyboard navigation, animation. |
| P2.2 | **MiniMap / WorldMap** | `MiniMapDlg`, `WorldMapDlg` — render terrain minimap, entity dots, fog of war |
| P2.3 | **Character Creation** | `CharMakeDlg` — class selection, appearance customization, preview |
| P2.4 | **Tutorial System** | `TutorialDlg`, `TutorialManager` — event-triggered tutorial popup, step tracking |
| P2.5 | **PK / PvP System** | `PKManager`, `PKLootingDlg` — PK flag, penalty, looting |
| P2.6 | **Helper / Mascot** | `HelperDlg` — companion NPC dengan buff |
| P2.7 | **Fade / Transition** | `FadeDlg` — screen fade in/out, cut transitions |
| P2.8 | **Movie / Video** | Integrasi video playback untuk cutscenes |
| P2.9 | **Effect System Complete** | Implementasi semua legacy effect units: Particle, Animation, Billboard, Light, Sound, CameraShake, Model |
| P2.10 | **Particle System** | ECS-based particle system: emitters, attractors, physics-based particles |
| P2.11 | **Input System Complete** | Gamepad/controller support via GLFW gamepad API. Rebinding UI. |
| P2.12 | **Config System Complete** | Video/audio/gameplay settings UI. Profile save/load. |

**Deliverable:** Client Luna-Plus-Reborn memiliki semua fitur yang ada di Luna-Plus-Old client. UI lengkap dan responsif.

---

### Phase 3 — Server Complete (Estimated: 8-10 bulan)

**Tujuan:** Server game yang fully functional — semua game logic dari Old diimplementasikan dengan ECS modern.

| # | Task | Komponen Server |
|---|------|-----------------|
| P3.1 | **Network Layer** | Asio async TCP server. flatbuffers packet routing. Connection management. |
| P3.2 | **Agent Server** | Login/auth, character management, session management |
| P3.3 | **Map Server Core** | ECS world simulation, entity spawning, grid/chunk management |
| P3.4 | **Combat System (Server)** | Damage calculation, skill execution, buff/debuff, status effects, knockback |
| P3.5 | **AI System (Server)** | Monster AI: patrol, aggro, chase, attack, flee, return. Boss AI: phase triggers, enrage. |
| P3.6 | **Item & Inventory** | Item generation, inventory management, equipment, item drop, loot |
| P3.7 | **Quest System (Server)** | Quest state tracking, conditions, rewards, NPC interactions |
| P3.8 | **Party & Guild** | Party formation, experience sharing, guild management, guild warehouse |
| P3.9 | **Housing System** | Housing instance, furniture placement, farming |
| P3.10 | **Siege System** | Castle siege, flag capture, guild war scoring |
| P3.11 | **Dungeon System** | Instance dungeons, boss encounters, reward tables |
| P3.12 | **Fishing & Cooking** | Fishing mini-game, cooking recipes, stat buff food |
| P3.13 | **Vehicle / Mount** | Mount system, movement speed buff, combat dismount |
| P3.14 | **Trading System** | Player-to-player trade, consignment auction, mail |
| P3.15 | **FSM Engine** | Finite State Machine untuk quest/event scripting |
| P3.16 | **Trigger System** | Event triggers: area enter, kill count, item use, time |
| P3.17 | **Distribute Server** | Channel management, chat server, party matching |
| P3.18 | **Database Layer** | SQLite via DBThread pattern. Player save/load. Game data queries. |

**Deliverable:** Server lengkap dengan semua game logic. Client bisa login, bermain, naik level, quest, party, guild, PvP, housing, siege, dungeon.

---

### Phase 4 — Content & Polish (Estimated: 4-6 bulan)

**Tujuan:** Konten game lengkap — semua map, monster, NPC, quest, item, skill.

| # | Task | Detail |
|---|------|--------|
| P4.1 | **All Maps Playable** | Load semua 54 map dengan terrain, props, NPC placements. Pathfinding per map. |
| P4.2 | **All Monsters** | 340+ monster types dengan AI, loot table, spawn points |
| P4.3 | **All NPCs** | 115+ NPCs dengan dialog, quest, shop |
| P4.4 | **All Quests** | Implementasi quest chains dari game data |
| P4.5 | **All Skills** | Active + buff skills per class. Skill tree. |
| P4.6 | **All Items** | Equipment, consumables, materials, recipes |
| P4.7 | **Boss Encounters** | Field bosses + dungeon bosses dengan mechanics |
| P4.8 | **Economy Balance** | Drop rates, shop prices, crafting costs |
| P4.9 | **Localization** | Multi-language support (dari legacy data) |

**Deliverable:** Game playable dari level 1 sampai end-game. Semua konten original tersedia.

---

### Phase 5 — Polish & Future (Estimated: 3-4 bulan)

**Tujuan:** Quality of life, performance, optimasi untuk masa depan mobile.

| # | Task | Detail |
|---|------|--------|
| P5.1 | **Performance Optimization** | GPU instancing, occlusion culling, texture atlasing, audio streaming |
| P5.2 | **Map Editor** | Qt/ImGui-based editor untuk terrain editing, object placement, NPC/monster placement |
| P5.3 | **GM Tools** | Admin panel: player management, item spawning, quest debugging |
| P5.4 | **Monitor Server** | Live server monitoring: player count, performance metrics, alerts |
| P5.5 | **Tutorial Complete** | New player experience: guided tutorial, tooltips, help system |
| P5.6 | **Mobile Prep** | Migrasi GLFW → SDL3. Touch input layer. UI scaling untuk small screen. Profile GPU untuk mobile. |
| P5.7 | **Controller / Gamepad** | Full controller support. UI navigation via controller. |
| P5.8 | **Achievements** | Achievement system dengan rewards |
| P5.9 | **Photo Mode** | Free camera, filters, screenshot sharing |
| P5.10 | **macOS + Windows Release** | Code signing, installer, auto-updater |

**Deliverable:** Game siap rilis. Foundation untuk mobile sudah siap.

---

## 5. Target Platform & Build System

### Platform Support Matrix

| Platform | Phase | Graphics Backend | Status |
|----------|-------|-----------------|--------|
| **macOS (Apple Silicon)** | P0 (sekarang) | bgfx + Metal | ✅ Aktif |
| **macOS (Intel)** | P0 (sekarang) | bgfx + Metal | ✅ Supported |
| **Windows 10/11 x64** | P0-P1 | bgfx + D3D12/Vulkan | ⚠️ Perlu setup |
| **Linux** | P2 | bgfx + Vulkan | ⏳ Future |
| **Android** | P5 | bgfx + Vulkan/GLES | ⏳ Future (P5) |
| **iOS** | P5 | bgfx + Metal | ⏳ Future (P5) |

### Build System

```cmake
# CMakeLists.txt root — target akhir
cmake_minimum_required(VERSION 3.30)
project(LunaPlusReborn VERSION 2.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# vcpkg manifest mode
if(DEFINED CMAKE_TOOLCHAIN_FILE)
    # vcpkg akan handle semua dependencies
endif()

# Platform detection
if(APPLE)
    set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64")
endif()

# Dependencies via vcpkg
find_package(bgfx CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_paphore(glm CONFIG REQUIRED)
find_package(EnTT CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)
find_package(Jolt REQUIRED)         # NEW
find_package(asio CONFIG REQUIRED)  # NEW
find_package(sol2 CONFIG REQUIRED)  # NEW
find_package(nlohmann_json CONFIG REQUIRED)  # NEW
find_package(meshoptimizer CONFIG REQUIRED)  # NEW
find_package(miniaudio CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(SQLite3 REQUIRED)
find_package(flatbuffers CONFIG REQUIRED)
```

### vcpkg.json Manifest

```json
{
  "name": "luna-plus-reborn",
  "version": "2.0.0",
  "dependencies": [
    "bgfx", "glfw3", "glm", "entt", "assimp",
    "joltphysics", "asio", "sol2", "nlohmann-json",
    "meshoptimizer", "miniaudio", "spdlog", "fmt",
    "sqlite3", "flatbuffers", "stb", "freetype"
  ]
}
```

---

## 6. Estimasi Timeline

```
Phase 0: Foundation & Asset Complete
  ████████████████████████████████░░░░░░░░░░░░░░░░  2-3 bulan
  Mulai: SEKARANG

Phase 1: Core Engine Runtime
  ░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░  4-5 bulan
  Mulai: Bulan 3

Phase 2: Client Feature Complete
  ░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░  5-6 bulan
  Mulai: Bulan 7

Phase 3: Server Complete
  ░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░  8-10 bulan
  Mulai: Bulan 8 (overlap dengan Phase 2)

Phase 4: Content & Polish
  ░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░  4-6 bulan
  Mulai: Bulan 15

Phase 5: Polish & Mobile Prep
  ░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░  3-4 bulan
  Mulai: Bulan 20

TOTAL ESTIMASI: ~24 bulan (2 tahun) → MVP dapat dimainkan di bulan 8-10
```

### Milestone Key

| Milestone | Target | Fitur |
|-----------|--------|-------|
| **M0** — Asset Complete | Bulan 3 | Semua asset 100% siap. Build cross-platform. |
| **M1** — Tech Demo | Bulan 4 | Karakter dengan animasi berjalan di map. Terrain + sky. |
| **M2** — Client Alpha | Bulan 8 | Client bisa login, create character, masuk game, move, chat, basic combat. |
| **M3** — Server Alpha | Bulan 12 | Server stabil. Multiplayer: party, quest, inventory, NPC. |
| **M4** — Content Beta | Bulan 18 | Semua map, monster, quest, skill, item. Progresi dari level 1-100. |
| **M5** — Release | Bulan 24+ | Game siap rilis. macOS + Windows. Foundation untuk mobile. |

---

## Lampiran: Yang Perlu Dipertahankan dari Reborn Existing

### ✅ SUDAH LENGKAP — Jangan diutak-atik

| Komponen | File | Keterangan |
|----------|------|------------|
| RenderDevice | `engine/gx_render/` | bgfx + GLFW init, view management |
| Texture + TextureManager | `engine/gx_render/` | stb_image loader, caching |
| Shader + ShaderManager | `engine/gx_render/` | bgfx program loading |
| VertexBuffer + IndexBuffer | `engine/gx_render/` | bgfx buffer management |
| Font | `engine/gx_render/` | FreeType + stb_truetype atlas |
| EnTT Components | `game/ecs/components/` | 17 components |
| EnTT Systems | `game/ecs/systems/` | 19 systems |
| Audio | `client/audio/` | AudioManager, AmbientSystem, SFX Sync |
| Input | `client/input/` | InputSystem, Keyboard, Mouse, UserInput |
| Config | `client/config/` | ConfigManager, KeyBindings |
| Network Crypto | `client/network/PacketCrypto` | AES-256-CBC |
| Terrain Renderer | `client/rendering/TerrainRenderer` | HGT loader, 3 LOD, elevation color |
| Sky System | `client/engine/EngineSky` | Dome mesh, day/night cycle |
| Camera | `client/engine/EngineCamera` | Orbit camera, smooth transition |
| UI Widgets | `client/ui/widgets/` | 11 widget types |
| Skin Manager | `client/ui/skin/` | UiSkinManager |
| ECS Systems (client) | `game/ecs/systems/` | Combat, AI, Movement, Skill, etc. |
| Databases | `assets/data/` | 3 SQLite DB |
| Shaders | `assets/shaders/` | 11 .sc + 9 .bin |

### ⚠️ PERLU REVISI / UPGRADE

| Komponen | Revisi |
|----------|--------|
| AnmParser → AnimationRuntime | Parser ✅, integrasi runtime ❌ |
| CharacterRenderer | Assimp loader ✅, skinning ✅, animasi runtime ❌ |
| TerrainRenderer | HGT ✅, LOD ✅, tile blending ❌, quad-tree ❌ |
| Hero/Monster/NPC | State machine ✅, visual rendering perlu animasi ✅ |
| 30+ Dialogs | Fungsi dasar ✅, banyak polish ❌ (tooltip, drag, animasi) |
| CMakeLists.txt | Upgrade C++23, tambah vcpkg, tambah dependencies baru |

### 🔴 PRIORITAS TINGGI — Yang Paling Kritis

1. **Animation Runtime** — Tanpa ini karakter tidak bisa bergerak
2. **Geometry Engine** — Loading model dengan material/texture
3. **Resource Manager** — Thread-safe async loading
4. **Complete Asset Pipeline** — 100% asset coverage
5. **Server Core** — Login + world simulation minimal
6. **Physics + Collision** — Jolt Physics integration

---

*"Kita tidak mem-porting kode. Kita membangkitkan jiwa Luna dengan teknologi masa depan."*
