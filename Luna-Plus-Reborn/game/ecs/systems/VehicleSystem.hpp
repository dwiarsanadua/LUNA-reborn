#pragma once
#include <entt/entt.hpp>
#include <cstdint>

class VehicleSystem {
public:
    bool Mount(entt::registry& reg, entt::entity player, uint32_t vehicle_id);
    void Dismount(entt::registry& reg, entt::entity player);
    bool IsMounted(entt::entity player) const;
    float GetSpeedBonus(entt::entity player) const;
    void Update(entt::registry& reg, float dt);
};
