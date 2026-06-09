#pragma once
#include <entt/entt.hpp>
#include <cstdint>
#include <string>
#include <vector>

struct HouseInfo {
    uint32_t house_id = 0;
    uint32_t owner_id = 0;
    uint32_t map_instance = 0;
    std::vector<uint32_t> furniture;
    uint32_t deco_points = 0;
    float pos_x = 0.0f;
    float pos_z = 0.0f;
};

class HousingSystem {
public:
    void BuyHouse(entt::registry& reg, entt::entity player, uint32_t house_id);
    void EnterHouse(entt::registry& reg, entt::entity player, uint32_t house_id);
    void PlaceFurniture(entt::registry& reg, uint32_t house_id, uint32_t furniture_id);
    void AddFurniture(entt::registry& reg, uint32_t house_id, uint32_t furniture_id);
    void RemoveFurniture(entt::registry& reg, uint32_t house_id, uint32_t furniture_id);
    bool Decorate(entt::registry& reg, entt::entity player, uint32_t house_id, bool on);
    HouseInfo GetHouseInfo(uint32_t house_id) const;
    bool HasHouse(uint32_t player_id) const;
    void Update(entt::registry& reg, float dt);
};
