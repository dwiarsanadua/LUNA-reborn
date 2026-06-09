#pragma once
#include <entt/entt.hpp>
#include <cstdint>

class ComboSystem {
public:
    void Update(entt::registry& registry, float dt);

    void RegisterHit(entt::registry& registry, entt::entity attacker, entt::entity target);
    void ResetCombo(entt::registry& registry, entt::entity attacker);

    int32_t GetComboCount(entt::registry& registry, entt::entity attacker) const;
    int32_t GetMaxCombo(entt::registry& registry, entt::entity attacker) const;
    float GetComboMultiplier(entt::registry& registry, entt::entity attacker) const;
    float GetComboProgress(entt::registry& registry, entt::entity attacker) const;

    static constexpr float COMBO_TIMEOUT = 3.0f;
    static constexpr float MULTIPLIER_PER_HIT = 0.02f;
    static constexpr float MAX_MULTIPLIER = 2.0f;
    static constexpr int32_t COMBO_STAGE_1 = 5;
    static constexpr int32_t COMBO_STAGE_2 = 10;
    static constexpr int32_t COMBO_STAGE_3 = 20;
    static constexpr int32_t COMBO_STAGE_4 = 50;
};
