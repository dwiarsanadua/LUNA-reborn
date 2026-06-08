#pragma once
#include <unordered_map>
#include <cstdint>

struct SkillEntry {
    uint16_t skill_id;
    uint8_t level = 1;
    uint8_t max_level = 10;
    uint32_t cooldown_until = 0;
    uint16_t hotbar_slot = 0;

    // Combat effects
    float add_damage = 0;
    uint8_t add_type = 1;
    float rate_add_value = 0;
    float plus_add_value = 0;
    float heal_amount = 0;
    float buff_duration = 0;

    // Status effects
    float stun_duration = 0;
    float poison_damage = 0;
    float poison_duration = 0;
    float slow_amount = 0;
    float slow_duration = 0;
};

struct SkillBook {
    std::unordered_map<uint16_t, SkillEntry> skills;
    static constexpr size_t HOTBAR_SIZE = 10;
    uint16_t hotbar[HOTBAR_SIZE] = {0};

    bool HasSkill(uint16_t id) const { return skills.find(id) != skills.end(); }
    void LearnSkill(uint16_t id, uint8_t level = 1) { skills[id] = {id, level, 10, 0, 0}; }
    void RemoveSkill(uint16_t id) { skills.erase(id); }
    bool IsOnCooldown(uint16_t id, uint32_t now) const {
        auto it = skills.find(id);
        return it != skills.end() && now < it->second.cooldown_until;
    }
    void SetCooldown(uint16_t id, uint32_t duration_ms, uint32_t now) {
        if (auto it = skills.find(id); it != skills.end())
            it->second.cooldown_until = now + duration_ms;
    }
};
