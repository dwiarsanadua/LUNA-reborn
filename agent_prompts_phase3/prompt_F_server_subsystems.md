# Agent F — Missing Server Subsystems 🟡 HIGH

Lokasi: /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn
Build: cmake --build build/macos-debug -j$(sysctl -n hw.ncpu)

## Masalah

Dari 200+ file Old [Server]Map/, Reborn baru punya ~12 system files. Beberapa subsystem penting belum di-port:
1. **LootingManager** — PK looting system (bad fame based)
2. **ExchangeManager** — Player-to-player trading
3. **FieldBossMonsterManager** — Field boss spawn management
4. **BossMonsterInfo** — Boss monster phase/skill data
5. **Grid system** — Spatial grid for entity queries
6. **Recall system** — Full NPC/skill recall flow (request→ask→confirm→execute)

## File yang harus dibuat

### server/map/systems/LootingSystem.cpp [BARU]

Pseudo-code:
```cpp
class LootingSystem {
public:
    // PK loot check
    bool ShouldLoot(const CharacterStats& victim, const CharacterStats& attacker);
    
    // Create loot room
    void CreateLootRoom(entt::entity victim, entt::entity attacker);
    
    // Player loots an item
    bool LootItem(entt::entity looter, uint32_t loot_room_id, uint32_t item_slot);
    
    // Auto-loot (if victim has bad fame)
    void AutoLoot(entt::entity victim, entt::entity attacker);
    
    // Timeout check
    void Process(float dt);

private:
    struct LootRoom {
        uint32_t id;
        entt::entity victim;
        entt::entity attacker;
        std::vector<LootItem> items;
        float time_remaining;
    };
    
    float GetLootChance(int32_t bad_fame);
    int GetLootItemCount(int32_t bad_fame);
    float GetWearItemLootRatio(int32_t bad_fame);
    // Old: BadFame 0→0%, 1000→50%, 3000→80%, 10000→100%
};
```

### server/map/systems/TradingSystem.cpp [BARU] / [UPDATE]

Update TradingSystem yang sudah ada (partial):
```cpp
// Old ExchangeManager memiliki flow:
// 1. CanApplyExchange() — validasi state (no battle, no existing exchange)
// 2. CreateExchangeRoom() — alokasi room
// 3. AddItem() — kedua pihak menambah item
// 4. Lock() — kunci item
// 5. InputMoney() — tambah gold
// 6. Confirm() — finalisasi
// 7. CancelExchange() — batalkan
// 8. UserLogOut() — cleanup otomatis

// Implementasi mengikuti pola yang sama
struct ExchangeRoom {
    uint32_t id;
    entt::entity player1, player2;
    std::vector<ExchangeItem> items1, items2;
    uint32_t gold1 = 0, gold2 = 0;
    bool locked1 = false, locked2 = false;
    bool confirmed1 = false, confirmed2 = false;
    float timeout = 30.0f;
};
```

### server/map/systems/FieldBossSystem.cpp [BARU]

```cpp
class FieldBossSystem {
public:
    void LoadFieldBossData(const std::string& json_path);
    void Update(float dt);
    
    void SpawnFieldBoss(uint32_t boss_id, int map_id);
    void OnFieldBossDeath(entt::entity boss, entt::entity killer);
    
    // Boss phase management
    struct FieldBossData {
        uint32_t id;
        uint32_t monster_id;
        int map_id;
        glm::vec3 spawn_position;
        float respawn_hours;
        uint32_t min_party_size;
        std::vector<uint32_t> loot_table;
        std::vector<BossPhase> phases;
    };

private:
    std::vector<FieldBossData> field_bosses_;
    std::unordered_map<uint32_t, float> respawn_timers_; // boss_id → remaining hours
    
    void BroadcastFieldBossSpawn(uint32_t boss_id, int map_id, const glm::vec3& pos);
    void BroadcastFieldBossDeath(uint32_t boss_id, const std::string& killer_name);
};
```

### server/map/systems/GridSystem.cpp [UPDATE]

Grid system sudah ada secara partial. Update dengan Old-aligned queries:
```cpp
// Tambah method:
std::vector<entt::entity> GetTargetsInRange(
    const glm::vec3& center, float radius, 
    entt::registry& registry, 
    const std::unordered_set<ObjectKind>& kinds
);
```

### server/map/systems/RecallSystem.cpp [BARU]

```cpp
class RecallSystem {
public:
    // Request recall (dari NPC atau skill)
    void RequestRecall(uint32_t character_id, RecallTarget target);
    
    // Accept recall
    bool AcceptRecall(uint32_t character_id, uint32_t key);
    
    struct RecallRequest {
        uint32_t id;
        uint32_t character_id;
        uint32_t target_char_id;
        int target_map_id;
        glm::vec3 target_position;
        float timeout;
        uint32_t key;          // Verification key
        uint32_t skill_id;     // Yang digunakan
    };
    
    // Old flow: Request→Ask→Confirm→Execute→Notify
    // Dengan state validation, skill consumption, cross-server transport
};
```

## Aturan

1. BUAT file baru dengan header guards (pragma once)
2. IKUTI style ECS pattern yang sudah ada
3. Wire ke CMakeLists.txt jika perlu (server/map/CMakeLists.txt)
4. JANGAN build atau compile
5. ✅ Kembalikan "Agent F done: missing server subsystems created"
