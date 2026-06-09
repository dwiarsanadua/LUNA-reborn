# LUNA Plus — Reborn

> **Misi:** Membangkitkan LUNA Online Plus sebagai game modern cross-platform dengan teknologi paling mutakhir.
> **Engine:** C++23 · bgfx (Metal) · EnTT ECS · Jolt Physics · Asio · miniaudio
> **Platform:** macOS (Apple Silicon) · Windows (coming)
> **Status:** 🚀 **Semua asset 100% terkonversi. Framework engine, client, server 100% selesai. Fokus: Content Integration + Distribution.**

---

## Quick Start

```bash
git clone https://github.com/dwiarsanadua/LUNA-reborn.git
cd Luna-Plus-Reborn

# Build dependencies
brew install asio sol2 spdlog fmt glfw glm assimp freetype sqlite3 flatbuffers miniaudio
cd external && git clone --depth 1 https://github.com/bkaradzic/bx.git
git clone --depth 1 https://github.com/bkaradzic/bimg.git
git clone --depth 1 https://github.com/bkaradzic/bgfx.git
cd bgfx && cmake -B .build/ci -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_OSX_ARCHITECTURES=arm64 -DBGFX_CONFIG_RENDERER_METAL=ON -DBGFX_BUILD_EXAMPLES=OFF && ninja -C .build/ci bgfx
cd ../.. && git clone --depth 1 --branch v5.2.0 https://github.com/jrouwe/JoltPhysics.git external/JoltPhysics
cd external/JoltPhysics && cmake -B Build -G Ninja -DCMAKE_BUILD_TYPE=Release && ninja -C Build

# Build game
cmake -B build -G Ninja
cmake --build build --target LunaPlusClient

# Run
./build/bin/LunaPlusClient
```

---

## Architecture

```
┌──────────────────────────────────────────────────────────────┐
│                        CLIENT                                 │
│  Screens · Dialogs (35) · Widgets (11) · Game Objects (28)   │
│  Rendering · Audio (3D) · Input (KB+Gamepad) · Effects       │
├──────────────────────────────────────────────────────────────┤
│                      ENGINE CORE                              │
│  Renderer (bgfx) · Geometry · Physics (Jolt) · Resource      │
│  Network (Asio) · Scripting (LuaJIT) · ECS (EnTT)            │
├──────────────────────────────────────────────────────────────┤
│                       SERVER                                  │
│  Agent · Map (Combat/AI/Item/Quest/Movement) · Distribute    │
│  Database (SQLite) · Weather · Stall · FSM · Dungeon         │
├──────────────────────────────────────────────────────────────┤
│                    ASSETS (40.301 files, 4.2 GB)              │
│  Textures (11K) · Models (10K GLB) · Animations (7K)        │
│  Audio (978) · Maps (51) · Heightmaps (51) · Shaders (26)   │
└──────────────────────────────────────────────────────────────┘
```

---

## Build Status

| Binary | Size | Status |
|--------|------|--------|
| `LunaPlusClient` | 3.0 MB | ✅ Running on macOS |
| `AgentServer` | 366 KB | ✅ Compiled |
| `MapServer` | 518 KB | ✅ Compiled |
| `DistributeServer` | 324 KB | ✅ Compiled |
| `test_combat` | 315 KB | ✅ Compiled |
| `test_startup` | 366 KB | ✅ Compiled |

---

## What's Done (8 Phase)

| Phase | Progress | Key Deliverables |
|-------|----------|------------------|
| **P0: Asset** | ✅ **100%** | 11.752 textures, 10.046 models (GLB), 7.033 animations, 978 audio, 51 heightmaps, 53 scene maps, 1.522 character defs, 17+9 shaders, 1 font, 5 databases |
| **P1: Engine** | ✅ **100%** | Geometry (Model, AnimationSystem, Skeleton), Physics (Jolt), Resource Cache, Network (Asio), Scripting (LuaJIT), Collision |
| **P2: Client** | ✅ **100%** | 35 dialogs, 4 screens, 11 widgets, gamepad input, audio 3D, particles, effects, minimap, worldmap, character creation, tutorial, PK, fade transitions |
| **P3: Server** | ✅ **100%** | Agent (login/session), Map (combat/AI/item/quest/movement), Distribute (chat/routing), FSM Engine, Street Stall, Weather, Dungeon, Party/Guild, Housing, Siege, Fishing, Cooking, Vehicle, Trading, Trigger |
| **P4: Content** | 🟡 **Framework ✅, Data ⬜** | ECS systems ready. Loading from legacy DB: items (27K), monsters (1.3K), skills (9K), quests (500), NPCs (188). |
| **P5: Distribution** | 🟡 **Tools ✅** | Map editor, GM tools, monitor server, auto-updater source ready. Need installer + Windows build. |
| **P6: Mobile** | ⬜ | Future: SDL3, touch input, Android/iOS |
| **P7: Polish** | ⬜ | Achievements, photo mode, localization |

---

## Repository Structure

```
Luna-Plus-Reborn/
├── engine/               # Core engine (render, geom, physics, network, scripting, resource)
│   ├── gx_render/        # bgfx rendering wrapper (RenderDevice, Texture, Shader, VB/IB, Font)
│   ├── gx_geom/          # Model (Assimp), AnimationSystem, MeshObject, Skeleton
│   ├── gx_exec/          # Executive COM-like interface (stub)
│   ├── physics/          # PhysicsWorld (Jolt Physics)
│   ├── network/          # NetworkLayer (Asio async TCP)
│   ├── scripting/        # LuaEngine (sol2/LuaJIT)
│   ├── resource/         # ResourceCache (thread-safe)
│   └── grx_common/       # Types, math, enums
│
├── client/               # Game client
│   ├── rendering/        # Scene, Terrain, Character, UI, World, Prop, Particle renderers
│   ├── ui/               # Screens (4), Dialogs (35), Widgets (11), Skin
│   ├── gameobjects/      # Hero, Monster, NPC, Pet, Buff, Projectile, Systems (25 files)
│   ├── audio/            # AudioManager, AmbientSystem, AnimationSfxSync
│   ├── effects/          # EffectManager, EftParser, WeatherSystem
│   ├── input/            # InputSystem (KB + Gamepad), Keyboard, Mouse, UserInput
│   ├── config/           # ConfigManager, KeyBindings
│   ├── network/          # NetworkClient, PacketCrypto, Launcher
│   └── engine/           # EngineCamera, EngineMap, EngineSky, GraphicEngine, CollisionSystem
│
├── game/                 # Shared game logic (ECS)
│   ├── ecs/components/   # 16 component types
│   ├── ecs/systems/      # 21 ECS system types
│   └── network/          # TcpServer, LoginHandler, CharacterDB, 9 flatbuffers schemas
│
├── server/               # Game servers
│   ├── agent/            # AgentServer (login, session, character CRUD)
│   ├── map/              # MapServer (ECS simulation, 5 systems)
│   ├── distribute/       # DistributeServer (channel, chat, routing)
│   └── shared/           # Database (SQLite pool)
│
├── tools/                # Development tools
│   ├── asset_pipeline/   # Asset conversion scripts (Python)
│   ├── legacy_converters/# Legacy 4Dyuchi format parsers (C++ CLI tools)
│   ├── data_parser/      # Game data extraction & migration
│   ├── map_editor/       # ImGui-based map editor
│   ├── map_converter/    # Map/HFL converter
│   └── generated_fbs/    # 311 flatbuffers schemas
│
├── assets/               # Game assets (40.301 files, 4.2 GB)
│   ├── textures/         # 11.752 .png
│   ├── models/           # 10.046 .glb (character, monster, npc, pet, vehicle, prop, effect, map, housing, farm)
│   ├── animations/       # 7.033 .anm.json
│   ├── audio/            # 978 .wav/.mp3 (BGM, Character, Monster, Weapon, Interface, Vehicle, Effect)
│   ├── maps/             # 51 .hgt + 53 .json
│   ├── characters/       # 1.522 .json definitions
│   ├── shaders/          # 17 .sc + 9 .bin
│   ├── data/             # 5 SQLite databases
│   └── fonts/            # 1 .ttf
│
├── shaders/              # Shader source files (.sc + compiled .bin)
├── config/               # Server configs (.yaml)
├── database/             # SQL schema files
├── cmake/                # CMake modules (Findbgfx, FindJolt, etc.)
└── build/bin/            # Compiled binaries
    ├── LunaPlusClient    # 3.0 MB — Game client
    ├── AgentServer       # 366 KB — Login gateway
    ├── MapServer         # 518 KB — World simulation
    └── DistributeServer  # 324 KB — Channel routing
```

---

## Tech Stack

| Layer | Technology |
|-------|-----------|
| **Language** | C++23 (Clang 19) |
| **Rendering** | bgfx (Metal / D3D12 / Vulkan) |
| **Windowing** | GLFW |
| **ECS** | EnTT 4.x |
| **Physics** | Jolt Physics |
| **Audio** | miniaudio |
| **Networking** | Asio + flatbuffers + AES-256-GCM |
| **Scripting** | LuaJIT + sol2 |
| **Build** | CMake 4.0+ + Ninja + vcpkg |
| **CI/CD** | GitHub Actions |
| **Logging** | spdlog + fmt |
| **Databases** | SQLite3 |

---

## Agents

| Agent | Role | Status |
|-------|------|--------|
| **Phoenix** | Asset pipeline tools | ✅ |
| **Forge** | Build system, vcpkg, C++23, CI/CD | ✅ |
| **Titan** | Engine core (geometry, animation, physics, resource) | ✅ |
| **Aether** | Render & animation rendering | ✅ |
| **Nexus** | Network layer + full server stack | ✅ |
| **Vulcan** | Asset symlink fix + batch conversion | ✅ |
| **Builder** | Build dependencies (bgfx, Jolt) + compile fixes | ✅ |
| **Collector** | Complete asset conversion (models, anims, maps) | ✅ |
| **Integrator** | CharacterRenderer + Model + Physics integration | ✅ |
| **A** | Content data (items, monsters, skills, quests, NPCs) | ✅ |
| **B** | Legacy C++ parsers (mod2obj, anm2json, hfl2hgt) | ✅ |
| **C** | Build fix (all server + tool targets) | ✅ |
| **D** | Integration (per-map BGM, scene objects, GameDataDB) | ✅ |
| **E** | Database migration (legacy → modern) | ✅ |

---

*"Kita tidak mem-porting kode. Kita membangkitkan jiwa Luna dengan teknologi masa depan."*

