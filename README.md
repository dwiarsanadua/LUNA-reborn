# LUNA Plus Reborn

Modern C++20 rebuild of LUNA Online Plus MMORPG.

**Platform:** macOS (arm64), Windows (via bgfx)  
**Stack:** C++20, EnTT ECS, bgfx/Metal, GLFW, FlatBuffers, SQLite, miniaudio  
**Real completion vs original game: ~18-25%**

---

## Quick Start (macOS)

```bash
brew install cmake ninja glm spdlog fmt flatbuffers assimp glfw libuv freetype miniaudio
cd LUNA-Plus-Reborn
cmake -G Ninja -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --target LunaPlusClient
./build/bin/LunaPlusClient
```

## Hotkeys

```
C=Character  E=Mail  Q=CashShop  I=Inventory
K=Skills     J=Quests  P=Party  G=Guild(+Siege)
F=Friends  L=Family  H=Farm  R=Housing  O=Options
B=Bank  M=Map  Y=Auction  U=Trade  V=Fish  X=Dungeon
Z=Costume  N=NPC  T=Chat  Space=Spawn
F1=Help  F2=ChatRooms  F3=Pets  F4=Cooking
F5=Mounts  F6=Tournament  F7=Avatar  F8=Upgrade  F9=Macros
→=KeyBindings
```

## Project Structure

```
LUNA-Plus-Reborn/
├── client/          # Game client (rendering, UI, audio, input)
│   ├── rendering/   # bgfx renderers (terrain, props, particles)
│   ├── ui/          # 34 dialog types, WindowManager, widgets
│   ├── gameobjects/ # 46+ game systems
│   ├── audio/       # BGM, SFX, ambient
│   └── network/     # FlatBuffers protocol, AES encryption
├── game/            # Shared ECS systems (server-side)
├── tools/           # Map converter, asset tools
├── assets/          # Game assets (textures, models, audio)
├── shaders/         # bgfx shader sources (.sc + compiled .bin)
└── external/        # Third-party (bgfx, bx, bimg)
```

## Documentation

| Document | Purpose |
|----------|---------|
| `QA_COMPARISON_REPORT.md` | Honest parity analysis vs original game |
| `REBORN_100_PERCENT_PLAN.md` | Ultra-detail 100% parity master plan |
| `TRACKING.md` | Progress checklist (18-25% real completion) |
| `ARCHITECTURE.md` | Technical architecture overview |
| `NETWORK_PROTOCOL.md` | Protocol schema documentation |

## Current State

**Playable tech demo with 34 dialog types, 46+ game systems, full 3D rendering pipeline including shadow mapping and terrain splatting.** Core features functional: combat, trading, farming, fishing, dungeons, NPC shops, enchanting, crafting, guild wars, pets, mounts, cooking, tournaments, mail, auction house, family system.

**What's missing (biggest gaps):** Network protocol expansion (95% → need 2,032+ packet types for multiplayer), UI completion (77% → 116+ dialog types), asset pipeline (91% → 15,500+ assets), database persistence (91% → 214 tables), audio library (96% → 480+ sounds).

Full parity estimated at 18-24 months for single developer.
