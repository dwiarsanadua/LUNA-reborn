#include "test_harness.hpp"
#include <ecs/systems/CombatSystem.hpp>
#include <ecs/components/CharacterStats.hpp>
#include <random>
#include <cmath>

static std::mt19937 rng(42); // fixed seed for reproducibility

void RunFormulaTests() {
    TEST_SUITE("COMBAT FORMULAS");

    // ─── Test CalcPhysicAttack ───
    TEST_STEP("CalcPhysicAttack with base stats");
    {
        CharacterStats s;
        s.level = 50;
        s.strength = 100; s.base_strength = 10;
        s.weapon_attack = 30;
        s.enchant_level = 0;
        float atk = CombatSystem::CalcPhysicAttack(s);
        spdlog::info("    PhysicAttack = {:.2f} (weapon={}, str={}, base_str={})",
                     atk, s.weapon_attack, s.strength, s.base_strength);
        // Old: (Weapon+Lvl)*(1+STR/1000)+(RealSTR-30) = (30+50)*(1+0.1)+(100-10) = 80*1.1+90 = 178
        TEST("CalcPhysicAttack basic", std::abs(atk - 178.0f) < 1.0f);
    }

    TEST_STEP("CalcPhysicAttack with enchant");
    {
        CharacterStats s;
        s.level = 50;
        s.strength = 100; s.base_strength = 10;
        s.weapon_attack = 30;
        s.enchant_level = 5;
        float atk = CombatSystem::CalcPhysicAttack(s);
        float enchant_bonus = 1.0f + (5.0f * 5.0f) / 400.0f; // 1.0625
        spdlog::info("    With enchant +5: atk={:.2f} (enchant_bonus={:.4f})", atk, enchant_bonus);
        TEST("CalcPhysicAttack with enchant > base", atk > 178.0f);
    }

    TEST_STEP("CalcPhysicAttack with buffs");
    {
        CharacterStats s;
        s.level = 50;
        s.strength = 100; s.base_strength = 10;
        s.weapon_attack = 30;
        s.physic_attack_pct = 20.0f; // +20% buff
        float atk = CombatSystem::CalcPhysicAttack(s);
        spdlog::info("    With +20% buff: atk={:.2f}", atk);
        float expected_base = (30 + 50) * (1.0f + 100 * 0.001f) + (100 - 10);
        float expected = expected_base * (1.0f + 20.0f / 100.0f);
        TEST("CalcPhysicAttack with buff", std::abs(atk - expected) < 0.1f);
    }

    // ─── Test CalcPhysicDefense ───
    TEST_STEP("CalcPhysicDefense base");
    {
        CharacterStats s;
        s.level = 50;
        s.constitution = 60;
        s.armor_defense = 100;
        s.enchant_level = 0;
        float def = CombatSystem::CalcPhysicDefense(s);
        // (100+50)*(1+60/3000)+(60-40)/5 = 150*1.02+4 = 157
        spdlog::info("    PhysicDefense = {:.2f} (armor={}, vit={})", def, s.armor_defense, s.constitution);
        TEST("CalcPhysicDefense base", std::abs(def - 157.0f) < 1.0f);
    }

    // ─── Test CalcCritRate ───
    TEST_STEP("CalcCritRate");
    {
        CharacterStats s;
        s.level = 50;
        s.dexterity = 80; s.base_dexterity = 20;
        // DexRate = (80-20)/((50-1)*5) = 60/245 = 0.2449
        // Crit = 45 * 0.2449 + (20-25)/5 = 11.02 - 1 = 10.02
        float crit = CombatSystem::CalcCritRate(s);
        spdlog::info("    CritRate = {:.2f}% (dex={}, base_dex={}, level={})",
                     crit, s.dexterity, s.base_dexterity, s.level);
        TEST("CalcCritRate basic", std::abs(crit - 10.02f) < 0.5f);
    }

    TEST_STEP("CalcCritRate — low dex = no crit");
    {
        CharacterStats s;
        s.level = 10;
        s.dexterity = 10; s.base_dexterity = 20; // dex BELOW base
        float crit = CombatSystem::CalcCritRate(s);
        spdlog::info("    CritRate with low dex = {:.2f}%", crit);
        TEST("CalcCritRate low dex", crit < 1.0f); // should be near 0
    }

    // ─── Test CalcBlockRate ───
    TEST_STEP("CalcBlockRate per class");
    {
        CharacterStats fighter, rogue, ranger, mage;
        fighter.dexterity = 50; fighter.class_id = 1;
        rogue.dexterity = 60;   rogue.class_id = 2;
        ranger.dexterity = 70;  ranger.class_id = 3;
        mage.dexterity = 40;    mage.class_id = 4;

        float f_block = CombatSystem::CalcBlockRate(fighter);
        float r_block = CombatSystem::CalcBlockRate(rogue);
        float ra_block = CombatSystem::CalcBlockRate(ranger);
        float m_block = CombatSystem::CalcBlockRate(mage);

        spdlog::info("    Fighter block = {:.2f} (should be DEX/27 + 15)", f_block);
        spdlog::info("    Rogue block   = {:.2f} (should be DEX/27 + 10)", r_block);
        spdlog::info("    Ranger block  = {:.2f} (should be DEX/27 + 5)",  ra_block);
        spdlog::info("    Mage block    = {:.2f} (should be DEX/27 + 9)",  m_block);

        TEST("Fighter highest block", f_block > r_block && f_block > ra_block && f_block > m_block);
        TEST("Mage better than ranger", m_block > ra_block);
        TEST("Formula DEX/27 + ClassBonus",
             std::abs(f_block - (50.0f/27.0f + 15.0f)) < 0.1f);
    }

    // ─── Test Accuracy / Evasion ───
    TEST_STEP("Accuracy and Evasion");
    {
        CharacterStats atk, def;
        atk.level = 50; atk.dexterity = 80; atk.base_dexterity = 20; atk.class_id = 1;
        def.level = 50; def.dexterity = 80; def.base_dexterity = 20; def.class_id = 1;

        float acc = CombatSystem::CalcAccuracy(atk);
        float eva = CombatSystem::CalcEvasion(def);
        float hit_chance = 85.0f + acc - eva;
        hit_chance = std::clamp(hit_chance, 10.0f, 99.0f);

        spdlog::info("    Accuracy = {:.2f}, Evasion = {:.2f}, HitChance = {:.2f}%", acc, eva, hit_chance);
        TEST("Same stats = near 85% hit", std::abs(hit_chance - 85.0f) < 10.0f);
    }

    TEST_STEP("IsHit rolls within expected range");
    {
        int hits = 0, total = 1000;
        CharacterStats atk, def;
        atk.level = 50; atk.dexterity = 80; atk.base_dexterity = 20; atk.class_id = 1;
        def.level = 50; def.dexterity = 80; def.base_dexterity = 20; def.class_id = 1;
        for (int i = 0; i < total; i++) {
            if (CombatSystem::IsHit(atk, def)) hits++;
        }
        float pct = 100.0f * hits / total;
        spdlog::info("    IsHit 1000 rolls: {} hits ({:.1f}%)", hits, pct);
        TEST("IsHit ~85% hit rate", pct > 70.0f && pct < 99.0f);
    }

    // ─── Test CalculateDamage (lengkap) ───
    TEST_STEP("CalculateDamage — full combat round");
    {
        CharacterStats atk, def;
        atk.level = 50; atk.strength = 100; atk.base_strength = 10;
        atk.dexterity = 80; atk.base_dexterity = 20;
        atk.weapon_attack = 30; atk.class_id = 1; atk.constitution = 40;
        atk.enchant_level = 0;
        atk.critical_damage_rate = 0;

        def.level = 48; def.strength = 80; def.base_strength = 10;
        def.dexterity = 60; def.base_dexterity = 20;
        def.armor_defense = 100; def.class_id = 2; def.constitution = 60;
        def.enchant_level = 0; def.shield_defense = 20;

        // Run 20 attacks, collect stats
        int total_dmg = 0, crits = 0, blocks = 0, misses = 0, rounds = 20;
        for (int i = 0; i < rounds; i++) {
            auto r = CombatSystem::CalculateDamage(atk, def, 0, 1, 0, 0, 1.0f, CombatContext::Normal);
            total_dmg += r.damage;
            if (r.is_critical) crits++;
            if (r.is_blocked) blocks++;
            if (r.is_miss) misses++;
            spdlog::info("    Round {}: dmg={}, crit={}, block={}, miss={}",
                         i+1, r.damage, r.is_critical, r.is_blocked, r.is_miss);
        }
        float avg = (float)total_dmg / rounds;
        spdlog::info("    Average damage: {:.1f} (crits={}, blocks={}, misses={})", avg, crits, blocks, misses);
        TEST("Average damage > 0", avg > 0);
        TEST("At least one non-miss hit", misses < rounds);
    }

    // ─── Test PvP reduction ───
    TEST_STEP("PvP damage reduction (35%)");
    {
        CharacterStats atk, def;
        atk.level = 50; atk.strength = 100; atk.base_strength = 10;
        atk.weapon_attack = 30;
        def.level = 48; def.armor_defense = 100; def.constitution = 60;

        int total_normal = 0, total_pvp = 0, n = 5;
        for (int i = 0; i < n; i++) {
            total_normal += CombatSystem::CalculateDamage(atk, def, 0, 1, 0, 0, 1.0f, CombatContext::Normal).damage;
            total_pvp += CombatSystem::CalculateDamage(atk, def, 0, 1, 0, 0, 1.0f, CombatContext::PvP).damage;
        }
        spdlog::info("    Normal dmg={}, PvP dmg={} (should be ~35%)", total_normal, total_pvp);
        if (total_normal > 0) {
            float ratio = (float)total_pvp / (float)total_normal;
            spdlog::info("    PvP/Normal ratio = {:.3f} (expected ~0.35)", ratio);
            TEST("PvP damage ~35% of normal", ratio > 0.20f && ratio < 0.50f);
        }
    }

    // ─── Test GT reduction ───
    TEST_STEP("Guild Tournament damage reduction (10%)");
    {
        CharacterStats atk, def;
        atk.level = 50; atk.strength = 300; atk.base_strength = 10;
        atk.weapon_attack = 100; atk.dexterity = 100; atk.base_dexterity = 20;
        atk.class_id = 1; atk.constitution = 40;
        def.level = 48; def.armor_defense = 50; def.dexterity = 60;
        def.base_dexterity = 20; def.class_id = 2; def.constitution = 40;
        def.shield_defense = 5;

        int total_gt_norm = 0, total_gt = 0, gt_n = 5;
        for (int i = 0; i < gt_n; i++) {
            total_gt_norm += CombatSystem::CalculateDamage(atk, def, 0, 1, 0, 0, 1.0f, CombatContext::Normal).damage;
            total_gt += CombatSystem::CalculateDamage(atk, def, 0, 1, 0, 0, 1.0f, CombatContext::GuildTournament).damage;
        }
        spdlog::info("    Normal dmg={}, GT dmg={} (should be ~10%)", total_gt_norm, total_gt);
        if (total_gt_norm > 0) {
            float ratio = (float)total_gt / (float)total_gt_norm;
            spdlog::info("    GT/Normal ratio = {:.3f} (expected ~0.10)", ratio);
            TEST("GT damage ~10% of normal", ratio < 0.20f);
        }
    }

    // ─── Test Level Penalty ───
    TEST_STEP("Level penalty (attacker < defender)");
    {
        // Gunakan fixed seed agar hasil konsisten
        CharacterStats atk, def;
        atk.level = 50; atk.strength = 100; atk.base_strength = 10;
        atk.dexterity = 100; atk.base_dexterity = 20;
        atk.weapon_attack = 30; atk.class_id = 1; atk.constitution = 40;
        def.level = 50; def.armor_defense = 100; def.constitution = 60;
        def.dexterity = 60; def.base_dexterity = 20; def.class_id = 2; def.shield_defense = 5;

        // Test 10x each untuk rata-rata
        int dmg_lower = 0, dmg_equal = 0, dmg_higher = 0, n = 10;
        for (int i = 0; i < n; i++) {
            atk.level = 30;
            dmg_lower += CombatSystem::CalculateDamage(atk, def, 0, 1, 0, 0, 1.0f, CombatContext::Normal).damage;
            atk.level = 50;
            dmg_equal += CombatSystem::CalculateDamage(atk, def, 0, 1, 0, 0, 1.0f, CombatContext::Normal).damage;
            atk.level = 70;
            dmg_higher += CombatSystem::CalculateDamage(atk, def, 0, 1, 0, 0, 1.0f, CombatContext::Normal).damage;
        }
        spdlog::info("    Avg atk<def (30v50): {}", dmg_lower / n);
        spdlog::info("    Avg atk=def (50v50): {}", dmg_equal / n);
        spdlog::info("    Avg atk>def (70v50): {}", dmg_higher / n);
        // Old: penalty hanya ketika atk<def, tidak ada boost ketika atk>def
        TEST("Lower level does less damage (penalty)", dmg_lower <= dmg_equal);
        // atk>def mungkin lebih tinggi karena base damage (Weapon+Lvl) lebih besar, bukan karena boost
        // Yang penting: tidak ada PINALTI ketika atk>def
        TEST("Higher level not penalized", dmg_higher >= dmg_equal);
    }

    // ─── Test Skill Damage Modifier ───
    TEST_STEP("Skill damage modifier (add_type 1 = STR based)");
    {
        CharacterStats atk, def;
        atk.level = 50; atk.strength = 200; atk.base_strength = 10;
        atk.weapon_attack = 30;
        def.level = 50; def.armor_defense = 100; def.constitution = 60;

        auto no_skill = CombatSystem::CalculateDamage(atk, def, 0, 1, 0, 0, 1.0f, CombatContext::Normal);
        auto with_skill = CombatSystem::CalculateDamage(atk, def, 500, 1, 50, 30, 1.0f, CombatContext::Normal);
        int avg_no = 0, avg_skill = 0, n2 = 5;
        for (int i = 0; i < n2; i++) {
            avg_no += CombatSystem::CalculateDamage(atk, def, 0, 1, 0, 0, 1.0f, CombatContext::Normal).damage;
            avg_skill += CombatSystem::CalculateDamage(atk, def, 500, 1, 50, 30, 1.0f, CombatContext::Normal).damage;
        }
        spdlog::info("    Without skill: avg={}, With skill: avg={}", avg_no/n2, avg_skill/n2);
        TEST("Skill adds damage", avg_skill >= avg_no);
    }

    // ─── Test Heal Formula ───
    TEST_STEP("Heal formula (CalcHealAmount)");
    {
        CharacterStats healer;
        healer.level = 50;
        healer.wisdom = 80;
        healer.intelligence = 60;

        float heal = CombatSystem::CalcHealAmount(healer, 1.0f);
        // ((80*11 + 60*4 + 50*20) * 1.0) / 800 + 100 = (880+240+1000)/800+100 = 2120/800+100 = 102.65
        spdlog::info("    Heal amount = {:.2f} (wis={}, int={}, level={})",
                     heal, healer.wisdom, healer.intelligence, healer.level);
        TEST("Heal > 100 base", heal > 100.0f);
        TEST("Heal formula matches Old", std::abs(heal - 102.65f) < 1.0f);
    }

    // ─── Test Magic Attack/Defense ───
    TEST_STEP("Magic Attack and Defense formulas");
    {
        CharacterStats s;
        s.level = 50;
        s.intelligence = 100; s.base_intelligence = 10;
        s.wisdom = 60; s.base_wisdom = 10;
        s.weapon_attack = 30;
        s.armor_defense = 50;

        float matk = CombatSystem::CalcMagicAttack(s);
        float mdef = CombatSystem::CalcMagicDefense(s);
        spdlog::info("    MagicAttack = {:.2f}", matk);
        spdlog::info("    MagicDefense = {:.2f}", mdef);
        TEST("MagicAttack > 0", matk > 0);
        TEST("MagicDefense > 0", mdef > 0);
    }

    // ─── Test Aggro Threat ───
    TEST_STEP("ThreatTable operations");
    {
        ThreatTable table;
        table.AddThreat(1, 100);
        table.AddThreat(2, 200);
        table.AddThreat(3, 50);
        TEST("Top threat is entity 2", table.GetTopThreat() == 2);
        table.AddThreat(1, 150); // now entity 1 = 250
        TEST("Top threat becomes entity 1", table.GetTopThreat() == 1);
        table.Clear();
        TEST("Empty after clear", table.GetTopThreat() == 0);
    }

    // ─── Test ScaleMonsterStats ───
    TEST_STEP("Monster stat scaling");
    {
        MonsterBaseStats base;
        base.hp = 100; base.attack = 20; base.defense = 10;
        base.exp = 50; base.gold_min = 10; base.gold_max = 20;

        auto scaled = CombatSystem::ScaleMonsterStats(base, 10, 50);
        spdlog::info("    Base lv10: HP={}, ATK={}, DEF={}", base.hp, base.attack, base.defense);
        spdlog::info("    Scaled lv50: HP={}, ATK={}, DEF={}", scaled.hp, scaled.attack, scaled.defense);
        TEST("Scaled HP > base HP", scaled.hp > base.hp);
        TEST("Scaling factor ~5x", scaled.hp < base.hp * 10);
    }

    // ─── Test CalcRequiredExp ───
    TEST_STEP("Experience requirements");
    {
        // Jika method ada
        int exp_lv1 = CombatSystem::CalcRequiredExp(1);
        int exp_lv50 = CombatSystem::CalcRequiredExp(50);
        int exp_lv99 = CombatSystem::CalcRequiredExp(99);
        spdlog::info("    Exp for lv1={}, lv50={}, lv99={}", exp_lv1, exp_lv50, exp_lv99);
        TEST("Exp scales with level", exp_lv99 > exp_lv50 && exp_lv50 > exp_lv1);
    }

    spdlog::info("━━━ Formulas: {}/{} passed, {}/{} failed ━━━",
                 g_tests_passed - (g_tests_passed + g_tests_failed - g_tests_passed),
                 g_tests_failed > 0 ? "SOME" : "ALL",
                 g_tests_failed, g_tests_passed + g_tests_failed);
}
