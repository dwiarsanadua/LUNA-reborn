#pragma once
#include <string>
#include <vector>
#include <array>
#include <random>
#include <cstdint>
#include <game/ecs/components/Equipment.hpp>

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
    Equipment equipment;
    bool equipment_dirty = false;
    std::vector<std::string> skill_list;
    std::vector<std::string> quest_list;
};
