#pragma once
#include <entt/entt.hpp>
#include <cstdint>
#include <unordered_map>

class PetSystem {
public:
    void SummonPet(entt::registry& reg, entt::entity player, uint32_t pet_id);
    void FeedPet(entt::registry& reg, entt::entity pet);
    void DespawnPet(entt::registry& reg, entt::entity player);
    void Update(entt::registry& reg, float dt);
};
