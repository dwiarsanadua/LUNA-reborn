#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <unordered_set>
#include <entt/entt.hpp>

class Database;

enum class MapTriggerType : uint8_t {
    RegionEnter = 0,
    Teleport = 1,
    QuestStart = 2,
    SpawnMonster = 3,
    Script = 4,
};

struct MapTriggerDef {
    uint32_t trigger_id = 0;
    int map_id = 0;
    MapTriggerType type = MapTriggerType::RegionEnter;
    float x = 0, z = 0, radius = 8.0f;
    uint32_t param0 = 0;
    uint32_t param1 = 0;
    std::string script_path;
    bool repeatable = false;
};

class TriggerSystem {
public:
    void LoadForMap(Database& db, int map_id);
    void LoadSeedFile(int map_id, const std::string& json_path);
    void Update(class MapServer* server, entt::entity player, float player_x, float player_z);

    const std::vector<MapTriggerDef>& GetTriggers() const { return triggers_; }

private:
    std::vector<MapTriggerDef> triggers_;
    std::unordered_set<uint32_t> fired_once_;
};
