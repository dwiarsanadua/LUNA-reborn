#pragma once
#include <entt/entt.hpp>

class SkillSystem {
public:
    bool CanUseSkill(entt::registry& reg, entt::entity caster, uint16_t skill_id);
    void UseSkill(entt::registry& reg, entt::entity caster, entt::entity target, uint16_t skill_id);
    void ApplyBuff(entt::registry& reg, entt::entity target, uint16_t buff_id, float duration);
    void UpdateCooldowns(entt::registry& reg, float dt);
    void UpdateBuffs(entt::registry& reg, float dt);
};
