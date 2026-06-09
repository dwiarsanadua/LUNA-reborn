# Agent B01 — Server-Side Validation & Anti-Cheat

Lokasi: /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn
Build: cmake --build build/macos-debug -j$(sysctl -n hw.ncpu)

## Konteks

Security layer Old punya HackShield + NProtect (kernel-level anti-cheat). Reborn tidak bisa port itu, jadi mitigasi pengganti adalah server-side validation. RateLimiter dan BcryptUtils sudah ada, perlu di-wire ke alur utama server.

## File yang harus diubah

### server/shared/ValidationSystem.hpp [BARU]

```cpp
#pragma once
#include <cstdint>
#include <glm/glm.hpp>

struct CharacterStats; // forward declare

class ValidationSystem {
public:
    // Movement validation: cek speed hack
    static bool ValidateMovement(
        const glm::vec3& from,
        const glm::vec3& to,
        float delta_time,
        float max_speed,
        uint32_t entity_id
    );

    // Damage validation: cek damage berada dalam range yang wajar
    static bool ValidateDamage(
        int32_t reported_damage,
        const CharacterStats& attacker,
        const CharacterStats& defender,
        uint16_t skill_id
    );

    // Position validation: cek entity tidak di luar map bounds
    static bool ValidatePosition(
        const glm::vec3& pos,
        int32_t map_id
    );

    // Item/duplication validation
    static bool ValidateItemAction(
        uint32_t character_id,
        uint32_t item_id,
        uint16_t count,
        const std::string& action
    );
};
```

### server/shared/ValidationSystem.cpp [BARU]

Implementasi:
- **ValidateMovement**: hitung jarak Euclidean, bandingkan dengan max_speed * dt
  - Jika jarak > max_speed * dt * 1.5f → log warning, return false
  - Track posisi sebelumnya per entity_id untuk anti-warp
- **ValidateDamage**: panggil CombatSystem::CalculateDamage untuk expected range
  - expected ±30% tolerance
- **ValidatePosition**: cek terhadap map_boundaries dari database
- **ValidateItemAction**: cek inventory client-side vs server-side

### server/map/MapServer.cpp [UPDATE]

**🔴 Wire validation ke movement handler** (Severity: H, Effort: 2 days)
- Di `HandleMoveWalk/Run`, panggil `ValidationSystem::ValidateMovement()`
- Jika validasi gagal: kirim MOVE_NACK + jangan update posisi

**🔴 Wire validation ke combat handler** (Severity: H, Effort: 2 days)
- Di `HandleCombatAttack()`, panggil `ValidationSystem::ValidateDamage()`
- Jika damage tidak valid: kirim COMBAT_NACK + log

**🟡 Wire RateLimiter ke MapServer** (Severity: M, Effort: 1 day)
- RateLimiter sudah ada di server/shared/
- Wire ke MapServer: di awal HandlePacket(), panggil rateLimiter.Allow(conn_id)
- Jika rate limit exceeded: disconnect

**🟡 Wire brute-force protection ke AgentServer** (Severity: M, Effort: 1 day)
- BruteForceManager sudah ada di AgentServer.cpp (partial)
- Perbaiki: count failed login per IP, lockout setelah 5 gagal dalam 1 menit
- Implement sliding window untuk IP-based + account-based lockout

### server/agent/AgentServer.cpp [UPDATE]

**🟡 Wire bcrypt untuk login** (Severity: H, Effort: 1 day)
- Di handleLogin, ganti password comparison dengan BcryptUtils::verifyPassword
- Saat create user, hash password dengan BcryptUtils::hashPassword

## Aturan

1. BACA file yang tercantum sebelum mengubah
2. IKUTI style yang sudah ada (snake_case functions, PascalCase classes)
3. JANGAN build atau compile
4. ✅ Kembalikan pesan "Agent B01 done: server validation wired"
