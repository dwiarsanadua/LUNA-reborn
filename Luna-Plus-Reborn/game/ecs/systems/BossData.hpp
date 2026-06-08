#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>

struct BossDefinition {
    uint32_t monster_id;
    std::string name;
    int map_id;
    float spawn_x, spawn_y, spawn_z;
    float respawn_time;
    std::vector<float> phase_hp;
    std::vector<std::string> special_abilities;
    bool has_enrage;
    float enrage_time;
    float min_party_size;
    std::string loot_table_id;
};

static inline std::unordered_map<uint32_t, BossDefinition> CreateDefaultBossDefinitions() {
    std::unordered_map<uint32_t, BossDefinition> defs;

    // ========== 5 DUNGEON BOSSES ==========

    defs[9001] = {
        9001, "Dragon Lord", 1, 100.0f, 50.0f, 0.0f, 600.0f,
        {75.0f, 50.0f, 25.0f, 10.0f},
        {"fire_breath", "tail_sweep", "wing_gust", "summon_adds"},
        true, 180.0f, 6.0f, "dragon_lord_loot"
    };
    defs[9002] = {
        9002, "Demon King", 2, 200.0f, 80.0f, 0.0f, 720.0f,
        {75.0f, 50.0f, 25.0f},
        {"dark_blast", "shadow_pulse", "summon_demons", "life_drain"},
        true, 240.0f, 8.0f, "demon_king_loot"
    };
    defs[9003] = {
        9003, "Ancient Guardian", 3, 150.0f, 60.0f, 0.0f, 540.0f,
        {80.0f, 55.0f, 30.0f, 10.0f},
        {"stone_spike", "earthquake", "petrify_gaze", "guardian_shield"},
        true, 300.0f, 5.0f, "ancient_guardian_loot"
    };
    defs[9004] = {
        9004, "Abyssal Giant", 4, 180.0f, 70.0f, 0.0f, 480.0f,
        {70.0f, 40.0f, 20.0f},
        {"crushing_blow", "stomp", "frenzy", "boulder_toss"},
        true, 150.0f, 4.0f, "abyssal_giant_loot"
    };
    defs[9005] = {
        9005, "Dark Emperor", 5, 250.0f, 100.0f, 0.0f, 900.0f,
        {85.0f, 65.0f, 45.0f, 25.0f, 10.0f},
        {"void_slash", "darkness_aura", "summon_shadows", "annihilate"},
        true, 360.0f, 10.0f, "dark_emperor_loot"
    };

    // ========== 34 FIELD BOSSES ==========

    // (408-412) Snowman Boss variants
    defs[408] = {
        408, "Snowman Boss", 32, 0.0f, 0.0f, 0.0f, 120.0f,
        {75.0f, 50.0f, 25.0f},
        {"ice_shard", "snow_storm"},
        true, 180.0f, 3.0f, "snowman_boss_loot"
    };
    defs[409] = {
        409, "Snowman Boss", 32, 50.0f, 0.0f, 0.0f, 180.0f,
        {75.0f, 50.0f, 25.0f},
        {"ice_shard", "snow_storm", "freeze_aura"},
        true, 180.0f, 3.0f, "snowman_boss_loot"
    };
    defs[410] = {
        410, "Snowman Boss", 33, 0.0f, 0.0f, 0.0f, 240.0f,
        {75.0f, 50.0f, 25.0f},
        {"ice_shard", "snow_storm", "freeze_aura", "blizzard"},
        true, 180.0f, 4.0f, "snowman_boss_loot"
    };
    defs[411] = {
        411, "Snowman Boss", 33, 60.0f, 0.0f, 0.0f, 300.0f,
        {70.0f, 40.0f, 20.0f},
        {"ice_shard", "snow_storm", "freeze_aura", "blizzard"},
        true, 180.0f, 5.0f, "snowman_boss_loot"
    };
    defs[412] = {
        412, "Snowman Boss", 34, 0.0f, 0.0f, 0.0f, 360.0f,
        {70.0f, 40.0f, 20.0f},
        {"ice_shard", "snow_storm", "blizzard", "absolute_zero"},
        true, 180.0f, 6.0f, "snowman_boss_loot"
    };

    // Goblin Kings
    defs[34] = {
        34, "Goblin King", 32, 20.0f, 0.0f, 0.0f, 120.0f,
        {75.0f, 50.0f},
        {"goblin_rush", "thief_strike"},
        false, 0.0f, 2.0f, "goblin_king_loot"
    };
    defs[481] = {
        481, "Goblin King", 32, 100.0f, 0.0f, 0.0f, 180.0f,
        {75.0f, 50.0f},
        {"goblin_rush", "thief_strike", "summon_goblins"},
        false, 0.0f, 3.0f, "goblin_king_loot"
    };
    defs[484] = {
        484, "Goblin King", 33, 30.0f, 0.0f, 0.0f, 240.0f,
        {70.0f, 40.0f, 20.0f},
        {"goblin_rush", "thief_strike", "summon_goblins", "king_slash"},
        true, 240.0f, 4.0f, "goblin_king_loot"
    };

    // Skeleton King
    defs[79] = {
        79, "Skeleton King", 32, 150.0f, 0.0f, 0.0f, 180.0f,
        {75.0f, 50.0f, 25.0f},
        {"bone_throw", "skeletal_wall", "summon_skeletons"},
        true, 180.0f, 4.0f, "skeleton_king_loot"
    };

    // King Grizzly Bear
    defs[711] = {
        711, "King Grizzly Bear", 33, 80.0f, 0.0f, 0.0f, 300.0f,
        {75.0f, 50.0f, 25.0f},
        {"maul", "roar", "frenzy"},
        true, 120.0f, 3.0f, "grizzly_loot"
    };

    // Desert Thief King
    defs[714] = {
        714, "Desert Thief King", 34, 90.0f, 0.0f, 0.0f, 240.0f,
        {70.0f, 40.0f},
        {"sand_blast", "thief_ambush", "summon_thieves"},
        false, 0.0f, 3.0f, "thief_king_loot"
    };

    // Native Witch Doctor King
    defs[716] = {
        716, "Native Witch Doctor King", 35, 200.0f, 0.0f, 0.0f, 600.0f,
        {80.0f, 60.0f, 40.0f, 20.0f},
        {"hex", "curse", "totem_summon", "spirit_wave"},
        true, 300.0f, 6.0f, "witch_doctor_loot"
    };

    // Giants
    defs[39] = {
        39, "Giant", 32, 30.0f, 0.0f, 0.0f, 120.0f,
        {75.0f, 50.0f},
        {"smash", "stomp"},
        false, 0.0f, 2.0f, "giant_loot"
    };
    defs[75] = {
        75, "Giant Mandragora", 32, 50.0f, 0.0f, 0.0f, 120.0f,
        {75.0f, 50.0f},
        {"poison_spore", "vine_whip"},
        false, 0.0f, 2.0f, "giant_mandragora_loot"
    };
    defs[96] = {
        96, "Abandoned Giant", 32, 80.0f, 0.0f, 0.0f, 180.0f,
        {75.0f, 50.0f, 25.0f},
        {"smash", "stomp", "ground_slam"},
        true, 180.0f, 3.0f, "giant_loot"
    };
    defs[97] = {
        97, "Giant Wanderer", 32, 100.0f, 0.0f, 0.0f, 180.0f,
        {75.0f, 50.0f, 25.0f},
        {"smash", "charge", "ground_slam"},
        true, 180.0f, 3.0f, "giant_wanderer_loot"
    };
    defs[98] = {
        98, "Magic Powered Giant", 33, 120.0f, 0.0f, 0.0f, 240.0f,
        {70.0f, 40.0f, 20.0f},
        {"magic_blast", "arcane_barrier", "elemental_smash"},
        true, 240.0f, 4.0f, "magic_giant_loot"
    };
    defs[214] = {
        214, "Aged Giant", 33, 60.0f, 0.0f, 0.0f, 120.0f,
        {75.0f, 50.0f},
        {"smash", "stomp"},
        false, 0.0f, 2.0f, "giant_loot"
    };
    defs[215] = {
        215, "Mean Giant", 33, 70.0f, 0.0f, 0.0f, 150.0f,
        {75.0f, 50.0f},
        {"smash", "enrage"},
        true, 120.0f, 2.0f, "mean_giant_loot"
    };
    defs[227] = {
        227, "Hurt Giant", 33, 90.0f, 0.0f, 0.0f, 180.0f,
        {70.0f, 40.0f},
        {"smash", "stomp", "frenzy"},
        true, 150.0f, 3.0f, "hurt_giant_loot"
    };
    defs[251] = {
        251, "Giant Tarantula", 34, 80.0f, 0.0f, 0.0f, 180.0f,
        {75.0f, 50.0f, 25.0f},
        {"poison_bite", "web_trap", "summon_spiders"},
        true, 180.0f, 3.0f, "giant_tarantula_loot"
    };
    defs[286] = {
        286, "Wild Giant", 34, 70.0f, 0.0f, 0.0f, 150.0f,
        {75.0f, 50.0f},
        {"smash", "charge"},
        false, 0.0f, 2.0f, "wild_giant_loot"
    };
    defs[332] = {
        332, "Giant Maggot", 34, 100.0f, 0.0f, 0.0f, 240.0f,
        {70.0f, 40.0f, 20.0f},
        {"acid_spit", "burrow", "swarm"},
        true, 180.0f, 4.0f, "giant_maggot_loot"
    };

    // Gargoyle Guardians
    defs[810] = {
        810, "Gargoyle Guardian", 35, 150.0f, 0.0f, 0.0f, 300.0f,
        {75.0f, 50.0f, 25.0f},
        {"stone_skin", "wing_sweep", "petrify"},
        true, 240.0f, 4.0f, "gargoyle_guardian_loot"
    };
    defs[845] = {
        845, "Gargoyle Guardian", 36, 200.0f, 0.0f, 0.0f, 360.0f,
        {75.0f, 50.0f, 25.0f},
        {"stone_skin", "wing_sweep", "petrify", "stone_rain"},
        true, 240.0f, 5.0f, "gargoyle_guardian_loot"
    };
    defs[856] = {
        856, "Gargoyle Guardian", 36, 180.0f, 0.0f, 0.0f, 300.0f,
        {70.0f, 40.0f, 20.0f},
        {"stone_skin", "wing_sweep", "petrify"},
        true, 240.0f, 4.0f, "gargoyle_guardian_loot"
    };

    // Dark variants
    defs[71] = {
        71, "Darkness Gnoll", 32, 60.0f, 0.0f, 0.0f, 120.0f,
        {75.0f, 50.0f},
        {"dark_bite", "shadow_step"},
        false, 0.0f, 2.0f, "dark_gnoll_loot"
    };
    defs[870] = {
        870, "Dark Satyr", 35, 130.0f, 0.0f, 0.0f, 240.0f,
        {75.0f, 50.0f, 25.0f},
        {"dark_bolt", "nature_fury", "summon_satyrs"},
        true, 180.0f, 4.0f, "dark_satyr_loot"
    };

    // Silver Gargoyle Leader
    defs[1248] = {
        1248, "Silver Gargoyle Leader", 37, 300.0f, 0.0f, 0.0f, 480.0f,
        {80.0f, 60.0f, 40.0f, 20.0f},
        {"silver_beam", "gargoyle_squad", "metal_shield", "silver_rain"},
        true, 300.0f, 6.0f, "silver_gargoyle_loot"
    };

    // High-level guardian spirits
    defs[1246] = {
        1246, "Buns Woody Guardian God", 38, 500.0f, 0.0f, 0.0f, 900.0f,
        {85.0f, 65.0f, 45.0f, 25.0f, 10.0f},
        {"divine_strike", "forest_wrath", "summon_guardians", "rejuvenation"},
        true, 360.0f, 10.0f, "woody_guardian_loot"
    };
    defs[1247] = {
        1247, "Native Guardian Crest", 38, 350.0f, 0.0f, 0.0f, 600.0f,
        {80.0f, 60.0f, 40.0f, 20.0f},
        {"crest_bash", "guardian_aura", "totem_summon"},
        true, 300.0f, 6.0f, "guardian_crest_loot"
    };

    // ========== 5 SPECIAL BOSS TYPES (Agent 2) ==========

    defs[9701] = {
        9701, "Arach the Poison Weaver", 35, 400.0f, 50.0f, 0.0f, 480.0f,
        {75.0f, 50.0f, 25.0f, 10.0f},
        {"poison_spit", "web_trap", "summon_spiders", "venom_aura"},
        true, 240.0f, 6.0f, "arach_loot"
    };
    defs[9702] = {
        9702, "Dragonian the Inferno", 36, 450.0f, 60.0f, 0.0f, 540.0f,
        {75.0f, 50.0f, 25.0f, 10.0f},
        {"fire_breath", "tail_sweep", "wing_gust", "inferno"},
        true, 270.0f, 6.0f, "dragonian_loot"
    };
    defs[9703] = {
        9703, "Leostein the Storm Lord", 36, 500.0f, 70.0f, 0.0f, 600.0f,
        {75.0f, 50.0f, 25.0f, 10.0f},
        {"lightning_strike", "roar", "storm_aura", "thunder_fall"},
        true, 300.0f, 8.0f, "leostein_loot"
    };
    defs[9704] = {
        9704, "Tarintus the Frost Giant", 37, 550.0f, 80.0f, 0.0f, 660.0f,
        {75.0f, 50.0f, 25.0f, 10.0f},
        {"ice_shard", "blizzard", "freeze_aura", "absolute_zero"},
        true, 330.0f, 8.0f, "tarintus_loot"
    };
    defs[9705] = {
        9705, "Kierra the Dark Empress", 38, 600.0f, 90.0f, 0.0f, 720.0f,
        {75.0f, 50.0f, 25.0f, 10.0f},
        {"dark_bolt", "life_drain", "shadow_pulse", "annihilate"},
        true, 360.0f, 10.0f, "kierra_loot"
    };

    // Named zone bosses
    defs[1236] = {
        1236, "Arasedual", 37, 250.0f, 0.0f, 0.0f, 480.0f,
        {75.0f, 50.0f, 25.0f},
        {"arcane_blast", "teleport_strike", "summon_arcana"},
        true, 240.0f, 5.0f, "arasedual_loot"
    };
    defs[1241] = {
        1241, "Ugiarik", 37, 280.0f, 0.0f, 0.0f, 540.0f,
        {80.0f, 55.0f, 30.0f},
        {"frozen_touch", "ice_storm", "summon_icicles"},
        true, 270.0f, 5.0f, "ugiarik_loot"
    };
    defs[1240] = {
        1240, "Siwana", 37, 320.0f, 0.0f, 0.0f, 600.0f,
        {80.0f, 60.0f, 40.0f, 20.0f},
        {"flame_wall", "fire_rain", "summon_fire_elementals"},
        true, 300.0f, 6.0f, "siwana_loot"
    };
    defs[1234] = {
        1234, "Dokkeumas", 36, 200.0f, 0.0f, 0.0f, 360.0f,
        {75.0f, 50.0f, 25.0f},
        {"toxic_cloud", "venom_strike", "summon_poison"},
        true, 180.0f, 4.0f, "dokkeumas_loot"
    };
    defs[1235] = {
        1235, "Sanjoburas", 36, 220.0f, 0.0f, 0.0f, 420.0f,
        {75.0f, 50.0f, 25.0f},
        {"sand_storm", "dune_slide", "summon_sand_elementals"},
        true, 200.0f, 4.0f, "sanjoburas_loot"
    };
    defs[1233] = {
        1233, "Gangdeusoa", 36, 180.0f, 0.0f, 0.0f, 300.0f,
        {70.0f, 40.0f},
        {"thunder_clap", "lightning_bolt"},
        true, 180.0f, 3.0f, "gangdeusoa_loot"
    };

    return defs;
}
