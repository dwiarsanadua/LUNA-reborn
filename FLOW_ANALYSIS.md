# Flow Analysis: Luna-Plus-Old vs Luna-Plus-Reborn

> **Berdasarkan audit kode mendalam** pada Hero lifecycle, Network flow, Monster FSM, dan Resource loading.

---

## 1. Hero Lifecycle Mapping

### Old — CHero (2.303 baris)

```
Per-Frame Update Loop (MainGame.cpp:228):
  └─ Process():
      1. Sleep(1) — frame limiter
      2. MHTIMEMGR->Process() — update gCurTime, gTickTime
      3. g_UserInput.Process() — mouse/keyboard
      4. WINDOWMGR->Process() — UI dialogs
      5. m_pCurrentGameState->Process() — game state
         └─ GameIn::Process():
             ├─ Hero::Process() — 145 baris:
             │   ├─ State machine: ProcessStateMachine()
             │   ├─ Auto-attack: AttackProcess()
             │   ├─ Battle state: CheckObjectBattleState()
             │   ├─ Combo check: DoCombo()
             │   ├─ Movement: Move_UsePath() / Move_Simple()
             │   ├─ Skill casting: ProcessStartSkill()
             │   └─ Waypoint: ProcessWayPoint()
             ├─ Monster::ProcessAllMonsters()
             ├─ Npc::ProcessAllNpcs()
             ├─ Item::ProcessDropItems()
             ├─ Animation sync: APPEARANCEMGR->ProcessReservation()
             └─ Effect cleanup: ProcessDeleteList()
      6. AUDIOMGR->Process() — BGM crossfade
      7. APPEARANCEMGR->AlphaAppearance() — alpha blending
      8. Engine: m_pEngine->EndProcess() — Present + garbage collect
```

**Urutan Old:**
```
Input → Timing → UI → Hero/Game State → Monsters → NPCs → Items → Animation Sync → Effects → Audio → Render
```

### Reborn — Hero (341 baris)

```
Per-Frame Game Loop (main.cpp:312):
  └─ device.BeginFrame() — GLFW poll + bgfx clear
  └─ input_sys.Update(dt)
  └─ Mouse::Update()
  └─ Camera update
  └─ (if not login screen):
  │    ├─ gfx.BeginFrame() — Shadow pass + scene setup
  │    ├─ ambient.Update()
  │    ├─ gfx.Render(&terrain, &props) — Terrain + Props
  │    └─ gfx.RenderCharacters() — CharacterRenderer
  ├─ ui.BeginFrame()
  ├─ screenManager.Update(dt)
  │   └─ GameScreen::Update():
  │       ├─ Hero::Update() — 28 baris:
  │       │   ├─ ProcessStateTransitions()
  │       │   ├─ Physics sync (SetCharacterPosition)
  │       │   ├─ Move X/Z toward target
  │       │   └─ GameState sync
  │       ├─ Monster::UpdateAll()
  │       └─ particleSys_.Update()
  ├─ screenManager.Render(ui, view, proj)
  │   └─ GameScreen::Render():
  │       ├─ Sky dome
  │       ├─ Terrain + Props + Characters (DUPLICATE RENDER)
  │       ├─ Particles
  │       └─ UI Overlay (HP bars, etc.)
  ├─ persistence.Update(dt)
  └─ device.EndFrame() — bgfx::frame()
```

**Urutan Reborn:**
```
Timing → Input → Camera → Shadow → Terrain/Props/Characters → UI → Game State → Render → Audio → Present
```

### GAP: Urutan Render State Sebelum Game State

| Aspek | Old | Reborn | Dampak |
|-------|-----|--------|--------|
| **Render timing** | Setelah game state update | Sebelum game state update | 🔴 Reborn render posisi frame SEBELUMNYA (1 frame delay) |
| **Animation sync** | `AppearanceManager::ProcessReservation()` — batch animasi | Tidak ada — langsung render | 🟡 CharacterRenderer tidak sinkron |
| **State machine** | 145 baris di `Process()` | 28 baris di `Update()` | 🟡 Reborn simplified, missing auto-attack, combo, waypoint |
| **Movement** | `Move_UsePath()` + `Move_Simple()` + `WayPoint` | Direct position set | 🔴 Old punya 3 mode movement + buffer 10 waypoint |
| **Combo/Auto-attack** | `AttackProcess()` + `DoCombo()` | ❌ Tidak ada | 🔴 Combat feel hilang |
| **Alpha blending/animation** | `AlphaAppearance()` — transisi smooth | ❌ Tidak ada | 🟡 Visual transisi abrupt |

---

## 2. Network-to-Logic Flow

### Old — Windows Message Based

```
Network.dll (IOCP)
  └─ OnRecv() callback
      ├─ Decrypt (optional XOR/CRC)
      ├─ CGameState::CommonNetworkParser()
      │   └─ Handle common messages (MP_MOVE, MP_CHAT, etc.)
      └─ m_pCurState->NetworkMsgParse()
          └─ GameIn::NetworkMsgParse():
              ├─ MP_ATTACK_ACK → Hero::AttackProcess()
              ├─ MP_USERCONN_MONSTER_ADD → MonsterManager::Add()
              ├─ MP_MOVE → Hero::SetTargetPosition()
              ├─ MP_SKILL → SkillManager::Execute()
              └─ ... 100+ message types
```

**Key:** Pesan network diproses **sinkron** dalam callback `OnRecv` — langsung mempengaruhi state game dalam thread yang sama.

### Reborn — Thread-Based + Callback

```
ReadThread() [std::thread]
  └─ recv() blocking
  └─ Packet framing (magic+length)
  └─ handler_(type, payload) callback → main thread?

PacketDispatcher (registered handlers)
  └─ HandlerLoginResult()
  └─ HandlerChatMessage()
  └─ HandlerEntityMove()
  └─ ... via PacketDispatcher.hpp
```

**Masalah:** `ReadThread()` memanggil `handler_` dari **thread terpisah**. Jika handler mengakses shared state tanpa mutex, akan terjadi **race condition**.

| Aspek | Old | Reborn | Dampak |
|-------|-----|--------|--------|
| **Thread safety** | Single thread (message queue) | Raw callback dari thread terpisah | 🔴 **Race condition risk** |
| **Dispatch latency** | Immediate (dalam callback) | Queue → Process di main loop? | 🔴 Tambah 1 frame latency |
| **Packet framing** | `MSGROOT` (Category/Protocol) | `PacketHeader` (magic+type+seq) | ✅ Modern |
| **Serialization** | Manual binary struct | FlatBuffers | ✅ Zero-copy |
| **Encryption** | XOR + optional AES | AES-256-GCM (PacketCrypto) | ✅ Lebih aman |

### GAP: Safety

**Rekomendasi:**
```cpp
// Tambah thread-safe queue di NetworkClient.hpp
#include <mutex>
#include <queue>

struct NetworkEvent {
    uint16_t type;
    std::vector<uint8_t> payload;
};
std::queue<NetworkEvent> event_queue_;
std::mutex queue_mutex_;

// ReadThread → push ke queue
void NetworkClient::ReadThread() {
    // ... receive loop ...
    std::lock_guard<std::mutex> lock(queue_mutex_);
    event_queue_.push({hdr.type, payload});
}

// Main loop → process queue
void NetworkClient::ProcessEvents() {
    std::lock_guard<std::mutex> lock(queue_mutex_);
    while (!event_queue_.empty()) {
        auto evt = event_queue_.front();
        event_queue_.pop();
        handler_(evt.type, evt.payload);
    }
}
```

---

## 3. Monster FSM — State Machine Consistency

### Old — Finite State Machine (FSM) Library

```
60+ file FSM library di [Server]Map/Finite State Machine/
CMonster memiliki FiniteStateMachine::CMachine
  └─ State machine di-set oleh GSTATEMACHINE global

States:
  eMA_STAND (Idle) → eMA_WALK (WalkAround) → eMA_ATTACK → eMA_PERSUIT (Chase) → eMA_STAND
  eMA_DIE → cleanup

Key constants:
  - Battle delay: 10.000 ms (cDelay)
  - Aggro scan: every 5000ms (mStateParamter.SearchLastTime)
  - Movement: via pathfinding (PATHMANAGER)
  - Help request: via AISystem.SendMsg() — monster ke monster
  - ThinkAggro(): Highest aggro wins
  - DoDamage() → AddAggro() → RequestHelp() → StateProcess()
```

### Reborn — ECS-Based AI

```
AISystem::Update(registry) → iterate AIComponent + Transform + CharacterStats
  └─ switch (ai.state):
      Idle → ScanForTargets() → Chase
      Patrol → move to patrol_points[current]
      Chase → check range → Attack or Return
      Attack → CombatState::StartCast() → damage
      Return → move to spawn_position → Idle
      BossPhase1-5 → HandleBossAI()

Key constants (dari AIComponent.hpp):
  - aggro_range: 10.0f
  - attack_range: 3.0f
  - chase_range: 30.0f
  - enrage_timer: 180.0f (3 menit)
  - phase_hp: 75%, 50%, 25%, 10%
  - special_attack_cooldown: 8.0f
```

### Perbandingan Konstanta

| Parameter | Old | Reborn | Match? |
|-----------|-----|--------|--------|
| **Battle delay** | 10.000 ms (fixed) | Variable (ECS tick) | ⚠️ Reborn tidak punya delay setara |
| **Aggro scan** | 5.000 ms | Setiap ECS tick | 🔴 Reborn terlalu cepat |
| **Max aggro distance** | Tidak jelas | 10.0 (aggro_range) | ✅ |
| **Attack range** | Monster-specific | 3.0f | ⚠️ Harusnya dari DB |
| **Chase range** | Tidak jelas | 30.0f | ✅ |
| **Help request** | ✅ AISystem.SendMsg() | ❌ **Tidak ada** | 🔴 Monster tidak saling bantu |
| **Pathfinding** | ✅ PathManager | ❌ NavMeshSystem (stub) | 🔴 Movement tanpa collision |
| **Threat table** | ✅ `AggroContainer` | ✅ `threat_table` vector | ✅ |
| **Boss phases** | ❌ (manual di script) | ✅ 5 phase + enrage | ✅ **Reborn unggul** |
| **FSM flexibility** | ✅ Generic FSM library | ✅ ECS switch-case | ⚠️ Reborn kurang reusable |

---

## 4. Resource Loading Sequence

### Old — CoExecutive::CreateGXObject

```
executive.cpp:1259 — CreateGXObject(filename):
  1. Normalize path + lowercase → hash lookup
  2. Cache hit → duplicate handles, inc refcount
  3. Cache miss → LoadGXObject():
      ├─ Parse .chr → LoadModelData() → LoadModel()
      │   ├─ CoGeometry::LoadModel() → model.cpp → ReadFile()
      │   │   ├─ Read FILE_SCENE_HEADER (28 bytes)
      │   │   ├─ Read materials (FILE_MATERIAL_HEADER)
      │   │   └─ Read objects (mesh, bone, light, camera)
      │   └─ Initialize(m_pRenderer) → Create IDIMeshObject
      ├─ LoadMotion() → CMotion::ReadFile()
      │   ├─ Read FILE_MOTION_HEADER
      │   └─ Parse keyframe data
      └─ LoadMaterial() → texture references
  4. AddGXObject() → CoGXObject* with schedule proc
  5. Linked list: pModDesc->pGXObject (sharing)

Urutan: Model → Texture → Animation → Material → Object
Semua SINKRON — objek tidak muncul sampai semua dependency siap.
```

### Reborn — AssetPreloader

```
AssetPreloader::ScanAssets() — hanya SCAN, tidak loading:
  ├─ ScanDirectory("assets/textures/", "png")
  ├─ ScanDirectory("assets/textures/ui/", "png")
  ├─ ScanDirectory("assets/interface/", "...")
  └─ Count: 11.752 textures, 532 interface, 978 audio

PreloadTextures() — on-demand (hitung target, tidak loading)
CharRenderer_LoadModel() — Assimp import + GPU upload (async)
EngineMap::Load() — load HGT + scene JSON (sync)
```

### GAP: Loading Sequence

| Aspek | Old | Reborn | Dampak |
|-------|-----|--------|--------|
| **Model loading** | Sync + cache + refcount | Assimp langsung di CharRenderer | 🔴 Tidak ada cache global |
| **Texture loading** | Sync + hash table 25.600 | on-demand via stb_image | 🟡 Load lambat saat first-use |
| **Animation loading** | Sync + cache via CoGeometry | ❌ Tidak ada loading (static pose) | 🔴 Karakter tidak bergerak |
| **Dependency order** | Model → Texture → Animation → Material | Random (first-use) | 🔴 Visual glitch saat objek muncul |
| **Background loading** | ❌ Semua sync | ⬜ AssetPreloader (stub) | Tidak ada progress bar |
| **Refcount** | ✅ MODEL_FILE_DESC | ❌ Tidak ada | 🔴 Memory leak potensial |

---

## 5. Ringkasan "Gap Flow"

### 🔴 Critical (menyebabkan game tidak berfungsi dengan benar)

| Gap | Lokasi | Dampak |
|-----|--------|--------|
| **Render sebelum game state update** | `main.cpp:337` vs `GameScreen::Update()` | 1 frame delay posisi karakter |
| **Network callback dari thread terpisah** | `NetworkClient::ReadThread()` | Race condition potensial |
| **Animation tidak pernah di-load** | `CharacterRenderer` tidak panggil `AnimationSystem` | Karakter static pose |
| **Tidak ada auto-attack** | `Hero.cpp` — `AttackProcess()` tidak ada | Combat tidak bisa auto-attack |
| **Tidak ada waypoint movement** | `ServerAuthMovement.hpp` — buffer ada tapi tidak dipakai | Movement kurang smooth |
| **Tidak ada resource cache global** | Setiap renderer load sendiri-sendiri | Memory usage tinggi, duplikasi |

### 🟡 Moderate (perlu diperbaiki)

| Gap | Detail |
|-----|--------|
| **Aggro scan terlalu cepat** | ECS tick setiap frame (vs 5 detik di Old) |
| **Monster help request** | AISystem tidak kirim bantuan ke monster lain |
| **Pathfinding** | NavMeshSystem masih stub |
| **Combo system** | Tidak ada chain attack |
| **Alpha blending transisi** | `AlphaAppearance()` tidak ada |
| **Resource cache** | Tidak ada reference counting |

### 🟢 Minor

| Gap | Detail |
|-----|--------|
| **BGM crossfade** | `SmoothBGMTransition()` ada ✅ |
| **Persistence** | Auto-save setiap 30 detik ✅ |
| **Camera orbit** | Smooth transition ✅ |
| **Physics** | Jolt integration ✅ |
| **Boss mechanics** | 5 phase + enrage ✅ |

---

## 6. Rekomendasi

### Urutan Perbaikan

| # | Fix | File | Effort |
|---|-----|------|--------|
| 1 | **Animation runtime** | `CharacterRenderer.cpp` — panggil `AnimationSystem::Play()` | 1 hari |
| 2 | **Thread-safe network queue** | `NetworkClient.hpp` — tambah event queue + mutex | 2 jam |
| 3 | **Render order: update→render** | `main.cpp` — pindah update sebelum render | 30 menit |
| 4 | **Auto-attack** | `Hero.cpp` — tambah `AttackProcess()` | 3 jam |
| 5 | **Waypoint movement** | `ServerAuthMovement.cpp` — aktifkan interpolasi | 2 jam |
| 6 | **Resource cache global** | `engine/resource/ResourceCache.h` — implementasi shared | 1 hari |
| 7 | **Monster help request** | `AISystem.cpp` — kirim event ke monster nearby | 2 jam |
| 8 | **Aggro scan delay** | `AISystem.cpp` — tambah cooldown 5 detik | 30 menit |

### Konstanta yang Perlu Disesuaikan

| Konstanta | Old | Reborn | Rekomendasi |
|-----------|-----|--------|-------------|
| Aggro scan interval | 5.000 ms | setiap frame | 5.000 ms |
| Battle delay | 10.000 ms | tidak ada | 10.000 ms |
| Attack range | per monster | 3.0f | dari DB monster |
| Chase range | tidak jelas | 30.0f | 50.0f |
| Waypoint buffer | 10 pos | deque 60 | ✅ sudah cukup |
| Animation FPS | 30 FPS (.anm) | tidak ada | 30 FPS |
