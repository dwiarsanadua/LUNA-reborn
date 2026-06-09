#include "HousingSystem.hpp"
#include "../components/Inventory.hpp"
#include "../components/Transform.hpp"
#include "../components/Tag.hpp"
#include <spdlog/spdlog.h>
#include <entt/entt.hpp>
#include <unordered_map>
#include <vector>
#include <algorithm>

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

void HousingSystem::AddFurniture(entt::registry& reg, uint32_t house_id, uint32_t furniture_id) {
    PlaceFurniture(reg, house_id, furniture_id);
}

void HousingSystem::RemoveFurniture(entt::registry& reg, uint32_t house_id, uint32_t furniture_id) {
    auto it = g_houses.find(house_id);
    if (it == g_houses.end()) return;
    auto& hd = it->second;
    auto& furniture = hd.furniture;
    auto fit = std::find(furniture.begin(), furniture.end(), furniture_id);
    if (fit != furniture.end()) {
        furniture.erase(fit);
        spdlog::info("Furniture {} removed from house {}", furniture_id, house_id);
    }
}

bool HousingSystem::Decorate(entt::registry& reg, entt::entity player, uint32_t house_id, bool on) {
    auto it = g_houses.find(house_id);
    if (it == g_houses.end()) return false;
    auto& hd = it->second;
    if (hd.owner_id != static_cast<uint32_t>(player)) {
        spdlog::warn("Player {} is not owner of house {}", static_cast<uint32_t>(player), house_id);
        return false;
    }
    spdlog::info("Player {} {} decoration mode for house {}",
                  static_cast<uint32_t>(player), on ? "enters" : "exits", house_id);
    return true;
}

HouseInfo HousingSystem::GetHouseInfo(uint32_t house_id) const {
    auto it = g_houses.find(house_id);
    if (it != g_houses.end()) {
        HouseInfo info;
        info.house_id = it->second.house_id;
        info.owner_id = it->second.owner_id;
        info.map_instance = it->second.map_instance;
        info.furniture = it->second.furniture;
        info.deco_points = static_cast<uint32_t>(it->second.furniture.size() * 10);
        info.pos_x = it->second.pos_x;
        info.pos_z = it->second.pos_z;
        return info;
    }
    return HouseInfo{};
}

bool HousingSystem::HasHouse(uint32_t player_id) const {
    for (auto& [id, hd] : g_houses)
        if (hd.owner_id == player_id) return true;
    return false;
}

void HousingSystem::Update(entt::registry& reg, float dt) {
    (void)reg; (void)dt;
}
