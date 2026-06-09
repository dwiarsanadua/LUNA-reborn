# Agent-04 — Server Methods: AI States (Patrol, Chase, Flee, Return)

Lokasi: /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn
Build: cmake --build build/macos-debug -j$(sysctl -n hw.ncpu)

## Tugas

Implementasi AI state machine untuk monster. Old punya AISystem.cpp dengan states: IDLE, PATROL, CHASE, ATTACK, FLEE, DIE, RETURN. Reborn sudah punya basic AISystem. Perlu tambah method untuk state-handling yang masih missing.

## Aturan Ketat

1. BACA game/ecs/systems/AISystem.hpp dan AISystem.cpp — catat method yang SUDAH ADA
2. BACA server/map/systems/AISystem.h dan .cpp — catat method
3. CEK method berikut:
   - `UpdateState` atau `SetState` → cek
   - `Patrol` → cek (monster walk random around spawn point)
   - `Chase` → cek (monster chase target)
   - `Flee` → cek (monster run away when low HP)
   - `ReturnToSpawn` → cek (monster return when target lost)
   - `FindNearestTarget` → cek
   - `HandleBossPhase` → cek
4. ✅ Jika SUDAH ADA → skip
5. 🔧 Jika BELUM ADA → tambah

## Method yang harus dicek

Execute di terminal:
```bash
grep -n "void\|bool\|int\|float.*State\|Patrol\|Chase\|Flee\|ReturnToSpawn\|FindNearest" \
  game/ecs/systems/AISystem.* server/map/systems/AISystem.*
```

Untuk AI state yang missing, implementasi dengan pattern:
```cpp
enum class AIState : uint8_t {
    Idle, Patrol, Chase, Attack, Hit, Flee, Die, Respawn, Return
};

void AISystem::SetState(entt::registry& reg, entt::entity entity, AIState new_state) {
    auto* ai = reg.try_get<AIComponent>(entity);
    if (!ai) return;
    if (ai->state == new_state) return;
    ai->state = new_state;
    ai->state_timer = 0.0f;
    // Trigger state entry logic
    switch (new_state) {
        case AIState::Patrol: ai->patrol_target = PickRandomPatrolPoint(ai->spawn_position); break;
        case AIState::Chase: /* target already set */ break;
        case AIState::Flee: ai->flee_direction = glm::normalize(ai->spawn_position - ai->position); break;
        case AIState::Return: ai->return_target = ai->spawn_position; break;
        default: break;
    }
}
```

## Output

✅ Kembalikan: "Agent-04 done: added [list state yang ditambah], skipped [list yang sudah ada]"
