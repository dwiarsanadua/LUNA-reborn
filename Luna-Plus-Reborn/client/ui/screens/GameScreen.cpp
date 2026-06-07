#include "GameScreen.hpp"
#include <network/NetworkClient.hpp>
#include <rendering/CharacterRenderer.hpp>
#include <ecs/systems/GameDataDB.hpp>
#include <gameobjects/DurabilitySystem.hpp>
#include <flatbuffers/flatbuffers.h>
#include <Login_generated.h>
#include <Character_generated.h>
#include <Inventory_generated.h>
#include <Chat_generated.h>
#include <Entity_generated.h>
#include <Movement_generated.h>
#include <PacketType_generated.h>
#include <algorithm>
#include <string>
#include <spdlog/spdlog.h>
#include <cstdio>

void GameScreen::Init(GameState* state, NetworkClient* network) {
    Screen::Init(state, network);
}

void GameScreen::Enter() {
    prev_x_ = state_->player_x;
    prev_z_ = state_->player_z;
    hero_.Init(state_, audio_);
    hero_.SetPosition(state_->player_x, state_->player_y, state_->player_z);
    ObjectBalloon::Clear();
    sky_.Init();
    weather_.SetWeather(WeatherSystem::Rain);
    farm_.Init();
    consignment_.Init();
    family_.Init();
    family_.Propose(999, "NPC_Sweetheart", state_->selected_char, state_->name);
    family_.CreateFamily(state_->selected_char, "MyFamily");
    siege_.Init();
    dungeon_sys_.Init();
    costume_.Init();
    tournament_sys_.Init();
    pet_.Init("Fluffy", 1);
    cash_shop_.Init();
    trading_.Init();
    telemetry_.Init();
    economy_.Init();
    patcher_.Init();
    ops_.Init();
    ops_.AddAnnouncement("Welcome to Luna Plus Reborn! Press F1 for help.", 120);
    ops_.AddAnnouncement("Hotkeys: I=Inv K=Skills J=Quests C=Stats P=Party G=Guild", 300);
    ops_.AddAnnouncement("Server maintenance every Sunday 3:00 AM", 300);
    siege_.ConquerTerritory(1, 1, "PlayerGuild");
    siege_.ScheduleSiege(2, time(nullptr) + 86400, 2, "RivalGuild");
    siege_.SetTaxRate(1, 15);
    consignment_.ListItem(2, 101, 1, 200, 500);
    consignment_.ListItem(2, 102, 5, 50, 120);
    consignment_.ListItem(3, 201, 1, 1000, 2500);
    consignment_.ListItem(2, 103, 3, 150, 350);
    consignment_.ListItem(4, 301, 1, 5000, 12000);
    if (ui_) sky_.SetSampler(ui_->GetSampler(), ui_->GetWhiteTexture());
}

void GameScreen::Exit() {
    sky_.Shutdown();
}

bool GameScreen::HandlePacket(uint16_t type, const std::vector<uint8_t>& payload) {
    switch (type) {
    case 0x0208: {
        auto resp = flatbuffers::GetRoot<luna::protocol::EnterWorldResponse>(payload.data());
        if (resp->position()) {
            state_->player_x = resp->position()->x();
            state_->player_y = resp->position()->y();
            state_->player_z = resp->position()->z();
        }
        state_->map_id = resp->map_id();
        if (!state_->characters.empty()) {
            auto& ch = state_->characters[state_->selected_char];
            state_->name = ch.name;
            state_->level = ch.level;
            state_->hp = 500; state_->max_hp = 500 + ch.level * 20;
            state_->mp = 100; state_->max_mp = 100 + ch.level * 5;
        }
        state_->connecting = true;
        CharRenderer_Spawn(0, "assets_converted/mod_objs/d_man.glb",
                           state_->player_x, state_->player_y, state_->player_z, 0xffffffff);
        GameDataDB gdb;
        if (gdb.Open(GAME_DATA_PATH)) {
            auto mobs = gdb.GetAllMonsters();
            for (int i = 0; i < 5 && i < (int)mobs.size(); i++) {
                float x = (float)(rand() % 80) - 40;
                float z = (float)(rand() % 80) - 40;
                uint32_t mid = state_->next_entity_id++;
                Monster mob(mid, mobs[i].name, x, z, mobs[i].level);
                monsters_.push_back(mob);
            }
            gdb.Close();
        }
        return true;
    }
    case 0x0501: {
        auto msg = flatbuffers::GetRoot<luna::protocol::ChatMessage>(payload.data());
        std::string sender = msg->sender_name() ? msg->sender_name()->str() : "?";
        std::string text = msg->message() ? msg->message()->str() : "";
        state_->chat_messages.push_back("[" + sender + "] " + text);
        chat_panel_.AddMessage("[" + sender + "] " + text);
        if (state_->chat_messages.size() > 50) state_->chat_messages.erase(state_->chat_messages.begin());
        return true;
    }
    case 0x060A: {
        auto inv = flatbuffers::GetRoot<luna::protocol::InventoryData>(payload.data());
        state_->inventory.clear();
        if (inv->slots()) {
            for (auto s : *inv->slots()) {
                InvItem item;
                item.slot = s->slot_index(); item.id = s->item_id(); item.count = s->count();
                char buf[64]; snprintf(buf, 64, "Item_%u", item.id); item.name = buf;
                state_->inventory.push_back(item);
            }
        }
        state_->gold = inv->gold();
        return true;
    }
    case 0x1001: {
        auto spawn = flatbuffers::GetRoot<luna::protocol::EntitySpawn>(payload.data());
        RemoteEntity e;
        e.id = spawn->entity_id();
        e.name = spawn->name() ? spawn->name()->str() : "?";
        e.level = spawn->level(); e.hp_pct = (int)(spawn->hp_percent() * 100);
        if (spawn->position()) { e.x = spawn->position()->x(); e.y = spawn->position()->y(); e.z = spawn->position()->z(); }
        state_->entities.push_back(e);
        
        std::string modelPath = spawn->model_id() ? spawn->model_id()->str() : "monster_placeholder.glb";
        // Logic to find real asset path
        if (modelPath.find("/") == std::string::npos) {
            modelPath = "assets_converted/mod_objs/" + modelPath;
        }
        
        uint32_t colors[] = {0xff44cc44, 0xffcc4444, 0xffcccc44, 0xff44cccc, 0xffcc44cc};
        CharRenderer_Spawn(e.id, modelPath, e.x, e.y, e.z, colors[state_->next_entity_id++ % 5]);
        return true;
    }
    case 0x1004: { // EntityTransform
        auto trans = flatbuffers::GetRoot<luna::protocol::EntityTransform>(payload.data());
        uint32_t eid = trans->entity_id();
        for (auto& e : state_->entities) {
            if (e.id == eid) {
                if (trans->position()) {
                    e.x = trans->position()->x();
                    e.y = trans->position()->y();
                    e.z = trans->position()->z();
                    CharRenderer_Move(e.id, e.x, e.y, e.z, true); // Assuming moving
                }
                break;
            }
        }
        return true;
    }
    default: return false;
    }
}

bool GameScreen::HandleKey(int key, int scancode, int action, int mods) {
    (void)scancode; (void)mods;
    if (action != 1) return true;

    float speed = 0.5f;
    if (key == 87 || key == 265) { state_->player_z -= speed; hero_.Move(0, -speed); }
    else if (key == 83 || key == 264) { state_->player_z += speed; hero_.Move(0, speed); }
    else if (key == 65 || key == 263) { state_->player_x -= speed; hero_.Move(-speed, 0); }
    else if (key == 68 || key == 262) { state_->player_x += speed; hero_.Move(speed, 0); }
    else if (key == 32) SpawnRandomMonster();
    else if (key == 80) {
        state_->pk_mode = !state_->pk_mode;
        state_->chat_messages.push_back(state_->pk_mode ? "PK MODE: ON" : "PK MODE: OFF");
    }
    else if (key == 68) {
        if (!state_->in_dungeon) {
            state_->in_dungeon = true;
            state_->dungeon_map = rand() % 100 + 10;
            state_->chat_messages.push_back("Entering dungeon " + std::to_string(state_->dungeon_map) + "...");
            for (int i = 0; i < 3; i++) SpawnRandomMonster();
        } else {
            state_->in_dungeon = false;
            state_->chat_messages.push_back("Leaving dungeon...");
        }
    }
    else if (key == 66) {
        uint32_t id = state_->next_entity_id++;
        float x = (float)(rand() % 80) - 40, z = (float)(rand() % 80) - 40;
        state_->entities.push_back({id, "Boss Leostein", x, 0, z, 50, 5000});
        CharRenderer_Spawn(id, "assets_converted/mod_objs/monster_placeholder.glb", x, 0, z, 0xffff4444);
        state_->chat_messages.push_back("BOSS Leostein has appeared!");
    }
    else if (key == 73) {
        state_->inv_open = !state_->inv_open;
        if (state_->inv_open) inv_dlg_.Open(state_, &wm_);
        if (audio_) audio_->PlaySFXByCategory(AudioManager::SFX_UI, "window_open1.wav");
    }
    else if (key == 75) {
        state_->skill_open = !state_->skill_open;
        if (state_->skill_open) skill_dlg_.Open(state_, &wm_);
        if (audio_) audio_->PlaySFXByCategory(AudioManager::SFX_UI, "window_open1.wav");
    }
    else if (key == 74) {
        state_->quest_open = !state_->quest_open;
        if (state_->quest_open) quest_dlg_.Open(state_, &wm_);
        if (audio_) audio_->PlaySFXByCategory(AudioManager::SFX_UI, "window_open1.wav");
    }
    else if (key == 67) { // C key - Character Dialog
        state_->charinfo_open = !state_->charinfo_open;
        if (state_->charinfo_open) char_dlg_.Open(state_, &wm_);
        if (audio_) audio_->PlaySFXByCategory(AudioManager::SFX_UI, "window_open1.wav");
    }
    else if (key == 80) { // P key - Party
        state_->party_open = !state_->party_open;
        if (state_->party_open) party_dlg_.Open(&wm_);
        if (audio_) audio_->PlaySFXByCategory(AudioManager::SFX_UI, "window_open1.wav");
    }
    else if (key == 71) { // G key - Guild
        state_->guild_open = !state_->guild_open;
        if (state_->guild_open) guild_dlg_.Open(&wm_, &siege_);
        if (audio_) audio_->PlaySFXByCategory(AudioManager::SFX_UI, "window_open1.wav");
    }
    else if (key == 70) { // F key - Friends
        state_->friend_open = !state_->friend_open;
        if (state_->friend_open) friend_dlg_.Open(&wm_);
        if (audio_) audio_->PlaySFXByCategory(AudioManager::SFX_UI, "window_open1.wav");
    }
    else if (key == 72) { // H key - Farm / Harvest
        state_->farm_open = !state_->farm_open;
        if (audio_) audio_->PlaySFXByCategory(AudioManager::SFX_UI, "window_open1.wav");
    }
    else if (key == 79) { // O key - Options
        state_->options_open = !state_->options_open;
        if (state_->options_open) options_dlg_.Open(&wm_);
        if (audio_) audio_->PlaySFXByCategory(AudioManager::SFX_UI, "window_open1.wav");
    }
    else if (key == 66) { // B key - Bank/Storage
        state_->storage_open = !state_->storage_open;
        if (state_->storage_open) storage_dlg_.Open(state_, &wm_);
        if (audio_) audio_->PlaySFXByCategory(AudioManager::SFX_UI, "window_open1.wav");
    }
    else if (key == 112) { // F1 key - Help
        state_->help_open = !state_->help_open;
        if (audio_) audio_->PlaySFXByCategory(AudioManager::SFX_UI, "window_open1.wav");
    }
    else if (key == 77) { // M key - World Map
        state_->worldmap_open = !state_->worldmap_open;
        if (audio_) audio_->PlaySFXByCategory(AudioManager::SFX_UI, "window_open1.wav");
    }
    else if (key == 85) { // U key - Trade
        state_->trade_open = !state_->trade_open;
        if (state_->trade_open) trade_dlg_.Open(state_, &trading_);
        if (audio_) audio_->PlaySFXByCategory(AudioManager::SFX_UI, "window_open1.wav");
    }
    else if (key == 86) { // V key - Fishing
        state_->fishing_open = !state_->fishing_open;
        if (state_->fishing_open) {
            fishing_dlg_.Open(state_);
            fishing_dlg_.SetResultCallback([this](bool success, int fish_type) {
                (void)fish_type;
                if (success) {
                    state_->gold += 15;
                    state_->chat_messages.push_back("Caught a fish! +15 gold");
                } else {
                    state_->chat_messages.push_back("Fish got away!");
                }
                state_->fishing_open = false;
            });
        }
        if (audio_) audio_->PlaySFXByCategory(AudioManager::SFX_UI, "window_open1.wav");
    }
    else if (key == 89) { // Y key - Auction/Consignment
        state_->consignment_open = !state_->consignment_open;
        if (state_->consignment_open) consignment_dlg_.Open(state_, &wm_, &consignment_);
        if (audio_) audio_->PlaySFXByCategory(AudioManager::SFX_UI, "window_open1.wav");
    }
    else if (key == 69) { // E key - Mail
        state_->mail_open = !state_->mail_open;
        if (state_->mail_open) mail_dlg_.Open(state_, &wm_);
        if (audio_) audio_->PlaySFXByCategory(AudioManager::SFX_UI, "window_open1.wav");
    }
    else if (key == 81) { // Q key - Cash Shop
        state_->cashshop_open = !state_->cashshop_open;
        if (state_->cashshop_open) cash_shop_dlg_.Open(state_, &wm_, &cash_shop_);
        if (audio_) audio_->PlaySFXByCategory(AudioManager::SFX_UI, "window_open1.wav");
    }
    else if (key == 262) { // Right Arrow key - Key Bindings
        state_->keybind_open = !state_->keybind_open;
        if (state_->keybind_open) keybind_dlg_.Open(state_, &wm_);
        if (audio_) audio_->PlaySFXByCategory(AudioManager::SFX_UI, "window_open1.wav");
    }
    else if (key == 291) { // F3 key - Pets
        state_->pet_open = !state_->pet_open;
        if (state_->pet_open) {
    pet_.Init("Fluffy", 1);
    // Demo buffs
    CharacterStats dummy;
    buff_sys_.Apply(BuffSystem::MakeStatBuff("ATK Boost", 120.0f, "attack", 20.0f), dummy);
    buff_sys_.Apply(BuffSystem::MakeStatBuff("DEF Boost", 90.0f, "defense", 10.0f), dummy);
    buff_sys_.Apply(BuffSystem::MakeStatBuff("Move Speed", 60.0f, "move_speed", 0.3f, true), dummy);
            pet_dlg_.Open(state_, &wm_, &pet_);
        }
        if (audio_) audio_->PlaySFXByCategory(AudioManager::SFX_UI, "window_open1.wav");
    }
    else if (key == 292) { // F4 key - Cooking
        state_->cooking_open = !state_->cooking_open;
        if (state_->cooking_open) cooking_dlg_.Open(state_, &wm_);
        if (audio_) audio_->PlaySFXByCategory(AudioManager::SFX_UI, "window_open1.wav");
    }
    else if (key == 293) { // F5 key - Mounts
        state_->mount_open = !state_->mount_open;
        if (state_->mount_open) mount_dlg_.Open(state_, &wm_);
        if (audio_) audio_->PlaySFXByCategory(AudioManager::SFX_UI, "window_open1.wav");
    }
    else if (key == 294) { // F6 key - Tournament
        state_->tournament_open = !state_->tournament_open;
        if (state_->tournament_open) tournament_dlg_.Open(state_, &wm_, &tournament_sys_);
        if (audio_) audio_->PlaySFXByCategory(AudioManager::SFX_UI, "window_open1.wav");
    }
    else if (key == 295) { // F7 key - Avatar Customization
        state_->avatar_open = !state_->avatar_open;
        if (state_->avatar_open) avatar_dlg_.Open(state_, &wm_);
        if (audio_) audio_->PlaySFXByCategory(AudioManager::SFX_UI, "window_open1.wav");
    }
    else if (key == 296) { // F8 key - Item Upgrade
        state_->upgrade_open = !state_->upgrade_open;
        if (state_->upgrade_open) upgrade_dlg_.Open(state_, &wm_);
        if (audio_) audio_->PlaySFXByCategory(AudioManager::SFX_UI, "window_open1.wav");
    }
    else if (key == 297) { // F9 key - Macros
        state_->macro_open = !state_->macro_open;
        if (state_->macro_open) macro_dlg_.Open(state_, &wm_);
        if (audio_) audio_->PlaySFXByCategory(AudioManager::SFX_UI, "window_open1.wav");
    }
    else if (key == 90) { // Z key - Costume/Equipment
        state_->costume_open = !state_->costume_open;
        if (state_->costume_open) costume_dlg_.Open(state_, &wm_, &costume_);
        if (audio_) audio_->PlaySFXByCategory(AudioManager::SFX_UI, "window_open1.wav");
    }
    else if (key == 88) { // X key - Dungeons
        state_->dungeon_open = !state_->dungeon_open;
        if (state_->dungeon_open) dungeon_dlg_.Open(state_, &wm_, &dungeon_sys_);
        if (audio_) audio_->PlaySFXByCategory(AudioManager::SFX_UI, "window_open1.wav");
    }
    else if (key == 82) { // R key - Housing
        state_->housing_open = !state_->housing_open;
        if (state_->housing_open) housing_dlg_.Open(state_, &wm_);
        if (audio_) audio_->PlaySFXByCategory(AudioManager::SFX_UI, "window_open1.wav");
    }
    else if (key == 76) { // L key - Family & Couple
        state_->family_open = !state_->family_open;
        if (state_->family_open) family_dlg_.Open(state_, &wm_, &family_);
        if (audio_) audio_->PlaySFXByCategory(AudioManager::SFX_UI, "window_open1.wav");
    }
    else if (key == 78) {
        state_->npc_open = !state_->npc_open;
        if (state_->npc_open) {
            state_->npc_text = "\"Need something? I'm the town Blacksmith.\"\n\n[1] Buy items  [2] Sell  [3] Repair  [4] Enchant  [5] Compose  [6] Leave";
            state_->npc_id = 1;
            npc_dlg_.Open(state_, 1, "Blacksmith");
        }
    }
    else if (key == 84) chat_panel_.Toggle();
    else if (key == 257 && state_->chat_open) {
        state_->chat_open = false;
        if (!state_->chat_input.empty()) {
            std::string cmd = state_->chat_input;
            // Check for slash commands
            if (cmd[0] == '/' && cmd.size() > 1) {
                size_t space = cmd.find(' ');
                std::string verb = (space != std::string::npos) ? cmd.substr(1, space - 1) : cmd.substr(1);
                std::string arg = (space != std::string::npos) ? cmd.substr(space + 1) : "";
                
                if (verb == "teleport" || verb == "tp" || verb == "goto") {
                    if (!arg.empty()) {
                        // Teleport to map
                        int map_id = std::atoi(arg.c_str());
                        if (map_id > 0) {
                            state_->map_id = map_id;
                            if (map_) {
                                map_->Unload();
                                map_->Load(std::to_string(map_id));
                            }
                            state_->chat_messages.push_back("Teleported to map " + std::to_string(map_id));
                            if (audio_) {
                                switch (map_id) {
                                    case 13: audio_->PlayBGM("14_Red_Orc_Outpost"); break;
                                    case 20: audio_->PlayBGM("20_Alker_Harbor"); break;
                                    case 51: audio_->PlayBGM("20_Alker_Harbor"); break;
                                    case 14: audio_->PlayBGM("15_Moon_Blind_Forest"); break;
                                    case 15: audio_->PlayBGM("16_17_Haunted_Mine"); break;
                                    default: audio_->PlayBGM("BGM_Login"); break;
                                }
                            }
                        }
                    } else {
                        state_->chat_messages.push_back("Usage: /teleport [map_id] — Available: 13,14,15,19,20,21,22,23,25,26,27,28,31,32,41,51,55,56,60,63,64,71,74,75,96");
                    }
                } else if (verb == "maps" || verb == "maplist") {
                    state_->chat_messages.push_back("Available maps: 13,14,15,19,20,21,22,23,25,26,27,28,31,32,41,51,55,56,60,63,64,71,74,75,96");
                } else if (verb == "help" || verb == "h") {
                    state_->chat_messages.push_back("Commands: /teleport [map_id], /maps, /nextmap, /prevmap");
                } else if (verb == "nextmap") {
                    int available[] = {13,14,15,19,20,21,22,23,25,26,27,28,31,32,41,51,55,56,60,63,64,71,74,75,96};
                    int count = sizeof(available)/sizeof(available[0]);
                    int cur = state_->map_id;
                    int next = available[0];
                    for (int i = 0; i < count; i++) {
                        if (available[i] == cur && i + 1 < count) { next = available[i + 1]; break; }
                        if (available[i] == cur) { next = available[0]; break; }
                    }
                    if (cur == 0) next = available[0];
                    state_->map_id = next;
                    if (map_) { map_->Unload(); map_->Load(std::to_string(next)); }
                    state_->chat_messages.push_back("Teleported to map " + std::to_string(next));
                } else if (verb == "prevmap") {
                    int available[] = {13,14,15,19,20,21,22,23,25,26,27,28,31,32,41,51,55,56,60,63,64,71,74,75,96};
                    int count = sizeof(available)/sizeof(available[0]);
                    int cur = state_->map_id;
                    int prev = available[count - 1];
                    for (int i = 0; i < count; i++) {
                        if (available[i] == cur && i > 0) { prev = available[i - 1]; break; }
                    }
                    state_->map_id = prev;
                    if (map_) { map_->Unload(); map_->Load(std::to_string(prev)); }
                    state_->chat_messages.push_back("Teleported to map " + std::to_string(prev));
                } else {
                    state_->chat_messages.push_back("Unknown command. Try /help");
                }
            } else if (network_->IsConnected()) {
                flatbuffers::FlatBufferBuilder fbb;
                auto chatMsg = luna::protocol::CreateChatMessageDirect(fbb, 0, state_->name.c_str(), state_->chat_input.c_str(), luna::protocol::ChatChannel_All, 0);
                fbb.Finish(chatMsg);
                network_->SendPacket(luna::protocol::PacketType_MP_CHAT_ALL_SYN, fbb.GetBufferPointer(), fbb.GetSize());
            }
        }
        state_->chat_input.clear();
    }
    else if ((key == 259 || key == 263) && state_->chat_open && !state_->chat_input.empty()) {
        state_->chat_input.pop_back();
    }
    else if (key == 256) state_->chat_messages.clear();

    // Farm actions (when farm panel is open)
    if (state_->farm_open) {
        if (key >= 49 && key <= 57) { // Keys 1-9: plant seed in plot
            int plot_id = key - 49;
            // Plant first available seed
            auto seeds = farm_.GetAllSeeds();
            if (!seeds.empty()) {
                if (farm_.Plant(plot_id, seeds[0].id)) {
                    state_->chat_messages.push_back("Planted " + seeds[0].name + "!");
                } else {
                    state_->chat_messages.push_back("Cannot plant there.");
                }
            }
        }
        else if (key == 87) { // W key: water random plot
            bool watered = false;
            for (int i = 0; i < 9; i++) {
                auto* plot = farm_.GetPlot(i);
                if (plot && plot->seed_id > 0 && !plot->watered && !plot->harvested) {
                    farm_.Water(i);
                    state_->chat_messages.push_back("Watered plot " + std::to_string(i+1));
                    watered = true; break;
                }
            }
            if (!watered) state_->chat_messages.push_back("Nothing to water.");
        }
        else if (key == 82) { // R key: harvest
            for (int i = 0; i < 9; i++) {
                auto* plot = farm_.GetPlot(i);
                if (plot && plot->growth_stage >= plot->max_stages && !plot->harvested) {
                    int item_id, count;
                    if (farm_.Harvest(i, item_id, count)) {
                        state_->gold += count * 10;
                        state_->chat_messages.push_back("Harvested! Got " + std::to_string(count) + " items.");
                        break;
                    }
                }
            }
        }
    }

    // NPC dialog choices
    if (state_->npc_open && key >= 49 && key <= 54) {
        int choice = key - 49 + 1;
        if (choice == 1) { // Buy items
            state_->npc_text = "--- SHOP ---\n[1] Health Potion (50g)\n[2] Mana Potion (80g)\n[3] HP Regen Ring (500g)\n[4] Scroll of Recall (200g)\n[5] Back";
            state_->npc_sub_mode = 1;
        } else if (choice == 2) { // Sell items
            if (!state_->inventory.empty()) {
                auto& item = state_->inventory[0];
                int price = 10 + item.enchant * 5;
                state_->gold += price;
                state_->npc_text = "Sold " + item.name + " for " + std::to_string(price) + " gold!";
                state_->inventory.erase(state_->inventory.begin());
            } else {
                state_->npc_text = "You have nothing to sell.";
            }
        } else if (choice == 3) { // Repair
            DurabilitySystem ds;
            int total_cost = 0;
            int repaired = 0;
            for (auto& item : state_->inventory) {
                auto* dur = ds.GetDurability(item.id);
                if (dur && dur->NeedsRepair()) {
                    int cost = ds.GetRepairCost(item.id);
                    if (state_->gold >= cost) {
                        state_->gold -= cost;
                        ds.Repair(item.id);
                        total_cost += cost;
                        repaired++;
                    }
                }
            }
            if (repaired > 0)
                state_->npc_text = "Repaired " + std::to_string(repaired) + " items for " + std::to_string(total_cost) + " gold!";
            else
                state_->npc_text = "No repairs needed.";
        } else if (choice == 4) { // Enchant
            if (state_->npc_sub_mode == 2) {
                // Coming back from sub-mode
                state_->npc_text = "\"Need something? I'm the town Blacksmith.\"\n\n[1] Buy items  [2] Sell  [3] Repair  [4] Enchant  [5] Compose  [6] Leave";
                state_->npc_sub_mode = 0;
            } else {
                for (auto& item : state_->inventory) {
                    if (item.enchant < 15) {
                        int cost = 100 + item.enchant * 50;
                        if (state_->gold >= cost) {
                            state_->gold -= cost;
                            item.enchant++;
                            state_->npc_text = item.name + " enchanted to +" + std::to_string(item.enchant) + "!";
                        } else {
                            state_->npc_text = "Not enough gold! Need " + std::to_string(cost) + "g.";
                        }
                    } else {
                        state_->npc_text = item.name + " is already max level!";
                    }
                    break;
                }
            }
        } else if (choice == 5) { // Compose
            if (!state_->inventory.empty()) {
                auto& first = state_->inventory[0];
                if (first.count >= 3) {
                    first.count -= 3;
                    if (first.count <= 0) state_->inventory.erase(state_->inventory.begin());
                    state_->inventory.push_back({first.id + 1000, "Enhanced " + first.name, 1, (int)state_->inventory.size()});
                    state_->npc_text = "Crafted!";
                } else {
                    state_->npc_text = "Need 3 of the same item!";
                }
            }
        } else if (choice == 6) { // Leave
            state_->npc_open = false;
            state_->npc_sub_mode = 0;
        } else if (choice >= 49 && choice <= 53 && state_->npc_sub_mode == 1) {
            // Shop sub-items
            const char* items[] = {"Health Potion", "Mana Potion", "HP Regen Ring", "Scroll of Recall"};
            int prices[] = {50, 80, 500, 200};
            int idx = choice - 49;
            if (idx < 4) {
                if (state_->gold >= prices[idx]) {
                    state_->gold -= prices[idx];
                    state_->inventory.push_back({(uint32_t)(1000 + idx), items[idx], 1, (int)state_->inventory.size()});
                    state_->npc_text = std::string("Bought ") + items[idx] + "!";
                } else {
                    state_->npc_text = std::string("Not enough gold! Need ") + std::to_string(prices[idx]) + "g.";
                }
            } else {
                state_->npc_text = "\"Need something? I'm the town Blacksmith.\"\n\n[1] Buy items  [2] Sell  [3] Repair  [4] Enchant  [5] Compose  [6] Leave";
                state_->npc_sub_mode = 0;
            }
        }
        npc_dlg_.UpdateFromState(state_);
    }
    return true;
}

bool GameScreen::HandleChar(unsigned int codepoint) {
    if (state_->chat_open && codepoint >= 32 && codepoint <= 126) {
        state_->chat_input += (char)codepoint;
        return true;
    }
    return false;
}

void GameScreen::Update(float dt) {
    effect_mgr_.Update(dt);
    sky_.Update(dt);
    weather_.Update(dt, state_->player_x, state_->player_z);
    state_->shake_x = effect_mgr_.GetShakeOffsetX();
    state_->shake_y = effect_mgr_.GetShakeOffsetY();
    if (state_->connecting) {
        state_->connecting = false;
        network_->Disconnect();
        if (network_->Connect("127.0.0.1", 8300)) spdlog::info("Connected to MapServer");
        if (audio_) {
            switch (state_->map_id) {
                case 13: audio_->PlayBGM("14_Red_Orc_Outpost"); break;
                case 14: audio_->PlayBGM("15_Moon_Blind_Forest"); break;
                case 15: audio_->PlayBGM("16_17_Haunted_Mine"); break;
                case 19: audio_->PlayBGM("19_The_Gate_Of_Alker"); break;
                case 20: audio_->PlayBGM("20_Alker_Harbor"); break;
                case 21: audio_->PlayBGM("21_Ruins_Of_Draconian"); break;
                case 22: audio_->PlayBGM("22_Zakandia"); break;
                case 23: audio_->PlayBGM("23_Tarintus"); break;
                case 25: audio_->PlayBGM("25_MontBlanc_Port"); break;
                case 26: audio_->PlayBGM("26_Dried_Gazell_Fall"); break;
                case 27: audio_->PlayBGM("27_Zakandia_Outpost"); break;
                case 28: audio_->PlayBGM("28_The_Dark_Portal"); break;
                case 29: audio_->PlayBGM("29_Distorted_Crevice"); break;
                case 31: audio_->PlayBGM("31_Dwarf_Village"); break;
                case 32: audio_->PlayBGM("32_Darkon_Outpost"); break;
                case 41: audio_->PlayBGM("41_Elven_Village"); break;
                case 51: audio_->PlayBGM("20_Alker_Harbor"); break;
                case 55: audio_->PlayBGM("55_Char_World"); break;
                case 56: audio_->PlayBGM("56_Snowfield"); break;
                case 60: audio_->PlayBGM("60_Rosefields"); break;
                case 63: audio_->PlayBGM("63_Wellis_Lab"); break;
                case 64: audio_->PlayBGM("64_Cryptic"); break;
                case 71: audio_->PlayBGM("71_Human_World"); break;
                case 72: audio_->PlayBGM("72_Ash_Plateau"); break;
                case 73: audio_->PlayBGM("73_Shade_Ruins"); break;
                case 74: audio_->PlayBGM("74_Forsaken_Outpost"); break;
                case 75: audio_->PlayBGM("75_Silent_Coast"); break;
                case 81: audio_->PlayBGM("81_Floating_Island"); break;
                case 91: audio_->PlayBGM("91_Dragon_Lair"); break;
                default: audio_->PlayBGM("BGM_Login"); break;
            }
        }
    }

    if (state_->equipment_dirty) {
        hero_.UpdateEquipment();
        state_->equipment_dirty = false;
    }
    hero_.Update(dt);
    state_->player_y = hero_.GetY();

    for (auto& m : monsters_) {
        float mh = terrain_ ? terrain_->GetHeight(m.GetX(), m.GetZ()) + 0.5f : 0.5f;
        m.SetPosition(m.GetX(), mh, m.GetZ());
        m.Update(dt);
        
        // Auto-aggro if player is close
        float dist = m.GetDistance(hero_.GetX(), hero_.GetZ());
        if (dist < 12.0f && m.IsAlive()) {
            m.SetTarget(0);
        } else {
            m.SetTarget(UINT32_MAX);
        }
    }

    DoCombat(dt);

    for (auto& df : state_->damage_floats) df.life -= dt;
    state_->damage_floats.erase(
        std::remove_if(state_->damage_floats.begin(), state_->damage_floats.end(),
            [](auto& df) { return df.life <= 0; }), state_->damage_floats.end());

    { entt::registry dummy; particleSys_.Update(dummy, dt); }

    DoLevelUp();

    chat_panel_.Update(state_, dt);

    if (state_->hp <= 0) {
        state_->hp = state_->max_hp / 2;
        if (state_->pk_mode) {
            int lostExp = state_->exp / 10;
            state_->exp = std::max(0, state_->exp - lostExp);
            state_->chat_messages.push_back("PK death! Lost " + std::to_string(lostExp) + " XP");
        } else state_->chat_messages.push_back("You died! Respawning...");
    }

    ObjectBalloon::UpdateAll(dt);
    farm_.Update(dt);
    consignment_.Update(dt);
    dungeon_sys_.Update(dt);
    if (state_->fishing_open) fishing_dlg_.Update(dt);
    if (state_->family_open) family_dlg_.UpdateFromState(state_);
    if (state_->housing_open) housing_dlg_.UpdateFromState(state_);
    if (state_->dungeon_open) dungeon_dlg_.UpdateFromState(state_);
    if (state_->costume_open) costume_dlg_.UpdateFromState(state_);
    if (state_->cashshop_open) cash_shop_dlg_.UpdateFromState(state_);
    if (state_->mail_open) mail_dlg_.UpdateFromState(state_);
    if (state_->keybind_open) keybind_dlg_.UpdateFromState(state_);
    if (state_->chatroom_open) chatroom_dlg_.UpdateFromState(state_);
    if (state_->pet_open) pet_dlg_.UpdateFromState(state_);
    if (state_->cooking_open) cooking_dlg_.UpdateFromState(state_);
    if (state_->mount_open) mount_dlg_.UpdateFromState(state_);
    if (state_->tournament_open) tournament_dlg_.UpdateFromState(state_);
    if (state_->avatar_open) avatar_dlg_.UpdateFromState(state_);
    if (state_->upgrade_open) upgrade_dlg_.UpdateFromState(state_);
    if (state_->macro_open) macro_dlg_.UpdateFromState(state_);
    ops_.Update(dt);
    trading_.Update(dt);
    telemetry_.Update(dt);
    economy_.Update(dt);
    tournament_sys_.Update(dt);
    { CharacterStats dummy; buff_sys_.Update(dt, dummy); }
    wm_.Update(dt, 0, 0, false, false);
    if (state_->consignment_open) consignment_dlg_.UpdateFromState(state_);

    if (audio_) audio_->Update();
}

void GameScreen::DoCombat(float dt) {
    // Animation Lock & Cast Time system
    // 3-phase combat: IDLE -> CASTING (0.3s) -> RECOVERY (0.5s) -> IDLE
    if (state_->combat_anim_lock > 0) {
        state_->combat_anim_lock -= dt;
        // During animation lock, movement is prevented
        return;
    }
    if (state_->combat_cast_time > 0) {
        state_->combat_cast_time -= dt;
        // During cast, skill hasn't hit yet
        return;
    }

    // Global cooldown between attacks (1.0s)
    state_->combat_timer += dt;
    if (state_->combat_timer < 1.0f) return;
    state_->combat_timer = 0;

    // Begin attack: set cast time + animation lock
    state_->combat_cast_time = 0.3f;    // 0.3s cast before damage
    state_->combat_anim_lock = 0.8f;    // 0.8s total animation lock

    // Drain weapon durability on attack
    if (!state_->inventory.empty()) {
        durability_.OnAttack(state_->inventory[0].id);
        if (durability_.IsBroken(state_->inventory[0].id)) {
            state_->chat_messages.push_back("Your weapon is broken! Repair at Blacksmith.");
        }
    }

    int dmg = 20 + hero_.GetLevel();
    if (state_->pk_mode) {
        hero_.TakeDamage(dmg / 2);
        // Drain armor on hit
        if (state_->inventory.size() > 1) {
            durability_.OnHit(state_->inventory[1].id);
        }
        effect_mgr_.SpawnDamageNumber(hero_.GetX(), hero_.GetY() + 1.5f, hero_.GetZ(), dmg/2, DamageType::Normal);
        effect_mgr_.SpawnCameraShake(3.0f, 0.2f);
        particleSys_.EmitAt(glm::vec3(hero_.GetX(), hero_.GetY() + 1.0f, hero_.GetZ()), ParticleEffect::HitSpark, 0xFFFF4444);
    }
    for (auto& m : monsters_) {
        if (!m.IsAlive()) continue;
        float dist = m.GetDistance(hero_.GetX(), hero_.GetZ());
        if (dist < 5.0f) {
            int roll = rand() % 100;
            DamageType dtype = DamageType::Normal;
            int dmgDealt = m.IsBoss() ? dmg / 2 : dmg * 2;
            if (roll < 5) { dmgDealt = 0; dtype = DamageType::Miss; }
            else if (roll < 20) { dmgDealt = (int)(dmgDealt * 1.5f); dtype = DamageType::Crit; }
            
            m.TakeDamage(dmgDealt);
            effect_mgr_.SpawnDamageNumber(m.GetX(), m.GetY() + 1.5f, m.GetZ(), dmgDealt, dtype);
            effect_mgr_.SpawnCameraShake(4.0f, 0.25f);
            effect_mgr_.SpawnAnimation(m.GetID(), 1);
            effect_mgr_.SpawnSound("Weapon_NoWeapon_Upper.wav", m.GetX(), m.GetY(), m.GetZ());
            // Hit spark particles
            particleSys_.EmitAt(glm::vec3(m.GetX(), m.GetY() + 1.0f, m.GetZ()), ParticleEffect::HitSpark, 0xFFFFAA44);
            state_->target_entity = (int32_t)m.GetID();
            
            if (!m.IsAlive()) {
                state_->monster_kills++;
                int xpGain = m.IsBoss() ? 500 + m.GetLevel() * 100 : 50 + m.GetLevel() * 10;
                int goldGain = m.IsBoss() ? 100 + m.GetLevel() * 20 : 10 + m.GetLevel() * 5;
                hero_.AddGold(goldGain); hero_.AddEXP(xpGain);
                CharRenderer_Remove(m.GetID());
                state_->chat_messages.push_back(m.GetName() + " defeated! +" + std::to_string(xpGain) + " XP");
                effect_mgr_.SpawnBillboard(m.GetX(), m.GetY() + 2.0f, m.GetZ(), 0xFFFFD700, 24, 1.5f);
                if (m.IsBoss()) {
                    state_->chat_messages.push_back("BOSS reward: +" + std::to_string(goldGain) + " gold!");
                    particleSys_.EmitAt(glm::vec3(m.GetX(), m.GetY(), m.GetZ()), ParticleEffect::LevelUp, 0xFFFFD700);
                    effect_mgr_.SpawnCameraShake(12.0f, 0.6f);
                }
                for (auto& q : state_->quest_list) {
                    auto killpos = q.find("Kill");
                    if (killpos != std::string::npos) {
                        auto sep = q.find("("); auto slash = q.find("/");
                        if (sep != std::string::npos && slash != std::string::npos) {
                            int cur = std::stoi(q.substr(sep + 1, slash - sep - 1));
                            q = q.substr(0, sep + 1) + std::to_string(cur + 1) + q.substr(slash);
                        }
                    }
                }
            }
            break;
        }
    }
    // Remove dead & respawn
    monsters_.erase(std::remove_if(monsters_.begin(), monsters_.end(),
        [](auto& m) { return !m.IsAlive(); }), monsters_.end());
    while ((int)monsters_.size() < 5) SpawnRandomMonster();
}

void GameScreen::DoLevelUp() {
    if (state_->exp < state_->exp_next) return;
    state_->exp -= state_->exp_next;
    state_->level++;
    state_->exp_next = state_->level * 500;
    state_->max_hp = 500 + state_->level * 20;
    state_->hp = state_->max_hp;
    state_->chat_messages.push_back("Level Up! You are now Lv." + std::to_string(state_->level));
    particleSys_.EmitAt(glm::vec3(state_->player_x, state_->player_y, state_->player_z), ParticleEffect::LevelUp, 0xFFFFD700);
    effect_mgr_.SpawnBillboard(state_->player_x, state_->player_y + 2.0f, state_->player_z, 0xFFFFD700, 48, 2.0f);
    effect_mgr_.SpawnCameraShake(8.0f, 0.5f);
    effect_mgr_.SpawnSound("button_ok.wav");
    if (audio_) audio_->PlaySFXByCategory(AudioManager::SFX_UI, "button_ok.wav");
}

void GameScreen::SpawnRandomMonster() {
    uint32_t id = state_->next_entity_id++;
    float x = (float)(rand() % 80) - 40, z = (float)(rand() % 80) - 40;
    int level = 1 + (rand() % 20);
    const char* names[] = {"Goblin", "Wolf", "Bear", "Slime", "Orc", "Skeleton", "Bat", "Spider"};
    monsters_.emplace_back(id, names[rand() % 8], x, z, level);
}

void GameScreen::Render(UIRenderer& ui) {
    RenderUI(ui);
}

void GameScreen::Render(UIRenderer& ui, const glm::mat4& view, const glm::mat4& proj) {
    // Build environment data from day/night cycle
    EnvData env;
    env.light_dir = glm::vec4(sky_.GetLightDirection(), 0.0f);
    env.fog_color = glm::vec4(sky_.GetFogColor(), 1.0f);
    env.fog_data  = glm::vec4(30.0f, 150.0f, 0.8f, 0.0f);

    // Sky dome (background, view 0)
    sky_.Render(ui, view, proj);

    // 3D scene rendering with environment lighting
    if (terrain_) terrain_->Render(view, proj, env);
    if (props_) props_->Render(view, proj, env);
    CharRenderer_Render(view, proj, 0, env);

    // Particles
    std::vector<glm::vec3> ppos; std::vector<uint32_t> pcol; std::vector<float> psiz;
    for (auto& p : particleSys_.GetActiveParticles()) { ppos.push_back(p.position); pcol.push_back(p.color); psiz.push_back(p.size); }
    if (particles_) particles_->Render(view, proj, ppos, pcol, psiz);

    RenderUI(ui);
}

void GameScreen::RenderUI(UIRenderer& ui) {
    float lw = ui.logicalWidth;
    float lh = ui.logicalHeight;

    // Weather overlay
    weather_.Render(ui, state_->player_x, state_->player_z);

    // === HUD (via Hero class) ===
    hero_.Render(ui);

    // Stats line
    ui.DrawText(10, 82, 0xffcccccc, "Kills: %d%s", state_->monster_kills, state_->pk_mode ? " [PK]" : "");

    // Status Icons (buffs/debuffs)
    {
        auto& buffs = buff_sys_.GetActiveBuffs();
        if (!buffs.empty()) {
            float bx = 10, by = 215;
            ui.DrawText(bx, by, 0xffffcc88, "BUFFS (%zu)", buffs.size()); by += 14;
            for (auto& b : buffs) {
                UIColor bc = (b.type == BuffType::DamageOverTime || b.type == BuffType::Stun) ?
                    UIColor{255, 60, 60, 220} : UIColor{60, 200, 60, 220};
                float remaining = b.permanent ? 0 : (b.duration > 0 ? (1.0f - b.elapsed / b.duration) : 1.0f);
                if (b.permanent) {
                    ui.DrawText(bx + 2, by, 0xffffffff, "%s [Permanent]", b.name.c_str());
                } else {
                    ui.DrawText(bx + 2, by, 0xffffffff, "%s (%.1fs)", b.name.c_str(), b.duration - b.elapsed);
                }
                ui.DrawBar(bx + 100, by + 2, 60, 10, remaining, bc, {40, 40, 40, 180});
                by += 14;
            }
        }
    }

    // Target frame
    if (state_->target_entity >= 0) {
        for (auto& m : monsters_) {
            if ((int32_t)m.GetID() == state_->target_entity) {
                ui.DrawWindow(10, 115, 240, 75, "TARGET", {80, 30, 30, 200});
                ui.DrawText(18, 135, 0xffffffff, "%s  Lv.%d", m.GetName().c_str(), m.GetLevel());
                float hp = (float)m.GetHP() / std::max(1, m.GetMaxHP());
                ui.DrawBar(18, 155, 220, 16, hp, {255, 60, 60, 255}, {60, 0, 0, 180});
                ui.DrawText(20, 155, 0xffffffff, "%d/%d", m.GetHP(), m.GetMaxHP());
                break;
            }
        }
    }

    // Server status
    uint32_t status_col = network_->IsConnected() ? 0xff44ff44 : 0xffff4444;
    ui.DrawText(10, 195, status_col, "● %s", network_->IsConnected() ? "Connected" : "Disconnected");

    // === Minimap (top-right corner) ===
    float mm_size = 160;
    float mm_x = lw - mm_size - 10, mm_y = 10;
    float mm_cx = mm_x + mm_size/2, mm_cy = mm_y + mm_size/2;
    
    // Load minimap texture for current map
    std::string mapIdStr = std::to_string(state_->map_id);
    std::string mmName = "mini_" + mapIdStr + "_ful";
    TextureInfo mmTex = ui.LoadTexture("mm_" + mapIdStr,
        mmName + ".tif");
    
    if (bgfx::isValid(mmTex.handle)) {
        ui.DrawImage(mm_x, mm_y, mm_size, mm_size, mmTex.handle);
        ui.DrawBorder(mm_x, mm_y, mm_size, mm_size, {100, 180, 255, 150});
    } else {
        ui.DrawRect(mm_x, mm_y, mm_size, mm_size, {20, 30, 50, 220});
        ui.DrawBorder(mm_x, mm_y, mm_size, mm_size, {100, 180, 255, 150});
        ui.DrawText(mm_x + 4, mm_y + 2, 0x88ffffff, "MINIMAP");
    }
    
    float mm_scale = 3.0f;
    // Player dot (center, green)
    ui.DrawRect(mm_cx - 3, mm_cy - 3, 6, 6, {100, 255, 100, 255});
    // Entity dots
    for (auto& e : state_->entities) {
        float dx = (e.x - state_->player_x) * mm_scale;
        float dz = (e.z - state_->player_z) * mm_scale;
        if (fabs(dx) > mm_size/2 - 4 || fabs(dz) > mm_size/2 - 4) continue;
        bool isBoss = e.name.find("Boss") != std::string::npos;
        ui.DrawRect(mm_cx + dx - 2, mm_cy + dz - 2, 4, 4, isBoss ? UIColor{255,50,50,255} : UIColor{255,100,50,200});
    }

    // === Effects ===
    effect_mgr_.Render(ui, glm::mat4(1), glm::mat4(1));

    // === Chat Panel ===
    chat_panel_.Render(ui, state_);

    // === Hotbar (bottom center) with cooldown overlay ===
    {
        float hb_y = lh - 50.0f, hb_s = 40, hb_p = 4;
        float hb_x = (lw - (10 * (hb_s + hb_p))) / 2;
        float cd_now = state_->combat_timer;
        for (int s = 0; s < 10; s++) {
            float sx = hb_x + s * (hb_s + hb_p);
            ui.DrawRect(sx, hb_y, hb_s, hb_s, {30, 30, 50, 200});
            ui.DrawBorder(sx, hb_y, hb_s, hb_s, {100, 100, 150, 150});
            char buf[8]; snprintf(buf, 8, "%d", (s+1)%10);
            ui.DrawText(sx + 12, hb_y + 24, 0xffcccccc, "%s", buf);
            
            // Cooldown overlay (dim the slot during cooldown)
            if (s == 0 && cd_now > 0 && cd_now < 0.8f) {
                float pct = cd_now / 0.8f;
                ui.DrawRect(sx, hb_y, hb_s, hb_s * (1.0f - pct), {0, 0, 0, 160});
            }
        }
    }

    // === Quest Tracker (right side) with progress ===
    if (!state_->quest_list.empty()) {
        float qx = lw - 230, qy = 180;
        float qh = 24 + (int)state_->quest_list.size() * 42;
        ui.DrawRect(qx, qy, 220, qh, {10, 15, 30, 220});
        ui.DrawBorder(qx, qy, 220, qh, {60, 80, 120, 150});
        ui.DrawText(qx + 4, qy + 2, 0xffffcc88, "QUEST TRACKER");
        float ly = qy + 20;
        for (size_t i = 0; i < state_->quest_list.size() && i < 5; i++) {
            std::string q = state_->quest_list[i];
            ui.DrawText(qx + 4, ly, 0xffffffff, "%s", q.c_str());
            // Parse progress from format "QuestName (cur/total)"
            auto paren = q.find('(');
            auto slash = q.find('/');
            if (paren != std::string::npos && slash != std::string::npos) {
                int cur = std::stoi(q.substr(paren + 1, slash - paren - 1));
                int tot = std::stoi(q.substr(slash + 1, q.find(')', slash) - slash - 1));
                if (tot > 0) {
                    float pct = (float)cur / tot;
                    ui.DrawRect(qx + 4, ly + 16, 200 * pct, 8, {80, 200, 80, 200});
                    ui.DrawRect(qx + 4, ly + 16, 200, 8, {40, 40, 40, 180});
                    ui.DrawBorder(qx + 4, ly + 16, 200, 8, {100, 200, 100, 100});
                }
            }
            ly += 38;
        }
    }

    // === Monster overhead rendering ===
    for (auto& m : monsters_) {
        if (m.IsAlive()) m.RenderOverhead(ui);
    }

    // === ObjectBalloon ===
    ObjectBalloon::RenderAll(ui);

    // === Character Info Dialog ===
    if (state_->charinfo_open && char_dlg_.GetWindow()) {
        char_dlg_.UpdateFromState(state_);
        char_dlg_.GetWindow()->Render(ui);
    }

    // === Help/Controls Overlay ===
    if (state_->help_open) {
        ui.DrawRect(200, 50, 880, 620, {5, 5, 15, 240});
        ui.DrawBorder(200, 50, 880, 620, {100, 180, 255, 200});
        ui.DrawText(220, 60, 0xffffcc88, "CONTROLS & INFORMATION");
        const char* lines[] = {
            "WASD / Arrow Keys - Move character",
            "Mouse Click - Move to location",
            "Mouse Drag (Right) - Rotate camera",
            "Space - Spawn random monster",
            "",
            "I - Inventory          K - Skills",
            "J - Quests             C - Character Info",
            "P - Party              G - Guild",
            "F - Friends            N - NPC Dialog",
            "O - Options            B - Bank/Storage",
            "H - Farm               M - World Map",
            "U - Trade              V - Fishing",
            "T - Chat               F1 - Help",
            "1-0 - Hotbar slots     Enter - Send chat",
            "",
            "Blacksmith NPC: [N] to open",
            "1:Buy 2:Sell 3:Repair 4:Enchant 5:Compose 6:Leave",
            "",
            "TIP: Enchant items up to +15 at the Blacksmith!",
            "TIP: Repair weapons before they break!",
            "TIP: Plant seeds in your farm with [H] then [1-9]!",
        };
        float ly = 88;
        for (auto* line : lines) {
            ui.DrawText(220, ly, 0xffcccccc, "%s", line);
            ly += 18;
        }
        ui.DrawText(220, ly + 10, 0xff888888, "[F1] Close");
    }

    // === Farm Panel ===
    if (state_->farm_open) {
        float fx = 100, fy = 100, fw = 300, fh = 250;
        ui.DrawRect(fx, fy, fw, fh, {20, 40, 20, 230});
        ui.DrawBorder(fx, fy, fw, fh, {100, 200, 100, 200});
        ui.DrawText(fx + 6, fy + 4, 0xaaffaa, "FARM");
        auto seeds = farm_.GetAllSeeds();
        float ly = fy + 26;
        for (size_t i = 0; i < seeds.size(); i++) {
            ui.DrawText(fx + 8, ly, 0xffffffff, "[%zu] %s (%ds)", i+1, seeds[i].name.c_str(), seeds[i].growth_time);
            ly += 18;
        }
        ly += 8;
        ui.DrawText(fx + 8, ly, 0xffcccccc, "P: Plant  W: Water  H: Harvest");
        ly += 18;
        // Show plots
        for (int p = 0; p < 9; p++) {
            auto* plot = farm_.GetPlot(p);
            if (plot && plot->seed_id > 0) {
                float px = fx + 20 + (p % 3) * 90;
                float py = ly + (p / 3) * 40;
                uint32_t col = plot->harvested ? 0xff666666 : 0xff44aa44;
                ui.DrawRect(px, py, 80, 30, {(uint8_t)(col & 0xff), (uint8_t)((col>>8)&0xff), (uint8_t)((col>>16)&0xff), 200});
                ui.DrawBorder(px, py, 80, 30, {100, 200, 100, 150});
                ui.DrawText(px + 2, py + 2, 0xffffffff, "%s", plot->plant_name.c_str());
                char stg[32]; snprintf(stg, 32, "S%d/%d", plot->growth_stage, plot->max_stages);
                ui.DrawText(px + 2, py + 14, 0xffcccccc, "%s", stg);
                if (plot->watered) ui.DrawText(px + 55, py + 2, 0x4488ff, "~");
            }
        }
        ui.DrawText(fx + 4, fy + fh - 16, 0xff888888, "[H] Close");
    }

    // === Dialog Windows ===
    if (state_->inv_open && inv_dlg_.GetWindow()) {
        inv_dlg_.UpdateFromState(state_, ui);
        inv_dlg_.GetWindow()->Render(ui);
    }
    if (state_->skill_open && skill_dlg_.GetWindow()) {
        skill_dlg_.UpdateFromState(state_);
        skill_dlg_.GetWindow()->Render(ui);
    }
    if (state_->quest_open && quest_dlg_.GetWindow()) {
        quest_dlg_.GetWindow()->Render(ui);
    }
    if (state_->npc_open && npc_dlg_.GetWindow()) {
        npc_dlg_.GetWindow()->Render(ui);
    }
    if (state_->party_open && party_dlg_.GetWindow()) {
        party_dlg_.GetWindow()->Render(ui);
    }
    if (state_->guild_open && guild_dlg_.GetWindow()) {
        guild_dlg_.GetWindow()->Render(ui);
    }
    if (state_->friend_open && friend_dlg_.GetWindow()) {
        friend_dlg_.GetWindow()->Render(ui);
    }
    if (state_->options_open && options_dlg_.GetWindow()) {
        options_dlg_.GetWindow()->Render(ui);
    }
    if (state_->storage_open && storage_dlg_.GetWindow()) {
        storage_dlg_.UpdateFromState(state_);
        storage_dlg_.GetWindow()->Render(ui);
    }
    if (state_->fishing_open) {
        fishing_dlg_.Render(ui);
    }
    if (state_->trade_open && trade_dlg_.GetWindow()) {
        trade_dlg_.UpdateFromState(state_);
        trade_dlg_.GetWindow()->Render(ui);
    }
    if (state_->consignment_open && consignment_dlg_.GetWindow()) {
        consignment_dlg_.GetWindow()->Render(ui);
    }
    if (state_->family_open && family_dlg_.GetWindow()) {
        family_dlg_.GetWindow()->Render(ui);
    }
    if (state_->housing_open && housing_dlg_.GetWindow()) {
        housing_dlg_.GetWindow()->Render(ui);
    }
    if (state_->dungeon_open && dungeon_dlg_.GetWindow()) {
        dungeon_dlg_.GetWindow()->Render(ui);
    }
    if (state_->costume_open && costume_dlg_.GetWindow()) {
        costume_dlg_.GetWindow()->Render(ui);
    }
    if (state_->cashshop_open && cash_shop_dlg_.GetWindow()) {
        cash_shop_dlg_.GetWindow()->Render(ui);
    }
    if (state_->mail_open && mail_dlg_.GetWindow()) {
        mail_dlg_.GetWindow()->Render(ui);
    }
    if (state_->keybind_open && keybind_dlg_.GetWindow()) {
        keybind_dlg_.GetWindow()->Render(ui);
    }
    if (state_->chatroom_open && chatroom_dlg_.GetWindow()) {
        chatroom_dlg_.GetWindow()->Render(ui);
    }
    if (state_->pet_open) {
        pet_dlg_.GetWindow()->Render(ui);
    }
    if (state_->cooking_open && cooking_dlg_.GetWindow()) {
        cooking_dlg_.GetWindow()->Render(ui);
    }
    if (state_->mount_open && mount_dlg_.GetWindow()) {
        mount_dlg_.GetWindow()->Render(ui);
    }
    if (state_->tournament_open && tournament_dlg_.GetWindow()) {
        tournament_dlg_.GetWindow()->Render(ui);
    }
    if (state_->avatar_open && avatar_dlg_.GetWindow()) {
        avatar_dlg_.GetWindow()->Render(ui);
    }
    if (state_->upgrade_open && upgrade_dlg_.GetWindow()) {
        upgrade_dlg_.GetWindow()->Render(ui);
    }
    if (state_->macro_open && macro_dlg_.GetWindow()) {
        macro_dlg_.GetWindow()->Render(ui);
    }
    
    // Server announcement overlay
    if (ops_.HasActiveAnnouncement()) {
        std::string msg = ops_.GetCurrentAnnouncement();
        if (!msg.empty()) {
            float ax = lw / 2 - 200;
            ui.DrawRect(ax, 0, 400, 28, {0, 0, 0, 180});
            ui.DrawBorder(ax, 0, 400, 28, {200, 200, 100, 150});
            ui.DrawText(ax + 10, 5, 0xffffff88, "%s", msg.c_str());
        }
    }
    
    // Script-loaded windows (WindowManager)
    wm_.Render(ui);
}
