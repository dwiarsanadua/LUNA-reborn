# LUNA Plus — Reborn

> **Engine:** C++23 · bgfx (Metal) · EnTT ECS · Jolt Physics · Asio · miniaudio
> **Platform:** macOS (Apple Silicon) · Windows
> **Status:** 🚧 Phase 3 — Integration & Asset Completion

## What's Inside

| Directory | Contents | Agent |
|-----------|----------|-------|
| `engine/gx_render/` | bgfx rendering wrapper (RenderDevice, Texture, Shader, VertexBuffer, Font) | Aether |
| `engine/gx_geom/` | Model loader (Assimp), AnimationSystem, MeshObject, Skeleton | Titan |
| `engine/physics/` | PhysicsWorld (Jolt Physics wrapper) | Titan |
| `engine/resource/` | ResourceCache (thread-safe template cache) | Titan |
| `engine/network/` | NetworkLayer (Asio async TCP) | Nexus |
| `engine/scripting/` | LuaEngine (sol2/LuaJIT wrapper) | Nexus |
| `client/` | Full game client (rendering, UI, audio, effects, input, game objects) | Aether + Nexus |
| `game/ecs/` | EnTT ECS — 18 components + 19 systems | Built-in |
| `server/` | Agent + Map + Distribute servers with 5 game systems | Nexus |
| `tools/` | Asset pipeline, converters, flatbuffers schemas | Phoenix |
| `assets/` | Game assets (textures, models, audio, shaders, etc.) | Vulcan |

## Current State

| Agent | Task | Status |
|-------|------|--------|
| Phoenix | Asset pipeline tools | ✅ Complete |
| Forge | Build system (vcpkg, C++23, CI/CD) | ✅ Complete |
| Titan | Engine core (geometry, animation, physics, resource) | ✅ Complete |
| Aether | Render & animation rendering | ✅ Complete |
| Nexus | Network layer + full server stack | ✅ Complete |
| Vulcan | Asset symlink fix + partial pipeline | ✅ Complete |
| **Compilation** | **Missing deps (bgfx, Jolt)** | **🔴 Blocked** |
| **Assets** | **Models, animations, maps still 0** | **🟡 Partial** |

## Next Phase — 3 Agents

See `AGENT_PROMPTS.md` for Phase 3 prompts:
1. **Agent Builder** — Clone & build bgfx/Jolt, fix compile errors
2. **Agent Collector** — Complete asset conversion (models, animations, maps)
3. **Agent Integrator** — Connect CharacterRenderer with Model, test server
