# LUNA Plus — Reborn

> **Project:** LUNA Online Plus — Rebuild from scratch (modern C++20, ECS-based)
> **Target:** macOS (Apple Silicon + Intel) · Windows 10/11 x64
> **Status:** 🚧 ~34% complete (100 of ~300 target files)
> **Base Path:** `LUNA-Plus-Reborn/`

## Quick Links

| Document | Description |
|----------|-------------|
| **`REBORN_100_PERCENT_PLAN.md`** | 📗 **BLUEPRINT** — Master plan to reach 100% original feature parity |
| **`TRACKING.md`** | ✅ **CHECKLIST** — Real-time progress tracking (read before coding) |
| **`ARCHITECTURE.md`** | 🏗️ System architecture overview |
| **`CLIENT_ARCHITECTURE.md`** | 🖥️ Client-side architecture |
| **`SERVER_ARCHITECTURE.md`** | 🌐 Server-side architecture |
| **`TECH_STACK.md`** | ⚙️ Technology decisions |
| **`NETWORK_PROTOCOL.md`** | 📡 FlatBuffers protocol reference |
| **`DATABASE_SCHEMA.md`** | 🗄️ SQLite database schema |
| **`ASSET_PIPELINE.md`** | 🎨 Asset conversion pipeline |

## Status

```
✅ Complete    ████████████░░░░░░░░░░  34%
⬜ Remaining   ░░░░░░░░░░████████████  66%
```

### ✅ What's Done (Layers 0-5)
- Full rendering pipeline: 7 views, bgfx/Metal, font atlas, particles, UI
- 3 ECS servers: Agent, Distribute, Map
- 19 ECS game systems & 14 components
- Game objects: Hero, Monster, Pet, NPC, ObjectBalloon
- UI framework: Screen/Window manager + 10 widgets + 8 dialogs
- Effects: Damage numbers, camera shake, weather (rain/snow)
- Audio: BGM/SFX/3D/volume
- Infrastructure: CMake, CI, SQLite, 809 models, 8,173 textures

### ⬜ What's Next
- **Phase A:** Input System + Config (12 files)
- **Phase B:** Effect System — 17 trigger types + 8 effect units (30 files)
- **Phase C:** UI Wave 1 — 48 core game dialogs (48 files)
- **Phase D:** UI Wave 2 — 28 advanced dialogs (28 files)
- **Phase E:** Widget completion — 8 widget types (10 files)
- **Phase F:** Rendering enhancement + game objects (18 files)
- **Phase G:** Game systems + engine completion (12 files)

## How to Contribute

1. Read `REBORN_100_PERCENT_PLAN.md` for full blueprint
2. Check `TRACKING.md` for current status
3. Pick an unmarked item from the highest priority layer
4. Implement per blueprint specs
5. Update `TRACKING.md` (⬜ → ✅) and commit

## Tech Stack

| Layer | Technology |
|-------|------------|
| Rendering | bgfx (Metal/Vulkan/D3D/OGL) |
| Windowing | GLFW 3.4 |
| ECS | EnTT 3.14+ |
| Networking | libuv + FlatBuffers |
| Data | SQLite (234 tables) |
| Audio | miniaudio |
| Models | glTF via Assimp |
| Build | CMake 4.3+ + Ninja |
| CI | GitHub Actions |
| Logging | spdlog + fmt |
