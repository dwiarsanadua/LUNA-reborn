# FEEL-03 — Movement & Pathfinding Alignment

Lokasi: /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn
Build: cmake --build build/macos-debug -j$(sysctl -n hw.ncpu)

## Tugas

Old punya movement system dengan: WayPoint A* pathfinding (di Hero.cpp:Move_UsePath), COMPRESSEDPOS (WORD wx, wz), KyungGong dash mode, collision ellipsoid, knockback, fade move. Reborn pakai NavMesh float. Gerakan karakter terasa berbeda.

## Aturan Ketat

1. BACA Old reference: `Luna-Plus-Old/[Client]LUNA/Hero.cpp` — cari Move_UsePath, NextMove, ClearWayPoint
2. BACA Reborn: `game/ecs/systems/MovementSystem.cpp` — catat method yang sudah ada
3. ✅ Jika implementasi sudah setara — skip
4. 🔧 Jika beda — update MovementSystem

## Yang harus dicek

### Old Pathfinding (Hero.cpp):
```cpp
// Hero.cpp:1910 — Move_UsePath():
// 1. Cek line collision langsung
// 2. Jika blocked → MAP->PathFind() via CWayPointManager (A*)
// 3. Simpan waypoints
// 4. NextMove() pop waypoint → Move_Simple() → MP_MOVE_ONETARGET

// Hero.cpp:1980 — Move_Simple():
// Gerak langsung ke target point
// Kirim MP_MOVE_ONETARGET dengan COMPRESSEDPOS
```

### Reborn MovementSystem.cpp:
```bash
# Baca implementasi sekarang
cat game/ecs/systems/MovementSystem.cpp
cat server/map/systems/MovementSystem.cpp
```

### Yang harus di-update:

```cpp
// 1. WayPoint pathfinding (A* via NavMesh)
// Jika belum ada, tambah:
struct WayPoint {
    glm::vec3 position;
    bool is_teleport;
};
class MovementSystem {
    std::vector<WayPoint> FindPath(glm::vec3 from, glm::vec3 to);
    void MoveToNextWaypoint(entt::entity entity);
    // ...
};

// 2. KyungGong dash
// Jika Movement.fbs sudah punya kyung_gong_idx tapi handler belum:
void HandleKyungGong(entt::entity entity, uint16_t skill_idx);

// 3. Knockback (MP_MOVE_EFFECTMOVE)
void ApplyKnockback(entt::entity entity, glm::vec3 direction, float distance);

// 4. Fade move (MP_FADEMOVE_SYN) — teleport dengan fade effect
void HandleFadeMove(entt::entity entity, glm::vec3 target);
```

## Verifikasi dengan test:

```cpp
// Pathfinding test: dari A ke B, ada obstacle di tengah
// Old: pathfinder harus menemukan jalan di sekitar obstacle
// Reborn: apakah pathfinder menghasilkan jalan yang sama?
```

## Output

✅ Kembalikan: "FEEL-03 done: updated movement/pathfinding — X features aligned"
