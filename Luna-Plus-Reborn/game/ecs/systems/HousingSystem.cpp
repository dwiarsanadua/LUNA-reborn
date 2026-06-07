#include "HousingSystem.hpp"
#include "../components/Inventory.hpp"
#include "../components/Transform.hpp"
#include "../components/Tag.hpp"
#include <spdlog/spdlog.h>
#include <entt/entt.hpp>
#include <unordered_map>
#include <vector>

struct HouseData {
    uint32_t house_id;
    uint32_t owner_id;
    uint32_t map_instance;
    std::vector<uint32_t> furniture;
    float pos_x = 0, pos_z = 0;
};

static std::unordered_map<uint32_t, HouseData> g_houses;
static uint32_t g_next_house = 1;

void HousingSystem::BuyHouse(entt::registry& reg, entt::entity player, uint32_t house_id) {
    if (!reg.valid(player)) return;
    auto& inv = reg.get<Inventory>(player);
    uint32_t cost = 50000;
    if (inv.gold < cost) { spdlog::warn("Not enough gold"); return; }
    inv.gold -= cost;
    HouseData hd;
    hd.house_id = g_next_house++;
    hd.owner_id = static_cast<uint32_t>(player);
    hd.map_instance = house_id;
    hd.pos_x = static_cast<float>((hd.house_id * 10) % 100);
    hd.pos_z = static_cast<float>((hd.house_id * 7) % 100);
    g_houses[hd.house_id] = hd;
    spdlog::info("Player {} bought house {} (id={}) for {} gold",
                 static_cast<uint32_t>(player), house_id, hd.house_id, cost);
}

void HousingSystem::EnterHouse(entt::registry& reg, entt::entity player, uint32_t house_id) {
    auto it = g_houses.find(house_id);
    if (it == g_houses.end()) { spdlog::warn("House {} not found", house_id); return; }
    auto& hd = it->second;
    if (reg.valid(player) && reg.all_of<Transform>(player)) {
        reg.get<Transform>(player).position = glm::vec3(hd.pos_x, 0, hd.pos_z);
        spdlog::info("Player {} entered house {} at ({}, {})",
                     static_cast<uint32_t>(player), house_id, hd.pos_x, hd.pos_z);
    }
}

void HousingSystem::PlaceFurniture(entt::registry& reg, uint32_t house_id, uint32_t furniture_id) {
    auto it = g_houses.find(house_id);
    if (it == g_houses.end()) return;
    it->second.furniture.push_back(furniture_id);
    spdlog::info("Furniture {} placed in house {}", furniture_id, house_id);
}

bool HousingSystem::HasHouse(uint32_t player_id) const {
    for (auto& [id, hd] : g_houses)
        if (hd.owner_id == player_id) return true;
    return false;
}

void HousingSystem::Update(entt::registry& reg, float dt) {
    (void)reg; (void)dt;
}
