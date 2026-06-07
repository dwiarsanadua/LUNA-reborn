#pragma once
#include <string>
#include <vector>
#include <cstdint>
#include <ctime>

struct DungeonInstance {
    uint32_t id = 0;
    int map_id = 0;
    std::string name;
    int min_level = 1, max_level = 999;
    int party_size_min = 1, party_size_max = 4;
    float time_limit = 1800.0f; // 30 min default
    int difficulty = 1;
    
    // Instance state
    bool active = false;
    float elapsed = 0;
    uint32_t creator_id = 0;
    std::vector<uint32_t> member_ids;
    bool completed = false;
    bool failed = false;
    
    // Rewards
    int xp_reward = 0;
    int gold_reward = 0;
    std::vector<uint32_t> item_rewards;
    int boss_hp_mult = 100; // percentage
    int boss_dmg_mult = 100;
};

struct DungeonLeaderboardEntry {
    std::string party_name;
    int dungeon_id = 0;
    float clear_time = 0;
    time_t date = 0;
    std::vector<std::string> member_names;
};

class DungeonSystem {
public:
    void Init();
    void Update(float dt);
    
    // Create/Join dungeon
    uint32_t CreateInstance(int map_id, uint32_t creator_id, const std::vector<uint32_t>& members);
    bool EnterInstance(uint32_t instance_id, uint32_t character_id);
    bool LeaveInstance(uint32_t instance_id, uint32_t character_id);
    void CompleteInstance(uint32_t instance_id);
    void FailInstance(uint32_t instance_id);
    
    // Queries
    DungeonInstance* GetInstance(uint32_t instance_id);
    std::vector<DungeonInstance> GetActiveInstances() const;
    std::vector<DungeonInstance> GetAvailableDungeons() const;
    bool IsInDungeon(uint32_t character_id) const;
    uint32_t FindInstanceByMember(uint32_t character_id) const;
    
    // Leaderboard
    void AddLeaderboardEntry(const DungeonLeaderboardEntry& entry);
    std::vector<DungeonLeaderboardEntry> GetLeaderboard(int dungeon_id, int limit = 10) const;
    
    // Dungeon definitions
    static std::vector<DungeonInstance> GetDungeonDefs();

private:
    std::vector<DungeonInstance> instances_;
    std::vector<DungeonLeaderboardEntry> leaderboard_;
    uint32_t next_id_ = 1;
    void CleanupInstances();
};
