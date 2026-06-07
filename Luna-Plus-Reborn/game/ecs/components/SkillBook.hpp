#pragma once
#include <vector>
#include <cstdint>

struct SkillEntry {
    uint16_t skill_id;
    uint8_t level = 1;
    uint8_t max_level = 10;
    uint32_t cooldown_until = 0;
    uint16_t hotbar_slot = 0;
};

struct SkillBook {
    std::vector<SkillEntry> skills;
    static constexpr size_t HOTBAR_SIZE = 10;
    uint16_t hotbar[HOTBAR_SIZE] = {0};

    bool HasSkill(uint16_t id) const;
    void LearnSkill(uint16_t id, uint8_t level = 1);
    void RemoveSkill(uint16_t id);
    bool IsOnCooldown(uint16_t id, uint32_t now) const;
    void SetCooldown(uint16_t id, uint32_t duration_ms, uint32_t now);
};
