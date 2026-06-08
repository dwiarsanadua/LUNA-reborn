# READY-TO-USE PROMPTS — Phase 3: Integration & Asset Completion

> **3 Agent Paralel** — Builder, Collector, Integrator.
> **Zero file overlap** — aman dijalankan bersamaan.

---

## Daftar Isi

1. [Agent Builder — Build & Dependencies](#1-agent-builder--build--dependencies)
2. [Agent Collector — Complete Asset Conversion](#2-agent-collector--complete-asset-conversion)
3. [Agent Integrator — Code Integration & Test](#3-agent-integrator--code-integration--test)
4. [Ringkasan Paralel](#4-ringkasan-paralel)

---

## 1. Agent Builder — Build & Dependencies

> **Tujuan:** Install semua dependency yang hilang (bgfx, Jolt), fix CMake, compile total.
> **Area:** `cmake/`, `external/`, `scripts/`, `engine/physics/`, `engine/scripting/`
> **Estimasi:** 3-5 hari

```
Kamu adalah BUILDER — Build & Dependencies Engineer.

## 🚨 SAFETY RULES (MUST FOLLOW)

### 1. FILE OWNERSHIP
You may ONLY create/modify files in these directories:
  /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/cmake/
  /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/external/        (CREATE)
  /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/scripts/
  /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/include/external/

You are ALSO allowed to edit:
  /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/CMakeLists.txt
  /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/engine/physics/CMakeLists.txt
  /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/engine/scripting/CMakeLists.txt
  /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/engine/network/CMakeLists.txt
  /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/engine/resource/CMakeLists.txt
  /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/engine/gx_geom/CMakeLists.txt
  /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/engine/CMakeLists.txt

You MUST NEVER modify files in client/, server/, game/, tools/, assets/.

### 2. COMMIT DISCIPLINE
git add cmake/ external/ scripts/ include/external/ CMakeLists.txt engine/*/CMakeLists.txt
git commit -m "agent_builder: [summary]"

### 3. STOP ON ERROR
If a command fails, LOG it and CONTINUE. Report all errors at the end.

## CURRENT STATE — READ FIRST

1. Read the root CMakeLists.txt:
   cat /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/CMakeLists.txt

2. Check what dependencies are available:
   ls /opt/homebrew/include/asio.hpp 2>/dev/null && echo "asio: OK" || echo "asio: MISSING"
   ls /opt/homebrew/include/sol/sol.hpp 2>/dev/null && echo "sol2: OK" || echo "sol2: MISSING"

3. Check build state:
   cmake --build /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/build 2>&1 | tail -20

## YOUR TASKS — Execute in Order

### Task 1 — Install Dependencies via Homebrew
Run these commands to install all available deps:
  brew install asio sol2 spdlog fmt glfw glm assimp freetype sqlite3

### Task 2 — Create external/ directory and build bgfx
mkdir -p /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/external/
cd /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/external/

git clone --depth 1 https://github.com/bkaradzic/bx.git
git clone --depth 1 https://github.com/bkaradzic/bimg.git
git clone --depth 1 https://github.com/bkaradzic/bgfx.git

cd bgfx
mkdir -p .build/ci
cmake -B .build/ci -G Ninja \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_OSX_ARCHITECTURES=arm64 \
  -DBGFX_CONFIG_RENDERER_METAL=ON \
  -DBGFX_BUILD_EXAMPLES=OFF \
  -DBGFX_BUILD_TOOLS=OFF
ninja -C .build/ci bgfx

Verify bgfx compiled:
  ls .build/ci/libbgfx.a 2>/dev/null && echo "bgfx built OK" || echo "bgfx build FAILED"

### Task 3 — Create Findbgfx.cmake for external/ build
Edit: /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/cmake/Findbgfx.cmake
Update the include path to point to:
  ${CMAKE_SOURCE_DIR}/external/bgfx/include
  ${CMAKE_SOURCE_DIR}/external/bx/include
  ${CMAKE_SOURCE_DIR}/external/bimg/include

And library path to:
  ${CMAKE_SOURCE_DIR}/external/bgfx/.build/ci

### Task 4 — Download Jolt Physics
cd /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/external/
git clone --depth 1 --branch v5.2.0 https://github.com/jrouwe/JoltPhysics.git

Create FindJolt.cmake:
  /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/cmake/FindJolt.cmake
  - Set Jolt_INCLUDE_DIR to external/JoltPhysics/
  - Set Jolt_LIBRARY to external/JoltPhysics/Build/Release/libJolt.a
  - Create IMPORTED target Jolt::Jolt

Build Jolt:
  cd external/JoltPhysics
  cmake -B Build -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_OSX_ARCHITECTURES=arm64
  ninja -C Build

### Task 5 — Create FindAsio.cmake
Create: /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/cmake/FindAsio.cmake
  find_path(ASIO_INCLUDE_DIR asio.hpp PATHS /opt/homebrew/include /usr/local/include /usr/include)
  add_library(asio::asio INTERFACE IMPORTED)
  set_target_properties(asio::asio PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${ASIO_INCLUDE_DIR}")

### Task 6 — Fix engine subdirectory CMakeLists
For each of these, ensure they properly link their dependencies:

engine/gx_geom/CMakeLists.txt:
  target_link_libraries(gx_geom PRIVATE assimp::assimp bgfx::bgfx glm::glm)

engine/physics/CMakeLists.txt:
  target_link_libraries(gx_physics PRIVATE Jolt::Jolt)

engine/network/CMakeLists.txt:
  target_link_libraries(gx_network PRIVATE asio::asio)

engine/scripting/CMakeLists.txt:
  target_link_libraries(gx_scripting PRIVATE sol2::sol2)

engine/resource/CMakeLists.txt:
  target_link_libraries(gx_resource PRIVATE spdlog::spdlog fmt::fmt)

### Task 7 — Iterative Compilation
Run cmake --build, fix errors one by one:
  cmake -B /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/build
  cmake --build /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/build

For each error:
  1. Read the error message carefully
  2. Read the offending file
  3. Fix the issue (typo, missing include, wrong API, etc.)
  4. Rebuild
  5. Repeat until compilation succeeds

### Task 8 — Fix Specific Compilation Issues
Expected issues to fix:
  - engine/gx_geom/Model.cpp: missing Assimp includes (need aiScene.h, aiMesh.h, etc.)
  - engine/gx_geom/AnimationSystem.cpp: missing include guards or typos
  - engine/physics/PhysicsWorld.cpp: Jolt API might differ slightly
  - engine/network/NetworkLayer.cpp: Asio async API details
  - engine/scripting/LuaEngine.cpp: sol2 API usage
  - server/*/AgentServer.cpp, MapServer.cpp: missing includes for flatbuffers, ECS
  - client/rendering/CharacterRenderer.cpp: type mismatches with new Model class
  - All "undefined reference" errors: add missing target_link_libraries

## YOUR DELIVERABLE
After ALL tasks:
  git add cmake/ external/ scripts/ include/external/ CMakeLists.txt engine/*/CMakeLists.txt
  git commit -m "agent_builder: build system complete — bgfx, Jolt, asio, sol2, all compile errors fixed"

Then run:
  cmake -B /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/build 2>&1
  cmake --build /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/build 2>&1

Print:
  "BUILDER COMPLETE
   Dependencies: bgfx ✅ Jolt ✅ asio ✅ sol2 ✅
   Compilation: [SUCCEEDED / FAILED with XX errors]
   Binaries: ls build/bin/"

## CRITICAL: DO NOT
- Touch client/ code (rendering, game objects, UI)
- Touch server/ code (agent, map, distribute)
- Touch assets/ or tools/asset_pipeline/
- Delete any existing cmake files
```

---

## 2. Agent Collector — Complete Asset Conversion

> **Tujuan:** Jalankan pipeline untuk melengkapi model GLB, animation JSON, dan HGT maps.
> **Area:** `tools/asset_pipeline/`, `assets/`
> **Estimasi:** 2-3 hari

```
Kamu adalah COLLECTOR — Asset Conversion Engineer.

## 🚨 SAFETY RULES (MUST FOLLOW)

### 1. FILE OWNERSHIP
You may ONLY create/modify files in these directories:
  /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/tools/asset_pipeline/
  /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/assets/

You may READ from:
  /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Old/LEGACY_ASSETS/
  /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Old/LunaPlus/

You MUST NEVER modify files in engine/, client/, game/, server/, shaders/, cmake/.

### 2. COMMIT DISCIPLINE
git add tools/asset_pipeline/ assets/
git commit -m "agent_collector: [summary]"

## CURRENT STATE — READ FIRST

1. Check what exists:
  find /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/assets/ -type f | wc -l

2. Check pipeline scripts:
  ls /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/tools/asset_pipeline/

3. Check available source assets:
  find /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Old/LEGACY_ASSETS/legacy_unpacked/raw_originals/assets/unpacked/ -name "*.mod" 2>/dev/null | wc -l
  find /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Old/LEGACY_ASSETS/legacy_unpacked/raw_originals/assets/unpacked/ -name "*.obj" 2>/dev/null | wc -l
  find /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Old/LEGACY_ASSETS/legacy_unpacked/raw_originals/assets/ -name "*.anm" 2>/dev/null | wc -l
  find /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Old/LEGACY_ASSETS/legacy_unpacked/raw_originals/assets/unpacked/map/ -name "*.hfl" 2>/dev/null | wc -l
  find /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Old/LEGACY_ASSETS/legacy_unpacked/raw_originals/assets/unpacked/map/ -name "*.map" 2>/dev/null | wc -l

4. Check existing pipeline log:
  cat /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/tools/asset_pipeline/pipeline_status.json 2>/dev/null

## YOUR TASKS — Execute in Order

### Task 1 — Verify Pipeline Scripts Work
cd /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/tools/asset_pipeline
python3 run.py --dry-run
This should list all steps without executing.

If run.py doesn't exist or has errors, check the individual scripts:
  ls convert_*.py
  If missing, copy from ../ (root tools/) or recreate.

### Task 2 — Complete Model Conversion (.mod/.obj → .glb)
Run the batch model converter:
  cd /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/tools/asset_pipeline
  python3 run.py --models

Source dir: /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Old/LEGACY_ASSETS/legacy_unpacked/raw_originals/assets/unpacked/
Target dir: /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/assets/models/

Organize into subdirectories:
  character/  — character body parts, costumes
  monster/    — monster models
  npc/        — NPC models
  pet/        — pet models
  vehicle/    — mount/vehicle models
  prop/       — environment props
  weapon/     — weapons, shields

### Task 3 — Complete Animation Conversion (.anm → .anm.json)
Run the animation converter:
  cd /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/tools/asset_pipeline
  python3 run.py --animations

If the --animations flag doesn't exist, run:
  cp /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Old/scripts_legacy/convert_anm.py .
  python3 convert_anm.py --input /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Old/LEGACY_ASSETS/legacy_unpacked/raw_originals/assets/ --output /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/assets/animations/

Note: Legacy .anm files are at:
  find /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Old/LEGACY_ASSETS/ -name "*.anm" 2>/dev/null

### Task 4 — Complete Heightmap Conversion (.hfl → .hgt)
Run the heightmap converter:
  cd /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/tools/asset_pipeline
  python3 run.py --heightmaps

Or use the dedicated script:
  cp /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Old/scripts_legacy/hfl_to_raw.py .
  python3 hfl_to_raw.py --input /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Old/LEGACY_ASSETS/legacy_unpacked/raw_originals/assets/unpacked/map/ --output /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/assets/maps/

### Task 5 — Convert Map Scripts (.map → scene.json)
Convert map definition files:
  cd /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/tools/asset_pipeline
  python3 run.py --maps

Or using the converter tool:
  /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/tools/map_converter/main

Map sources: /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Old/LEGACY_ASSETS/legacy_unpacked/raw_originals/assets/unpacked/map/
Look for .map files and convert to JSON scene format.

### Task 6 — Organize and Validate
After all conversions:
  cd /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/tools/asset_pipeline
  python3 validate_assets.py --report
  python3 generate_inventory.py

### Task 7 — Create README_ASSETS.md
Create: /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/assets/README_ASSETS.md
With final counts:

# Assets Inventory
Generated: [date]

| Category | Count | Format |
|----------|-------|--------|
| Textures | XX | .png |
| Models | XX | .glb |
| Animations | XX | .anm.json |
| Audio | XX | .wav/.mp3 |
| Heightmaps | XX | .hgt |
| Scene Maps | XX | .json |
| Character Defs | XX | .json |
| Shaders (compiled) | XX | .bin |
| Fonts | XX | .ttf |

## YOUR DELIVERABLE
After ALL tasks:
  git add tools/asset_pipeline/ assets/
  git commit -m "agent_collector: complete asset conversion — models, animations, heightmaps done"

Print:
  "COLLECTOR COMPLETE
   Textures: XX files
   Models: XX → XX files (was 0)
   Animations: XX → XX files (was 0)
   Heightmaps: XX → XX files (was 0)
   Total assets: XX GB"

## CRITICAL: DO NOT
- Touch engine/, client/, game/, server/ directories
- Touch cmake/ or CMakeLists.txt files
- Delete existing assets — only add new ones
```

---

## 3. Agent Integrator — Code Integration & Test

> **Tujuan:** Hubungkan CharacterRenderer dengan Model class, integrasikan AnimationSystem, test server startup.
> **Area:** `client/rendering/`, `client/gameobjects/`, `client/engine/`, `engine/gx_geom/`, `server/`
> **Estimasi:** 4-6 hari

```
Kamu adalah INTEGRATOR — Code Integration & Test Engineer.

## 🚨 SAFETY RULES (MUST FOLLOW)

### 1. FILE OWNERSHIP
You may ONLY create/modify files in these directories:
  /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/client/rendering/
  /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/client/gameobjects/
  /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/client/engine/
  /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/engine/gx_geom/
  /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/game/ecs/
  /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/server/

You may READ files from ANYWHERE but may only MODIFY files in the above directories.
You MUST NEVER modify files in cmake/, tools/, assets/, shaders/.

### 2. COMMIT DISCIPLINE
After each integration step:
  git add [specific files]
  git commit -m "agent_integrator: [summary]"

### 3. API CONTRACT
Before modifying a file, read it fully. If it has:
  // AGENT Titan — DO NOT MODIFY
  // AGENT Nexus — DO NOT MODIFY
Then coordinate the change — do NOT silently override.

## CURRENT STATE — READ FIRST

Read these files to understand the interfaces:

Titan's engine core:
  /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/engine/gx_geom/Model.h
  /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/engine/gx_geom/AnimationSystem.h
  /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/engine/gx_geom/MeshObject.h
  /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/engine/gx_geom/Skeleton.h

Existing renderer:
  /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/client/rendering/CharacterRenderer.cpp
  /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/client/rendering/CharacterRenderer.hpp
  /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/client/rendering/AnmParser.h
  /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/client/rendering/SceneRenderer.cpp

Game objects:
  /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/client/gameobjects/Hero.cpp
  /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/client/gameobjects/Monster.cpp

Server:
  /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/server/agent/AgentServer.h
  /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/server/map/MapServer.h
  /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/server/shared/Database.h

## YOUR TASKS — Execute in Order

### Task 1 — Integrate Model class into CharacterRenderer
Edit: /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/client/rendering/CharacterRenderer.cpp

Currently CharacterRenderer uses Assimp directly (aiScene, aiMesh). Change it to use Titan's Model class:

  1. Include "engine/gx_geom/Model.h" and "engine/gx_geom/MeshObject.h"
  2. Change CharRenderer_LoadModel() to create a Model instance and call LoadFromGLB()
  3. Use Model::GetMeshes() to get mesh data
  4. Use MeshObject::UploadToGPU() to create bgfx vertex/index buffers
  5. Use Model::GetBones() for skeleton data
  6. Update the SkinnedVertex struct if needed to match MeshPart format

### Task 2 — Integrate AnimationSystem into CharacterRenderer
Edit: /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/client/rendering/CharacterRenderer.cpp

  1. Include "engine/gx_geom/AnimationSystem.h"
  2. Load .anm.json files using AnimationSystem::LoadFromJson()
  3. In the render loop, call AnimationSystem::Update(dt) and GetBlendedPose()
  4. Upload bone matrices to GPU uniform "u_bones"
  5. Support animation state transitions (idle → walk → run → attack via CharAnim enum)
  6. Implement blend between animations (BlendTo() with 0.2s transition)

### Task 3 — Integrate Skeleton
Edit: /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/client/rendering/CharacterRenderer.cpp

  1. Include "engine/gx_geom/Skeleton.h"
  2. Build skeleton from Model::GetBones()
  3. Use Skeleton::ComputeFinalPose() to get world-space bone transforms
  4. Pass to AnimationSystem::GetBlendedPose() for correct skinning

### Task 4 — Update vs_skinned.shader to Match
Edit: /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/shaders/vs_skinned.sc

  1. Ensure it declares uniform mat4 u_bones[64]
  2. Ensure it transforms positions by bone weights
  3. Ensure it transforms normals for correct lighting

### Task 5 — Integrate PhysicsWorld into Hero/Monster
Edit: /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/client/gameobjects/Hero.cpp
Edit: /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/client/gameobjects/Monster.cpp

  1. Include "engine/physics/PhysicsWorld.h"
  2. On Hero::Init(), create a physics character via PhysicsWorld::CreateCharacter()
  3. On Hero::Move(), use PhysicsWorld::SetCharacterPosition()
  4. On Monster::Update(), check PhysicsWorld::RayCast() for line-of-sight
  5. Add gravity: on each frame, apply PhysicsWorld::GetTerrainHeight() to stick to ground

### Task 6 — Integrate PhysicsWorld into CollisionSystem
Edit: /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/client/engine/CollisionSystem.hpp

  Verify it already has SetPhysicsWorld() method.
  If not, add it. Ensure IsWalkable(x,z) uses PhysicsWorld raycast.

### Task 7 — Test Server Startup
Create a simple test script or modify server main to:
  1. Initialize Database
  2. Initialize NetworkLayer
  3. Start AgentServer on port 8100
  4. Print "Server started successfully"
  5. Run for 5 seconds, then shutdown

File: /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/server/test_startup.cpp
  #include "agent/AgentServer.h"
  #include "shared/Database.h"
  
  int main() {
    Database db;
    db.Initialize("assets/data/luna_member.db");
    
    AgentServer server;
    server.Initialize(8100);
    
    printf("Server test: OK\n");
    sleep(5);
    
    server.Shutdown();
    db.Shutdown();
    return 0;
  }

### Task 8 — Update CMakeLists for Integration
Edit: /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/client/CMakeLists.txt
  Ensure client links: gx_geom gx_physics gx_render

Edit: /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/game/CMakeLists.txt
  Ensure game_core links: gx_geom gx_physics

## YOUR DELIVERABLE
After ALL tasks:
  git add client/rendering/ client/gameobjects/ client/engine/ engine/gx_geom/ game/ecs/ server/ shaders/
  git commit -m "agent_integrator: full integration — Model, AnimationSystem, Physics, Server test"

Print:
  "INTEGRATOR COMPLETE
   CharacterRenderer → Model: [OK/SKIPPED]
   CharacterRenderer → AnimationSystem: [OK/SKIPPED]
   Hero/Monster → Physics: [OK/SKIPPED]
   Server startup test: [OK/FAILED]
   Compilation: [SUCCEEDED/FAILED]"

## CRITICAL: DO NOT
- Touch cmake/ or root CMakeLists.txt — Builder handles that
- Touch tools/ or assets/ — Collector handles that
- Touch shaders/ except vs_skinned.sc
- Delete any existing files — only extend/modify
```

---

## 4. Ringkasan Paralel

### Area Kerja — Zero Conflict

| Agent | Direktori | File Baru | File Existing |
|-------|-----------|-----------|---------------|
| **Builder** | `cmake/`, `external/`, `scripts/` | `external/bgfx/`, `external/JoltPhysics/` | `CMakeLists.txt`, `engine/*/CMakeLists.txt` |
| **Collector** | `tools/`, `assets/` | `.glb`, `.anm.json`, `.hgt` | `tools/asset_pipeline/` |
| **Integrator** | `client/rendering/`, `client/gameobjects/`, `server/` | `server/test_startup.cpp` | `CharacterRenderer.cpp`, `Hero.cpp`, `Monster.cpp` |

**Zero konflik — setiap agent memiliki area tertutup sendiri.**

### Timeline

```
AGENT      DAY 1  2  3  4  5  6  7  8  9 10
────────  ────────────────────────────────────
Builder   ██████████████████░░░░░░░░░░░░░░░░
Collector ██████████████████████████░░░░░░░░
Integrator░░░░░░░░████████████████████████░░

           Week 1          Week 2
```

### Aturan

1. **Builder + Collector bisa mulai bersamaan** (Hari 1) — beda area total
2. **Integrator mulai Hari 4** — butuh Model class dari Builder dan asset dari Collector
3. **Semua commit WAJIB** setelah setiap task selesai
4. **Setelah semua selesai** → compile total dari root
