# Agent F01 — Collision → Movement Integration (Phase 2)

Lokasi: /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn
Build: cmake --build build/macos-debug -j$(sysctl -n hw.ncpu)

## Konteks

EngineMap.cpp sudah punya CollisionLine check. PhysicsWorld sudah punya sphere/ellipsoid collision. Tapi movement system belum fully di-integrasikan dengan collision detection.

## File yang harus diubah

### client/engine/EngineMap.cpp [UPDATE]

**🟡 Integrasi collision check ke movement** (Severity: M, Effort: 3 days)

Pseudo-code:
```cpp
bool EngineMap::TryMove(glm::vec3 from, glm::vec3 to, float radius, entt::entity entity) {
    // 1. Cek line collision antara from dan to
    if (CollisionLine(from, to)) {
        // Hitung posisi slide (slide along wall)
        glm::vec3 slide = CalculateSlide(from, to);
        if (!CollisionLine(from, slide)) {
            // Update posisi ke slide position
            SetEntityPosition(entity, slide);
            return true; // Partial move (slide)
        }
        return false; // Blocked
    }
    
    // 2. Cek ellipsoid collision dengan entity lain
    if (CollisionTestMovingEllipsoid(from, to, radius)) {
        return false; // Blocked by entity
    }
    
    // 3. Cek map boundary
    auto bounds = GetMapBounds(current_map_);
    if (to.x < bounds.min_x || to.x > bounds.max_x ||
        to.z < bounds.min_z || to.z > bounds.max_z) {
        return false; // Out of bounds
    }
    
    // Move allowed
    SetEntityPosition(entity, to);
    return true;
}

glm::vec3 EngineMap::CalculateSlide(const glm::vec3& from, const glm::vec3& to) {
    // Hitung slide vector ketika kena wall
    glm::vec3 dir = glm::normalize(to - from);
    // Project ke bidang wall normal
    // ... implementasi sliding physics
    return from + dir * glm::distance(from, to) * 0.5f; // Simplified
}
```

### client/gameobjects/Hero.cpp [UPDATE]

**🟡 Wire collision ke hero movement** (Severity: M, Effort: 2 days)
- Di Update(), panggil EngineMap::TryMove() sebelum update posisi hero
- Jika TryMove return false, set velocity = 0

### game/ecs/systems/MovementSystem.cpp [UPDATE]

**🟡 Server-side collision check** (Severity: M, Effort: 3 days)
- Di MapServer movement handler, validasi pergerakan dengan collision map
- Gunakan navmesh untuk pathfinding jika waypoint system aktif

## Aturan

1. CollisionLine sudah ada di EngineMap.cpp — jangan buat ulang
2. PhysicsWorld.h sudah punya method collision — pakai yang ada
3. JANGAN build atau compile
4. ✅ Kembalikan "Agent F01 done: collision integrated with movement"
