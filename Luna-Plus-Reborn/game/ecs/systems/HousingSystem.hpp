#pragma once
#include <entt/entt.hpp>
#include <cstdint>

class HousingSystem {
public:
    void BuyHouse(entt::registry& reg, entt::entity player, uint32_t house_id);
    void EnterHouse(entt::registry& reg, entt::entity player, uint32_t house_id);
    void PlaceFurniture(entt::registry& reg, uint32_t house_id, uint32_t furniture_id);
    bool HasHouse(uint32_t player_id) const;
    void Update(entt::registry& reg, float dt);
};
