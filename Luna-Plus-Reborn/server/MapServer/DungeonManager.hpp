#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <mutex>
#include <memory>
#include <entt/entt.hpp>

// Thread-safe dungeon instance manager for MapServer.
// Each dungeon instance runs in its own ECS registry (scene isolation).

struct DungeonInstance {
    uint32_t id = 0;
    int map_id = 0;
    std::string name;
    uint32_t owner_id = 0;     // Party leader / creator
    std::vector<uint32_t> member_ids;
    
    // ECS registry for this instance (isolated scene)
    std::unique_ptr<entt::registry> registry;
    
    // Timers
    float time_limit = 1800.0f; // 30 minutes
    float elapsed = 0;
    bool completed = false;
    bool failed = false;
    bool active = true;
    
    // Difficulty
    int difficulty = 1;
    float hp_mult = 1.0f;
    float dmg_mult = 1.0f;
    int xp_reward = 0;
    int gold_reward = 0;
    
    // Limit Dungeon specific
    bool is_limit_dungeon = false;
    int wave_current = 0;
    int wave_max = 10;
    int enemies_killed = 0;
    int enemies_to_kill = 50;
};

struct DungeonLeaderboardEntry {
    std::string party_name;
    int dungeon_id = 0;
    float clear_time = 0;
    int waves_cleared = 0;
    int enemies_killed = 0;
    int64_t timestamp = 0;
    std::vector<std::string> member_names;
};

class DungeonManager {
public:
    DungeonManager();
    ~DungeonManager();
    
    void Init();
    void Update(float dt);
    void Shutdown();
    
    // Instance management
    uint32_t CreateInstance(int map_id, uint32_t owner_id, const std::vector<uint32_t>& members);
    bool DestroyInstance(uint32_t instance_id);
    DungeonInstance* GetInstance(uint32_t instance_id);
    
    // Player management
    bool AddPlayer(uint32_t instance_id, uint32_t player_id);
    bool RemovePlayer(uint32_t instance_id, uint32_t player_id);
    uint32_t FindPlayerInstance(uint32_t player_id) const;
    DungeonInstance* GetInstance(uint32_t instance_id) const;
    
    // Completion
    void CompleteInstance(uint32_t instance_id);
    void FailInstance(uint32_t instance_id);
    
    // Limit Dungeon
    uint32_t CreateLimitDungeon(uint32_t owner_id, const std::vector<uint32_t>& members, int target_kills = 50);
    void OnEnemyKilled(uint32_t instance_id);
    int GetWave(uint32_t instance_id) const;
    
    // Leaderboard
    void AddLeaderboardEntry(const DungeonLeaderboardEntry& entry);
    std::vector<DungeonLeaderboardEntry> GetLeaderboard(int dungeon_id, int limit = 10) const;
    
    // Queries
    int GetActiveCount() const { return (int)instances_.size(); }
    std::vector<uint32_t> GetActiveInstanceIds() const;

private:
    std::vector<std::unique_ptr<DungeonInstance>> instances_;
    std::vector<DungeonLeaderboardEntry> leaderboard_;
    uint32_t next_id_ = 1;
    mutable std::mutex mutex_;
    float cleanup_timer_ = 0;
    
    void CleanupInstances();
    void SpawnWave(DungeonInstance& inst);
};
