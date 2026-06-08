#include "DungeonSystem.hpp"
#include <algorithm>
#include <cstdlib>
#include <spdlog/spdlog.h>

void DungeonSystem::Init() {
    instances_.clear();
    leaderboard_.clear();
    spdlog::info("DungeonSystem: initialized");
}

void DungeonSystem::Update(float dt) {
    for (auto& inst : instances_) {
        if (!inst.active || inst.completed || inst.failed) continue;
        inst.elapsed += dt;
        if (inst.time_limit > 0 && inst.elapsed >= inst.time_limit) {
            inst.failed = true;
            spdlog::info("Dungeon {}: time limit expired", inst.id);
        }
    }
    CleanupInstances();
}

uint32_t DungeonSystem::CreateInstance(int map_id, uint32_t creator_id, const std::vector<uint32_t>& members) {
    auto defs = GetDungeonDefs();
    DungeonInstance inst;
    inst.id = next_id_++;
    inst.map_id = map_id;
    inst.active = true;
    inst.creator_id = creator_id;
    inst.member_ids = members;
    inst.time_limit = 1800.0f;
    
    for (auto& d : defs) {
        if (d.map_id == map_id) {
            inst.name = d.name;
            inst.min_level = d.min_level;
            inst.max_level = d.max_level;
            inst.difficulty = d.difficulty;
            inst.xp_reward = d.xp_reward;
            inst.gold_reward = d.gold_reward;
            inst.item_rewards = d.item_rewards;
            break;
        }
    }
    
    instances_.push_back(inst);
    spdlog::info("Dungeon: created instance {} (map {})", inst.id, map_id);
    return inst.id;
}

bool DungeonSystem::EnterInstance(uint32_t instance_id, uint32_t character_id) {
    auto* inst = GetInstance(instance_id);
    if (!inst || !inst->active) return false;
    for (auto m : inst->member_ids) if (m == character_id) return true;
    inst->member_ids.push_back(character_id);
    return true;
}

bool DungeonSystem::LeaveInstance(uint32_t instance_id, uint32_t character_id) {
    auto* inst = GetInstance(instance_id);
    if (!inst) return false;
    inst->member_ids.erase(
        std::remove(inst->member_ids.begin(), inst->member_ids.end(), character_id),
        inst->member_ids.end());
    if (inst->member_ids.empty()) inst->active = false;
    return true;
}

void DungeonSystem::CompleteInstance(uint32_t instance_id) {
    auto* inst = GetInstance(instance_id);
    if (!inst) return;
    inst->completed = true;
    inst->active = false;
    spdlog::info("Dungeon {}: completed in {:.1f}s", instance_id, inst->elapsed);
}

void DungeonSystem::FailInstance(uint32_t instance_id) {
    auto* inst = GetInstance(instance_id);
    if (!inst) return;
    inst->failed = true;
    inst->active = false;
}

DungeonInstance* DungeonSystem::GetInstance(uint32_t instance_id) {
    for (auto& inst : instances_) if (inst.id == instance_id) return &inst;
    return nullptr;
}

std::vector<DungeonInstance> DungeonSystem::GetActiveInstances() const {
    std::vector<DungeonInstance> result;
    for (auto& inst : instances_) if (inst.active) result.push_back(inst);
    return result;
}

std::vector<DungeonInstance> DungeonSystem::GetAvailableDungeons() const {
    return GetDungeonDefs();
}

bool DungeonSystem::IsInDungeon(uint32_t character_id) const {
    return FindInstanceByMember(character_id) > 0;
}

uint32_t DungeonSystem::FindInstanceByMember(uint32_t character_id) const {
    for (auto& inst : instances_) {
        if (!inst.active) continue;
        for (auto m : inst.member_ids) if (m == character_id) return inst.id;
    }
    return 0;
}

void DungeonSystem::ApplyEntranceResponse(uint32_t instance_id, uint32_t template_id,
                                          uint16_t map_id, uint32_t time_limit_sec) {
    DungeonInstance inst;
    inst.id = instance_id;
    inst.map_id = static_cast<int>(map_id ? map_id : template_id);
    inst.name = "Dungeon " + std::to_string(template_id);
    inst.time_limit = static_cast<float>(time_limit_sec > 0 ? time_limit_sec : 1800);
    inst.active = true;
    inst.creator_id = 0;
    instances_.push_back(inst);
    if (instance_id >= next_id_) next_id_ = instance_id + 1;
}

void DungeonSystem::ApplyInfoResponse(uint32_t instance_id, uint8_t state,
                                      uint32_t elapsed_sec, bool boss_active) {
    if (auto* inst = GetInstance(instance_id)) {
        inst->elapsed = static_cast<float>(elapsed_sec);
        inst->active = state != 3 && state != 4;
        inst->completed = state == 3;
        inst->failed = state == 4;
        (void)boss_active;
    }
}

void DungeonSystem::AddLeaderboardEntry(const DungeonLeaderboardEntry& entry) {
    leaderboard_.push_back(entry);
}

std::vector<DungeonLeaderboardEntry> DungeonSystem::GetLeaderboard(int dungeon_id, int limit) const {
    std::vector<DungeonLeaderboardEntry> result;
    for (auto& e : leaderboard_) {
        if (e.dungeon_id == dungeon_id) result.push_back(e);
    }
    std::sort(result.begin(), result.end(), [](auto& a, auto& b) {
        return a.clear_time < b.clear_time;
    });
    if ((int)result.size() > limit) result.resize(limit);
    return result;
}

void DungeonSystem::CleanupInstances() {
    instances_.erase(
        std::remove_if(instances_.begin(), instances_.end(),
            [](auto& i) { return !i.active && (i.completed || i.failed); }),
        instances_.end());
}

std::vector<DungeonInstance> DungeonSystem::GetDungeonDefs() {
    std::vector<DungeonInstance> defs;
    DungeonInstance d;
    
    d = DungeonInstance{}; d.map_id = 10; d.name = "Goblin Cave"; d.min_level = 10; d.max_level = 30;
    d.time_limit = 900.0f; d.xp_reward = 1000; d.gold_reward = 500; d.item_rewards = {1001}; defs.push_back(d);
    
    d = DungeonInstance{}; d.map_id = 11; d.name = "Ancient Ruins"; d.min_level = 20; d.max_level = 50;
    d.time_limit = 1200.0f; d.xp_reward = 3000; d.gold_reward = 1500; d.item_rewards = {1002, 1003}; defs.push_back(d);
    
    d = DungeonInstance{}; d.map_id = 12; d.name = "Dragon's Lair"; d.min_level = 40; d.max_level = 80;
    d.time_limit = 1800.0f; d.xp_reward = 10000; d.gold_reward = 5000; d.item_rewards = {1004, 1005}; defs.push_back(d);
    
    d = DungeonInstance{}; d.map_id = 13; d.name = "Shadow Realm"; d.min_level = 60; d.max_level = 99;
    d.time_limit = 2400.0f; d.xp_reward = 25000; d.gold_reward = 10000; d.item_rewards = {1006}; defs.push_back(d);
    
    d = DungeonInstance{}; d.map_id = 14; d.name = "Limit Challenge"; d.min_level = 10; d.max_level = 99;
    d.time_limit = 600.0f; d.xp_reward = 5000; d.gold_reward = 2000; d.item_rewards = {1007}; defs.push_back(d);
    
    return defs;
}
