#include "SkillSystem.hpp"
#include "GameDataDB.hpp"
#include "../components/SkillBook.hpp"
#include "../components/CharacterStats.hpp"
#include "../components/Tag.hpp"

#include <spdlog/spdlog.h>

bool SkillSystem::CanUseSkill(entt::registry& reg, entt::entity caster, uint16_t skill_id) {
    if (!reg.valid(caster)) return false;
    if (!reg.all_of<SkillBook, CharacterStats>(caster)) return false;
    auto& book = reg.get<SkillBook>(caster);
    if (!book.HasSkill(skill_id)) return false;
    auto& stats = reg.get<CharacterStats>(caster);
    if (stats.hp <= 0) return false;

    GameDataDB db;
    if (!db.Open(GAME_DATA_PATH)) return false;
    auto sd = db.GetSkill(skill_id);
    db.Close();
    if (sd.skill_id == 0) return false;
    if (stats.mp < sd.cost_mp) return false;
    if (stats.hp <= sd.cost_hp) return false;
    return true;
}

void SkillSystem::UseSkill(entt::registry& reg, entt::entity caster,
                            entt::entity target, uint16_t skill_id) {
    if (!CanUseSkill(reg, caster, skill_id)) return;

    GameDataDB db;
    if (!db.Open(GAME_DATA_PATH)) return;
    auto sd = db.GetSkill(skill_id);
    db.Close();
    if (sd.skill_id == 0) return;

    auto& stats = reg.get<CharacterStats>(caster);
    stats.hp -= sd.cost_hp;
    stats.mp -= sd.cost_mp;

    if (reg.valid(target) && reg.all_of<CharacterStats>(target)) {
        auto& tgt = reg.get<CharacterStats>(target);
        int32_t damage = sd.damage_fixed + static_cast<int32_t>(stats.physic_attack * sd.damage_mult);
        damage = std::max(1, damage - (int32_t)tgt.physic_defense / 2);
        tgt.hp -= damage;
        spdlog::info("Skill {} hits for {} damage", sd.name, damage);
        if (tgt.hp <= 0) {
            tgt.hp = 0;
            spdlog::info("Target killed by {}", sd.name);
        }
    }

    auto& book = reg.get<SkillBook>(caster);
    for (auto& sk : book.skills) {
        if (sk.skill_id == skill_id) {
            sk.cooldown_until = sd.cooldown_ms;
            break;
        }
    }
}

void SkillSystem::ApplyBuff(entt::registry& reg, entt::entity target,
                             uint16_t buff_id, float duration) {
    spdlog::info("Buff {} applied to entity {} for {}s", buff_id,
                 static_cast<uint32_t>(target), duration);
}

void SkillSystem::UpdateCooldowns(entt::registry& reg, float dt) {
    auto view = reg.view<SkillBook>();
    for (auto entity : view) {
        auto& book = view.get<SkillBook>(entity);
        for (auto& skill : book.skills) {
            if (skill.cooldown_until > 0) {
                uint32_t dec = static_cast<uint32_t>(dt * 1000);
                skill.cooldown_until = (skill.cooldown_until > dec) ? skill.cooldown_until - dec : 0;
            }
        }
    }
}

void SkillSystem::UpdateBuffs(entt::registry& reg, float dt) {
}
