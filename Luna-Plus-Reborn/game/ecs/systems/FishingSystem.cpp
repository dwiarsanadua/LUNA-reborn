#include "FishingSystem.hpp"
#include "../components/Inventory.hpp"
#include "../components/Transform.hpp"
#include "../components/Tag.hpp"
#include <spdlog/spdlog.h>
#include <random>
#include <unordered_map>

struct FishingSession {
    uint32_t player_id;
    float cast_time = 0;
    float wait_duration = 0;
    bool waiting = false;
    bool fish_on_line = false;
    float catch_window = 0;
};

static std::unordered_map<uint32_t, FishingSession> g_fishing;
static std::mt19937 fish_rng(std::random_device{}());

void FishingSystem::StartFishing(entt::registry& reg, entt::entity player) {
    uint32_t pid = static_cast<uint32_t>(player);
    if (g_fishing.find(pid) != g_fishing.end()) {
        spdlog::info("Already fishing");
        return;
    }
    FishingSession fs;
    fs.player_id = pid;
    fs.waiting = true;
    fs.wait_duration = 3.0f + std::uniform_real_distribution<float>(0, 5)(fish_rng);
    g_fishing[pid] = fs;
    spdlog::info("Player {} started fishing (wait {:.1f}s)", pid, fs.wait_duration);
}

bool FishingSystem::TryCatch(entt::registry& reg, entt::entity player) {
    uint32_t pid = static_cast<uint32_t>(player);
    auto it = g_fishing.find(pid);
    if (it == g_fishing.end()) return false;
    auto& fs = it->second;
    if (!fs.fish_on_line) return false;
    // Catch window: player must react within 2 seconds
    bool success = (fs.catch_window > 0);
    if (success) {
        uint32_t fish_table[] = {1001, 1002, 1003, 1004, 1005};
        uint32_t fish_id = fish_table[fish_rng() % 5];
        if (reg.valid(player) && reg.all_of<Inventory>(player)) {
            reg.get<Inventory>(player).AddItem(fish_id, 1);
            spdlog::info("Caught fish! Item {}", fish_id);
        }
    } else {
        spdlog::info("Fish got away!");
    }
    g_fishing.erase(it);
    return success;
}

void FishingSystem::Update(entt::registry& reg, float dt) {
    (void)reg;
    for (auto it = g_fishing.begin(); it != g_fishing.end(); ) {
        auto& fs = it->second;
        if (fs.waiting) {
            fs.cast_time += dt;
            if (fs.cast_time >= fs.wait_duration) {
                fs.waiting = false;
                fs.fish_on_line = true;
                fs.catch_window = 2.0f;
                spdlog::info("Fish on line! Press action to catch!");
            }
        }
        if (fs.fish_on_line) {
            fs.catch_window -= dt;
            if (fs.catch_window <= 0) {
                spdlog::info("Fish escaped");
                it = g_fishing.erase(it);
                continue;
            }
        }
        ++it;
    }
}
