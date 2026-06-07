#pragma once
#include <entt/entt.hpp>

class FishingSystem {
public:
    void StartFishing(entt::registry& reg, entt::entity player);
    bool TryCatch(entt::registry& reg, entt::entity player);
    void Update(entt::registry& reg, float dt);
};
