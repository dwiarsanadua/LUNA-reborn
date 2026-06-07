#include "VehicleSystem.hpp"
#include "../components/Movement.hpp"
#include "../components/Tag.hpp"
#include <spdlog/spdlog.h>
#include <unordered_map>

struct MountData {
    uint32_t player_id;
    uint32_t vehicle_id;
    float speed_bonus;
    float duration = 0;
};

static std::unordered_map<uint32_t, MountData> g_mounted;

bool VehicleSystem::Mount(entt::registry& reg, entt::entity player, uint32_t vehicle_id) {
    uint32_t pid = static_cast<uint32_t>(player);
    if (!reg.valid(player)) return false;
    if (g_mounted.find(pid) != g_mounted.end()) return false;
    MountData md;
    md.player_id = pid;
    md.vehicle_id = vehicle_id;
    md.speed_bonus = 3.0f + (vehicle_id % 5) * 0.5f;
    g_mounted[pid] = md;
    if (reg.all_of<Movement>(player))
        reg.get<Movement>(player).current_speed += md.speed_bonus;
    spdlog::info("Player {} mounted vehicle {} (speed+{:.1f})", pid, vehicle_id, md.speed_bonus);
    return true;
}

void VehicleSystem::Dismount(entt::registry& reg, entt::entity player) {
    uint32_t pid = static_cast<uint32_t>(player);
    auto it = g_mounted.find(pid);
    if (it == g_mounted.end()) return;
    if (reg.valid(player) && reg.all_of<Movement>(player))
        reg.get<Movement>(player).current_speed -= it->second.speed_bonus;
    g_mounted.erase(it);
    spdlog::info("Player {} dismounted", pid);
}

bool VehicleSystem::IsMounted(entt::entity player) const {
    return g_mounted.find(static_cast<uint32_t>(player)) != g_mounted.end();
}

float VehicleSystem::GetSpeedBonus(entt::entity player) const {
    auto it = g_mounted.find(static_cast<uint32_t>(player));
    return (it != g_mounted.end()) ? it->second.speed_bonus : 0;
}

void VehicleSystem::Update(entt::registry& reg, float dt) {
    (void)reg;
    for (auto it = g_mounted.begin(); it != g_mounted.end(); ) {
        it->second.duration += dt;
        if (it->second.duration > 300.0f) { // auto-dismount after 5 min
            spdlog::info("Vehicle expired for player {}", it->first);
            it = g_mounted.erase(it);
        } else ++it;
    }
}
