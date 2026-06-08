#pragma once
#include <string>
#include <vector>
#include <array>
#include <random>
#include <cstdint>
#include <game/ecs/components/Equipment.hpp>
#include <gameobjects/ConsignmentSystem.hpp>

struct CharInfo {
    uint32_t id;
    std::string name;
    int level;
    int map_id;
    float pos_x, pos_y, pos_z;
};

struct RemoteEntity {
    uint32_t id;
    std::string name;
    float x, y, z;
    int level;
    int hp_pct;
};

struct InvItem {
    uint32_t id;
    std::string name;
    int count;
    int slot;
    int enchant = 0;
};

struct PartyMember {
    uint32_t id = 0;
    std::string name;
    int level = 1;
    int hp = 500;
    int max_hp = 500;
    uint16_t map_id = 51;
    bool is_leader = false;
};

struct FriendEntry {
    uint32_t id = 0;
    std::string name;
    int level = 1;
    bool online = false;
    uint16_t map_id = 51;
};

struct GuildMemberEntry {
    uint32_t id = 0;
    std::string name;
    int level = 1;
    uint8_t rank = 0;
    bool online = false;
};

struct StreetStallSlot {
    uint8_t slot = 0;
    uint32_t item_id = 0;
    uint16_t count = 0;
    uint32_t price = 0;
    std::string item_name;
};

struct StreetStallView {
    uint32_t owner_id = 0;
    std::string owner_name;
    std::string title;
    bool open = false;
    std::vector<StreetStallSlot> items;
};

struct TradeOfferItem {
    uint8_t trade_slot = 0;
    uint32_t item_id = 0;
    uint16_t count = 0;
    uint8_t inv_slot = 0;
    std::string name;
};

struct DamageFloat {
    float x, y, z;
    int dmg;
    float life;
};

enum class ClientState {
    None,
    Connect,
    Title,
    CharSelect,
    CharMake,
    Loading,
    GameIn,
    MapChange,
    Ending
};

struct GameState {
    ClientState current_state = ClientState::None;
    int selected_account = 0;
    int selected_char = 0;
    bool login_sent = false;
    bool login_ok = false;
    bool connecting = false;
    bool map_changing = false;
    uint16_t map_server_port = 0;
    bool offline_mode = false;
    bool enter_game_pending = false;
    float enter_game_load_timer = 0.0f;
    float battle_delay_timer = 0.0f;
    float player_x = 0, player_z = 0;
    bool has_waypoint = false;
    float waypoint_x = 0, waypoint_z = 0;
    float player_y = 0;
    int hp = 500, max_hp = 500;
    int mp = 100, max_mp = 100;
    int exp = 0, exp_next = 500;
    int level = 10;
    int gold = 0;
    int attack = 50, defense = 20;
    int class_id = 0; // 0=Warrior, 1=Mage, 2=Archer
    int race = 0, gender = 0;
    int hair_style = 0, face_style = 0;
    uint32_t hair_color = 0xff442200, skin_color = 0xffe8c090, eye_color = 0xff4488cc;
    int stat_str = 10, stat_dex = 10, stat_int = 10, stat_con = 10;
    std::vector<int> learned_skills;
    std::array<uint32_t, 10> hotbar_skills{};
    std::array<float, 10> hotbar_cooldowns{};
    uint32_t pending_skill_id = 0;
    int skill_points = 5;
    float cam_yaw = -45.0f, cam_pitch = -40.0f;
    float cam_dist = 80.0f;
    std::string name = "Hero";
    bool chat_open = false;
    bool inv_open = false;
    bool skill_open = false;
    bool charinfo_open = false;
    bool party_open = false;
    bool guild_open = false;
    bool friend_open = false;
    bool farm_open = false;
    bool options_open = false;
    bool storage_open = false;
    bool help_open = false;
    bool worldmap_open = false;
    bool fishing_open = false;
    bool trade_open = false;
    bool consignment_open = false;
    bool family_open = false;
    bool housing_open = false;
    bool dungeon_open = false;
    bool costume_open = false;
    bool cashshop_open = false;
    bool mail_open = false;
    bool keybind_open = false;
    bool chatroom_open = false;
    bool pet_open = false;
    bool cooking_open = false;
    bool mount_open = false;
    bool tournament_open = false;
    bool avatar_open = false;
    bool upgrade_open = false;
    bool macro_open = false;
    bool quest_open = false;
    bool npc_open = false;
    bool revival_open = false;
    bool mix_open = false;
    bool party_invite_open = false;
    bool guild_notice_open = false;
    bool stall_sell_open = false;
    bool siege_flag_open = false;
    std::string npc_text;
    int npc_id = 0;
    int npc_sub_mode = 0; // 0=main menu, 1=shop, 2=other
    bool pk_mode = false;
    int pk_kills = 0;
    bool in_dungeon = false;
    int dungeon_map = 0;
    std::string chat_input;
    std::vector<std::string> chat_messages;
    std::string session_token;
    std::vector<CharInfo> characters;
    std::vector<RemoteEntity> entities;
    uint32_t map_id = 0;
    std::string login_error;
    uint32_t next_entity_id = 100;
    std::mt19937 rng{std::random_device{}()};
    float combat_timer = 0;
    float combat_cast_time = 0;    // Casting phase remaining
    float combat_anim_lock = 0;    // Animation lock remaining
    uint32_t monster_kills = 0;
    int32_t target_entity = -1;
    std::vector<DamageFloat> damage_floats;
    float shake_x = 0, shake_y = 0;
    std::vector<InvItem> inventory;
    std::vector<InvItem> storage_items;
    uint32_t storage_gold = 0;
    uint32_t party_id = 0;
    std::vector<PartyMember> party_members;
    std::vector<FriendEntry> friends;
    uint32_t guild_id = 0;
    std::string guild_name;
    uint8_t guild_level = 1;
    uint32_t guild_gp = 0;
    std::vector<GuildMemberEntry> guild_members;
    uint32_t trade_session_id = 0;
    std::string trade_partner_name;
    std::vector<TradeOfferItem> trade_my_items;
    std::vector<TradeOfferItem> trade_their_items;
    uint32_t trade_my_gold = 0;
    uint32_t trade_their_gold = 0;
    bool trade_my_confirmed = false;
    bool trade_their_confirmed = false;
    bool trade_active = false;
    bool trade_completed = false;
    std::vector<AuctionListing> consignment_browse;
    std::vector<AuctionListing> consignment_mine;
    std::vector<AuctionListing> consignment_bids;
    StreetStallView my_stall;
    std::vector<StreetStallView> nearby_stalls;
    Equipment equipment;
    bool equipment_dirty = false;
    std::vector<std::string> skill_list;
    std::vector<std::string> quest_list;

    struct NetworkQuestObjective {
        uint8_t type = 0;
        uint32_t target_id = 0;
        uint16_t current = 0;
        uint16_t required = 0;
    };
    struct NetworkQuestEntry {
        uint32_t quest_id = 0;
        std::string name;
        bool is_completed = false;
        bool is_reward_taken = false;
        std::vector<NetworkQuestObjective> objectives;
    };
    std::vector<NetworkQuestEntry> network_quests;
    std::vector<uint32_t> completed_quest_ids;

    uint32_t dungeon_instance_id = 0;
    uint32_t dungeon_template_id = 0;
    uint8_t dungeon_state = 0;
    uint32_t dungeon_elapsed_sec = 0;
    bool dungeon_boss_active = false;
    std::string last_trigger_message;

    struct NetworkFamilyMember {
        uint32_t character_id = 0;
        std::string name;
        uint8_t relation = 1;
        uint32_t partner_id = 0;
        std::string partner_name;
        uint64_t married_date = 0;
        bool is_master = false;
    };
    uint32_t network_family_id = 0;
    std::string network_family_name;
    uint32_t network_family_master_id = 0;
    std::vector<NetworkFamilyMember> network_family_members;
    bool network_can_accept_marriage = false;
    bool network_can_reject_proposal = false;
    bool network_can_divorce = false;
    bool network_can_leave_family = false;
    bool network_can_create_family = true;
    uint32_t network_engaged_partner_id = 0;
    std::string network_engaged_partner_name;

    struct NetworkPetInfo {
        uint32_t pet_id = 0;
        uint32_t template_id = 0;
        std::string name;
        uint16_t level = 1;
        uint16_t hp = 100;
        uint16_t max_hp = 100;
        uint16_t satiation = 100;
        bool summoned = false;
        uint8_t evolution = 1;
        uint32_t exp = 0;
        uint32_t exp_to_next = 50;
        uint32_t feed_cost = 50;
        uint32_t evolve_cost = 500;
        bool can_summon = true;
    };
    NetworkPetInfo network_pet;

    struct NetworkSiegeTerritory {
        uint32_t territory_id = 0;
        std::string name;
        uint32_t owner_guild_id = 0;
        std::string owner_guild_name;
        uint16_t tax_rate = 10;
        uint64_t siege_time = 0;
        bool is_castle = false;
        uint8_t defense_bonus = 0;
        uint32_t attacker_guild_id = 0;
        std::string attacker_guild_name;
        uint32_t seconds_until_siege = 0;
        uint32_t tax_accumulated = 0;
        bool owned_by_player_guild = false;
        bool can_attack = false;
        bool can_manage_tax = false;
    };
    struct NetworkSiegeSchedule {
        uint32_t territory_id = 0;
        std::string territory_name;
        uint32_t attacker_guild_id = 0;
        std::string attacker_guild_name;
        uint32_t defender_guild_id = 0;
        std::string defender_guild_name;
        uint64_t siege_time = 0;
        uint32_t seconds_until = 0;
    };
    std::vector<NetworkSiegeTerritory> network_siege_territories;
    std::vector<NetworkSiegeSchedule> network_siege_schedules;
    uint32_t network_siege_player_guild_id = 0;
    std::string network_siege_player_guild_name;
    bool network_can_declare_siege = false;
    bool network_can_set_tax = false;

    struct NetworkTournamentEntry {
        uint32_t tournament_id = 0;
        std::string name;
        uint8_t state = 0;
        uint16_t registered = 0;
        uint16_t max_teams = 8;
        uint32_t prize_gold = 0;
        uint64_t registration_end = 0;
        uint8_t current_round = 0;
        uint8_t min_team_size = 1;
        uint8_t max_team_size = 6;
        uint32_t winner_guild_id = 0;
        bool player_registered = false;
        bool can_register = false;
        bool can_unregister = false;
        bool can_claim_prize = false;
        uint32_t seconds_until_start = 0;
    };
    struct NetworkTournamentTeam {
        uint32_t tournament_id = 0;
        uint32_t guild_id = 0;
        std::string guild_name;
        uint16_t seed = 0;
        bool eliminated = false;
    };
    struct NetworkTournamentMatch {
        uint32_t tournament_id = 0;
        uint8_t round = 0;
        uint8_t match_index = 0;
        uint32_t team1_guild_id = 0;
        uint32_t team2_guild_id = 0;
        uint32_t winner_guild_id = 0;
        bool completed = false;
    };
    std::vector<NetworkTournamentEntry> network_tournaments;
    std::vector<NetworkTournamentTeam> network_tournament_teams;
    std::vector<NetworkTournamentMatch> network_tournament_matches;
    uint32_t network_tournament_player_guild_id = 0;

    struct NetworkHousingFurniture {
        uint32_t furniture_id = 0;
        uint32_t item_id = 0;
        std::string name;
        float pos_x = 0;
        float pos_y = 0;
        float rot_y = 0;
    };
    struct NetworkHouseInfo {
        uint32_t house_id = 0;
        uint32_t owner_id = 0;
        std::string name;
        uint16_t map_id = 51;
        float pos_x = 0;
        float pos_y = 0;
        uint8_t house_type = 0;
        uint16_t furniture_count = 0;
        uint16_t max_furniture = 16;
        bool is_owner = false;
        bool can_enter = false;
        std::vector<NetworkHousingFurniture> furniture;
    };
    struct NetworkHouseTemplate {
        uint8_t template_id = 0;
        std::string name;
        uint32_t price = 0;
        uint16_t max_furniture = 16;
        uint16_t map_id = 51;
    };
    std::vector<NetworkHouseInfo> network_houses;
    std::vector<NetworkHouseTemplate> network_house_templates;
    uint32_t network_selected_house_id = 0;
    bool network_can_buy_house = false;

    struct NetworkCashShopItem {
        uint32_t item_id = 0;
        std::string name;
        std::string description;
        uint32_t price = 0;
        std::string category;
        uint8_t currency_type = 0;
        uint16_t stack_count = 1;
        uint16_t max_purchase = 99;
        uint16_t purchased_count = 0;
        bool can_afford = false;
        bool on_sale = false;
    };
    std::vector<NetworkCashShopItem> network_cashshop_items;
    int luna_points = 0;
    uint16_t network_battle_pass_level = 1;
    uint32_t network_battle_pass_xp = 0;
    uint32_t network_battle_pass_max_xp = 1000;
    bool network_battle_pass_active = false;
    std::string network_season_name;

    struct NetworkFarmPlot {
        uint8_t plot_id = 0;
        uint32_t seed_id = 0;
        std::string plant_name;
        uint8_t growth_stage = 0;
        uint8_t max_stages = 4;
        uint8_t growth_pct = 0;
        float growth_timer = 0;
        float growth_time = 60;
        bool watered = false;
        bool harvested = false;
        bool ready = false;
    };
    std::vector<NetworkFarmPlot> network_farm_plots;

    struct NetworkFarmSeed {
        uint32_t seed_id = 0;
        std::string name;
        uint16_t growth_time_sec = 0;
        uint32_t harvest_item_id = 0;
    };
    std::vector<NetworkFarmSeed> network_farm_seeds;
};
