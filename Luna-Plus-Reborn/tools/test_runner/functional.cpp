#include "test_harness.hpp"
#include <glm/glm.hpp>
#include <cmath>
#include <string>
#include <vector>
#include <algorithm>

static float CalcPhysicAttack(int level, int strength, int base_strength, int weapon_attack, int enchant_level) {
    float weapon = static_cast<float>(weapon_attack + level);
    float str_ratio = 1.0f + static_cast<float>(strength) * 0.001f;
    float base = weapon * str_ratio;
    float real_str_bonus = static_cast<float>(strength - base_strength);
    float atk = base + real_str_bonus;
    if (enchant_level > 0) {
        float enchant_bonus = 1.0f + (enchant_level * enchant_level) / 400.0f;
        atk *= enchant_bonus;
    }
    return atk;
}

static float CalcPhysicDefense(int level, int constitution, int armor_defense) {
    float def = static_cast<float>(armor_defense + level);
    def *= (1.0f + constitution / 3000.0f);
    def += (constitution - 40) / 5.0f;
    return def;
}

static float CalcCritRate(int level, int dexterity, int base_dexterity) {
    if (dexterity <= base_dexterity) return 0.0f;
    float dex_rate = static_cast<float>(dexterity - base_dexterity) / ((level - 1) * 5.0f);
    if (dex_rate > 1.0f) dex_rate = 1.0f;
    return 45.0f * dex_rate + (base_dexterity - 25.0f) / 5.0f;
}

static float CalcBlockRate(int dexterity, int class_id) {
    float base = dexterity / 27.0f;
    float bonuses[] = {0, 15, 10, 5, 9};
    int idx = (class_id >= 1 && class_id <= 4) ? class_id : 0;
    return base + bonuses[idx];
}

static float CalcHealAmount(int level, int wisdom, int intelligence, float modifier) {
    return ((wisdom * 11 + intelligence * 4 + level * 20) * modifier) / 800.0f + 100.0f;
}

static int CalcRequiredExp(int level) {
    if (level <= 1) return 150;
    return static_cast<int>(150.0f * std::pow(level, 1.5f) * 10.0f);
}

void RunFunctionalTests() {
    TEST_SUITE("FUNCTIONAL: FORMULA VERIFICATION");

    TEST_STEP("CalcPhysicAttack — baseline");
    {
        float atk = CalcPhysicAttack(50, 100, 10, 30, 0);
        spdlog::info("  PhysicAttack = {:.2f}", atk);
        TEST("Base physic attack ~178", std::abs(atk - 178.0f) < 1.0f);
    }

    TEST_STEP("CalcPhysicAttack — with enchant");
    {
        float base = CalcPhysicAttack(50, 100, 10, 30, 0);
        float enchanted = CalcPhysicAttack(50, 100, 10, 30, 5);
        TEST("Enchant increases damage", enchanted > base);
        spdlog::info("  Base={:.2f}, Ench+5={:.2f}", base, enchanted);
    }

    TEST_STEP("CalcPhysicAttack — high STR scaling");
    {
        float atk = CalcPhysicAttack(50, 500, 10, 200, 0);
        spdlog::info("  High STR attack = {:.2f}", atk);
        TEST("High STR attack > 500", atk > 500);
    }

    TEST_STEP("CalcPhysicDefense — baseline");
    {
        float def = CalcPhysicDefense(50, 60, 100);
        spdlog::info("  PhysicDefense = {:.2f}", def);
        TEST("Base physic defense ~157", std::abs(def - 157.0f) < 1.0f);
    }

    TEST_STEP("CalcPhysicDefense — high VIT");
    {
        float def_high = CalcPhysicDefense(50, 200, 100);
        float def_low = CalcPhysicDefense(50, 20, 100);
        spdlog::info("  VIT=200 def={:.2f}, VIT=20 def={:.2f}", def_high, def_low);
        TEST("Higher VIT = higher defense", def_high > def_low);
    }

    TEST_STEP("CalcCritRate — normal");
    {
        float crit = CalcCritRate(50, 80, 20);
        spdlog::info("  CritRate = {:.2f}%", crit);
        TEST("Base crit rate ~10%", std::abs(crit - 10.0f) < 1.0f);
    }

    TEST_STEP("CalcCritRate — low dex");
    {
        float crit_low = CalcCritRate(10, 10, 20);
        TEST("Below base dex = 0% crit", crit_low == 0.0f);
    }

    TEST_STEP("CalcCritRate — max dex");
    {
        float crit = CalcCritRate(50, 999, 20);
        spdlog::info("  Max dex crit = {:.2f}%", crit);
        TEST("Max dex capped crit", crit <= 50.0f);
    }

    TEST_STEP("CalcBlockRate — class comparison");
    {
        float fighter = CalcBlockRate(50, 1);
        float rogue = CalcBlockRate(60, 2);
        float ranger = CalcBlockRate(70, 3);
        float mage = CalcBlockRate(40, 4);
        spdlog::info("  Fighter={:.2f}, Rogue={:.2f}, Ranger={:.2f}, Mage={:.2f}",
                     fighter, rogue, ranger, mage);
        TEST("Fighter highest block", fighter > rogue && fighter > ranger && fighter > mage);
    }

    TEST_STEP("CalcHealAmount — baseline");
    {
        float heal = CalcHealAmount(50, 80, 60, 1.0f);
        spdlog::info("  Heal amount = {:.2f}", heal);
        TEST("Heal formula ~102.65", std::abs(heal - 102.65f) < 1.0f);
    }

    TEST_STEP("CalcRequiredExp — scaling");
    {
        int e1 = CalcRequiredExp(1);
        int e50 = CalcRequiredExp(50);
        int e99 = CalcRequiredExp(99);
        spdlog::info("  Exp lv1={}, lv50={}, lv99={}", e1, e50, e99);
        TEST("Exp scales with level", e99 > e50 && e50 > e1);
    }

    TEST_STEP("CalcRequiredExp — known values");
    {
        TEST("Level 1 exp = 150", CalcRequiredExp(1) == 150);
        spdlog::info("  Known values verified: lv1={}", CalcRequiredExp(1));
    }
}
