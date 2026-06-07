#include "DungeonManager.hpp"
#include <algorithm>
#include <cstdlib>
#include <spdlog/spdlog.h>

DungeonManager::DungeonManager() {}
DungeonManager::~DungeonManager() { Shutdown(); }

void DungeonManager::Init() {
    std::lock_guard<std::mutex> lock(mutex_);
    instances_.clear();
    leaderboard_.clear();
    spdlog::info("DungeonManager: initialized");
}

void DungeonManager::Update(float dt) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    for (auto& inst : instances_) {
        if (!inst->active || inst->completed || inst->failed) continue;
        inst->elapsed += dt;
        
        // Check time limit
        if (inst->time_limit > 0 && inst->elapsed >= inst->time_limit) {
            inst->failed = true;
            spdlog::info("Dungeon {}: time limit expired ({:.0f}s)", inst->id, inst->time_limit);
        }
        
        // Limit Dungeon wave progression
        if (inst->is_limit_dungeon && inst->active) {
            int kills_per_wave = inst->enemies_to_kill / inst->wave_max;
            int expected_wave = std::min(inst->wave_max, (inst->enemies_killed / kills_per_wave) + 1);
            if (expected_wave > inst->wave_current) {
                inst->wave_current = expected_wave;
                spdlog::info("Dungeon {}: wave {} started", inst->id, inst->wave_current);
            }
            if (inst->enemies_killed >= inst->enemies_to_kill) {
                inst->completed = true;
                inst->active = false;
                spdlog::info("Dungeon {}: LIMIT DUNGEON COMPLETED!", inst->id);
            }
        }
    }
    
    cleanup_timer_ += dt;
    if (cleanup_timer_ >= 60.0f) {
        cleanup_timer_ = 0;
        CleanupInstances();
    }
}

uint32_t DungeonManager::CreateInstance(int map_id, uint32_t owner_id, const std::vector<uint32_t>& members) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto inst = std::make_unique<DungeonInstance>();
    inst->id = next_id_++;
    inst->map_id = map_id;
    inst->name = "Dungeon_" + std::to_string(inst->id);
    inst->owner_id = owner_id;
    inst->member_ids = members;
    inst->registry = std::make_unique<entt::registry>();
    inst->time_limit = 1800.0f;
    
    uint32_t id = inst->id;
    instances_.push_back(std::move(inst));
    spdlog::info("DungeonManager: created instance {} (map {})", id, map_id);
    return id;
}

uint32_t DungeonManager::CreateLimitDungeon(uint32_t owner_id, const std::vector<uint32_t>& members, int target_kills) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto inst = std::make_unique<DungeonInstance>();
    inst->id = next_id_++;
    inst->map_id = 14; // Limit dungeon map
    inst->name = "Limit Dungeon";
    inst->owner_id = owner_id;
    inst->member_ids = members;
    inst->registry = std::make_unique<entt::registry>();
    inst->is_limit_dungeon = true;
    inst->time_limit = 600.0f; // 10 minutes
    inst->enemies_to_kill = target_kills;
    inst->wave_max = 10;
    inst->xp_reward = target_kills * 100;
    inst->gold_reward = target_kills * 50;
    
    uint32_t id = inst->id;
    instances_.push_back(std::move(inst));
    spdlog::info("DungeonManager: created LIMIT DUNGEON {} ({} kills)", id, target_kills);
    return id;
}

bool DungeonManager::DestroyInstance(uint32_t instance_id) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = std::find_if(instances_.begin(), instances_.end(),
        [&](auto& i) { return i->id == instance_id; });
    if (it != instances_.end()) {
        instances_.erase(it);
        return true;
    }
    return false;
}

DungeonInstance* DungeonManager::GetInstance(uint32_t instance_id) {
    std::lock_guard<std::mutex> lock(mutex_);
    for (auto& inst : instances_) {
        if (inst->id == instance_id) return inst.get();
    }
    return nullptr;
}

DungeonInstance* DungeonManager::GetInstance(uint32_t instance_id) const {
    // This should not be called from const contexts;
    // non-const version handles locking.
    // const overload provided for compile-time compatibility.
    return const_cast<DungeonManager*>(this)->GetInstance(instance_id);
}

bool DungeonManager::AddPlayer(uint32_t instance_id, uint32_t player_id) {
    auto* inst = GetInstance(instance_id);
    if (!inst) return false;
    for (auto m : inst->member_ids) if (m == player_id) return true; // Already in
    inst->member_ids.push_back(player_id);
    return true;
}

bool DungeonManager::RemovePlayer(uint32_t instance_id, uint32_t player_id) {
    auto* inst = GetInstance(instance_id);
    if (!inst) return false;
    inst->member_ids.erase(
        std::remove(inst->member_ids.begin(), inst->member_ids.end(), player_id),
        inst->member_ids.end());
    return true;
}

uint32_t DungeonManager::FindPlayerInstance(uint32_t player_id) const {
    std::lock_guard<std::mutex> lock(mutex_);
    for (auto& inst : instances_) {
        if (!inst->active) continue;
        for (auto m : inst->member_ids) if (m == player_id) return inst->id;
    }
    return 0;
}

void DungeonManager::CompleteInstance(uint32_t instance_id) {
    auto* inst = GetInstance(instance_id);
    if (!inst) return;
    inst->completed = true;
    inst->active = false;
    spdlog::info("Dungeon {}: completed in {:.1f}s", instance_id, inst->elapsed);
}

void DungeonManager::FailInstance(uint32_t instance_id) {
    auto* inst = GetInstance(instance_id);
    if (!inst) return;
    inst->failed = true;
    inst->active = false;
}

void DungeonManager::OnEnemyKilled(uint32_t instance_id) {
    auto* inst = GetInstance(instance_id);
    if (!inst || !inst->is_limit_dungeon) return;
    inst->enemies_killed++;
}

int DungeonManager::GetWave(uint32_t instance_id) const {
    auto* inst = GetInstance(instance_id);
    if (!inst) return 0;
    return inst->wave_current;
}

void DungeonManager::AddLeaderboardEntry(const DungeonLeaderboardEntry& entry) {
    std::lock_guard<std::mutex> lock(mutex_);
    leaderboard_.push_back(entry);
}

std::vector<DungeonLeaderboardEntry> DungeonManager::GetLeaderboard(int dungeon_id, int limit) const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<DungeonLeaderboardEntry> result;
    for (auto& e : leaderboard_) {
        if (e.dungeon_id == dungeon_id || dungeon_id == 0) result.push_back(e);
    }
    std::sort(result.begin(), result.end(), [](auto& a, auto& b) {
        if (a.waves_cleared != b.waves_cleared) return a.waves_cleared > b.waves_cleared;
        return a.clear_time < b.clear_time;
    });
    if ((int)result.size() > limit) result.resize(limit);
    return result;
}

std::vector<uint32_t> DungeonManager::GetActiveInstanceIds() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<uint32_t> ids;
    for (auto& inst : instances_) if (inst->active) ids.push_back(inst->id);
    return ids;
}

void DungeonManager::CleanupInstances() {
    instances_.erase(
        std::remove_if(instances_.begin(), instances_.end(),
            [](auto& i) { return !i->active && (i->completed || i->failed); }),
        instances_.end());
}

void DungeonManager::Shutdown() {
    std::lock_guard<std::mutex> lock(mutex_);
    instances_.clear();
    leaderboard_.clear();
    spdlog::info("DungeonManager: shutdown");
}

void DungeonManager::SpawnWave(DungeonInstance& inst) {
    (void)inst;
    // In production: spawn monsters into inst.registry
}
