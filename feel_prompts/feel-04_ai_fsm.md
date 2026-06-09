# FEEL-04 — Monster AI FSM Alignment

Lokasi: /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn
Build: cmake --build build/macos-debug -j$(sysctl -n hw.ncpu)

## Tugas

Old punya Finite State Machine untuk AI monster di `[Server]Map/AISystem.cpp`: IDLE → PATROL → CHASE → ATTACK → HIT → FLEE → DIE → RETURN. Transisi state punya threshold dan timer spesifik. Reborn punya AISystem basic. Perilaku monster berbeda.

## Aturan Ketat

1. BACA Old: `/Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Old/[Server]Map/AISystem.cpp`
2. BACA Reborn: `game/ecs/systems/AISystem.cpp` dan `server/map/systems/AISystem.cpp`
3. ✅ Jika AI states sudah match Old — skip
4. 🔧 Jika beda — update state machine

## Old AI State Machine

```
IDLE:
  - Stay at spawn position
  - Scan for targets setiap N detik
  - Jika target in aggro_range → CHASE
  
PATROL:
  - Walk random around spawn (radius = patrol_radius)  
  - Pause at each waypoint for random time
  - Jika target in aggro_range → CHASE

CHASE:
  - Move toward target
  - Jika target out of chase_range → RETURN
  - Jika target in attack_range → ATTACK

ATTACK:
  - Stop, face target
  - Execute attack skill
  - Attack delay = skill delay / attack_speed_rate
  - Jika target out of attack_range → CHASE

HIT:
  - Play hit animation
  - Brief stun (200-500ms)

FLEE:
  - If HP < flee_threshold (20-30%)
  - Run toward spawn
  - Jika HP > flee_threshold → RETURN

RETURN:
  - Move back to spawn_position
  - Reset aggro table
  - Jika sampai spawn → IDLE

DIE:
  - Play death animation
  - Drop loot
  - Schedule respawn via RegenManager
```

## Reborn AI check:

```bash
rg "State\|state_\|Patrol\|Chase\|Flee\|Return\|Aggro\|aggro" game/ecs/systems/AISystem.* --type cpp
rg "TransitionState\|UpdateIdle\|UpdatePatrol\|UpdateChase" game/ecs/systems/AISystem.* --type cpp  
```

## Implementasi (jika belum ada):

```cpp
enum class AIState : uint8_t {
    Idle, Patrol, Chase, Attack, Hit, Flee, Die, Respawn, Return
};

struct AIStateConfig {
    float aggro_range = 10.0f;
    float attack_range = 2.5f;
    float chase_range = 30.0f;
    float flee_hp_pct = 0.20f;  // Flee at 20% HP
    float patrol_radius = 5.0f;
    float respawn_time = 10.0f;
    float attack_delay = 1.5f;  // Base attack speed
};

void AISystem::TransitionState(entt::registry& reg, entt::entity entity, AIState new_state) {
    auto& ai = reg.get<AIComponent>(entity);
    if (ai.state == new_state) return;
    ai.previous_state = ai.state;
    ai.state = new_state;
    ai.state_timer = 0.0f;
    
    switch (new_state) {
        case AIState::Patrol:
            ai.patrol_target = RandomPatrolPoint(ai.spawn_position, ai.config.patrol_radius);
            break;
        case AIState::Chase:
            // target already set by aggro
            break;
        case AIState::Flee:
            ai.flee_direction = glm::normalize(ai.spawn_position - ai.position);
            break;
        case AIState::Return:
            ai.return_target = ai.spawn_position;
            ai.ClearAggro();
            break;
        default: break;
    }
}
```

## Output

✅ Kembalikan: "FEEL-04 done: AI FSM aligned — X states implemented"
