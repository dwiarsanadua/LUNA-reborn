#pragma once
#include <string>
#include <vector>
#include <cstdint>

struct SkillTreeEntry {
    int skill_id;
    std::string name;
    int required_level;
    int required_skill_id; // 0 = none
    int max_level;
    std::string description;
    bool unlocked = false;
    int current_level = 0;
};

class ClassAdvancement {
public:
    static std::vector<SkillTreeEntry> GetSkillsForClass(int class_id) {
        std::vector<SkillTreeEntry> skills;
        // Basic skills available to all classes
        skills.push_back({1, "Power Strike", 1, 0, 10, "Powerful melee attack"});
        skills.push_back({2, "Quick Step", 5, 0, 5, "Move faster"});
        skills.push_back({3, "Iron Skin", 10, 0, 5, "Increase defense"});
        
        // Class-specific skills
        if (class_id == 0) { // Warrior
            skills.push_back({10, "Berserker Rage", 15, 1, 5, "Increase attack power"});
            skills.push_back({11, "Shield Wall", 20, 3, 3, "Block incoming damage"});
            skills.push_back({12, "Whirlwind", 25, 10, 5, "Attack all nearby enemies"});
        } else if (class_id == 1) { // Mage
            skills.push_back({20, "Fireball", 15, 1, 5, "Ranged fire attack"});
            skills.push_back({21, "Ice Shield", 20, 3, 3, "Protective ice barrier"});
            skills.push_back({22, "Teleport", 25, 20, 3, "Teleport to target location"});
        } else if (class_id == 2) { // Archer
            skills.push_back({30, "Arrow Rain", 15, 1, 5, "Attack multiple enemies"});
            skills.push_back({31, "Eagle Eye", 20, 2, 3, "Increase accuracy"});
            skills.push_back({32, "Poison Arrow", 25, 30, 5, "Poison target over time"});
        }
        
        return skills;
    }
    
    static bool CanLearn(const SkillTreeEntry& skill, int player_level, const std::vector<int>& learned_skills) {
        if (player_level < skill.required_level) return false;
        if (skill.required_skill_id > 0) {
            bool has_prereq = false;
            for (int ls : learned_skills) {
                if (ls == skill.required_skill_id) { has_prereq = true; break; }
            }
            if (!has_prereq) return false;
        }
        return skill.current_level < skill.max_level;
    }
};
