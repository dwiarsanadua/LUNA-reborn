#include "GameScreen.hpp"
#include <gameobjects/FarmSystem.hpp>
#include <network/NetworkClient.hpp>
#include <rendering/CharacterRenderer.hpp>
#include <ecs/systems/GameDataDB.hpp>
#include <config/ModelResolver.hpp>
#include <config/Paths.hpp>
#include <engine/gx_render/VFS.h>
#include <gameobjects/ClassAdvancement.hpp>
#include <gameobjects/DurabilitySystem.hpp>
#include <flatbuffers/flatbuffers.h>
#include <Login_generated.h>
#include <Character_generated.h>
#include <Inventory_generated.h>
#include <Chat_generated.h>
#include <Entity_generated.h>
#include <Movement_generated.h>
#include <Combat_generated.h>
#include <NPC_generated.h>
#include <CharLife_generated.h>
#include <MapChange_generated.h>
#include <Party_generated.h>
#include <Storage_generated.h>
#include <Friend_generated.h>
#include <Guild_generated.h>
#include <Trade_generated.h>
#include <Consignment_generated.h>
#include <StreetStall_generated.h>
#include <Quest_generated.h>
#include <Dungeon_generated.h>
#include <Trigger_generated.h>
#include <Family_generated.h>
#include <Pet_generated.h>
#include <Fishing_generated.h>
#include <Secondary_generated.h>
#include <Farm_generated.h>
#include <Vehicle_generated.h>
#include <PacketType_generated.h>
#include <algorithm>
#include <cmath>
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
    weather_dlg_.SetCurrentWeather(WeatherDialog::CLEAR);
    weather_dlg_.SetWeatherCallback([this](WeatherDialog::WeatherType type) {
        switch (type) {
            case WeatherDialog::CLEAR:      weather_.SetWeather(WeatherSystem::Clear); break;
            case WeatherDialog::RAIN:       weather_.SetWeather(WeatherSystem::Rain); break;
            case WeatherDialog::HEAVY_RAIN: weather_.SetWeather(WeatherSystem::HeavyRain); break;
            case WeatherDialog::SNOW:       weather_.SetWeather(WeatherSystem::Snow); break;
        }
        state_->chat_messages.push_back("Weather changed.");
    });
    farm_.Init();
    farm_.AllocatePlots(9);
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
    if (state_->offline_mode) {
        consignment_.ListItem(2, 101, 1, 200, 500);
        consignment_.ListItem(2, 102, 5, 50, 120);
        consignment_.ListItem(3, 201, 1, 1000, 2500);
        consignment_.ListItem(2, 103, 3, 150, 350);
        consignment_.ListItem(4, 301, 1, 5000, 12000);
    }
    if (ui_) sky_.SetSampler(ui_->GetSampler(), ui_->GetWhiteTexture());
    wm_.PreloadUI(VFS::Resolve("assets/interface/"));
    InitScriptDialogs();
    if (ui_) {
        legacy_hud_.SetHotbarHandler([this](int slot) { CastHotbarSkill(slot); });
        legacy_hud_.Init(ui_->logicalWidth, ui_->logicalHeight);
        hero_.SetRenderHud(!legacy_hud_.IsActive());
    }
    navmesh_.Init(256.0f, 2.0f);
    hero_.SetNavMesh(&navmesh_);
    hero_.SetPKManager(&pk_dlg_);
    pk_dlg_.SetModeChangeCallback([this](PKMode mode) {
        state_->pk_mode = (mode != PKMode::Peaceful);
    });
    InitializeWorld();
    if (!state_->offline_mode && network_ && network_->IsConnected()) {
        SetupQuestNetworkCallbacks();
        SetupDungeonNetworkCallbacks();
        SetupSecondaryNetworkCallbacks();
        RequestQuestList();
    }
}

void GameScreen::Exit() {
    sky_.Shutdown();
}

bool GameScreen::HandlePacket(uint16_t type, const std::vector<uint8_t>& payload) {
    switch (type) {
    case 0x0208:
        return true;
    case luna::protocol::PacketType_MP_CHAT_ALL_ACK:
    case luna::protocol::PacketType_MP_CHAT_PARTY_ACK:
    case luna::protocol::PacketType_MP_CHAT_GUILD_ACK:
    case luna::protocol::PacketType_MP_CHAT_WHISPER_ACK:
    case 0x0501: {
        auto msg = flatbuffers::GetRoot<luna::protocol::ChatMessage>(payload.data());
        std::string sender = msg->sender_name() ? msg->sender_name()->str() : "?";
        std::string text = msg->message() ? msg->message()->str() : "";
        std::string line;
        if (msg->channel() == luna::protocol::ChatChannel_Party)
            line = "[Party] " + sender + ": " + text;
        else if (msg->channel() == luna::protocol::ChatChannel_Guild)
            line = "[Guild] " + sender + ": " + text;
        else if (msg->channel() == luna::protocol::ChatChannel_Whisper)
            line = "[Whisper] " + text;
        else
            line = "[" + sender + "] " + text;
        state_->chat_messages.push_back(line);
        if (!legacy_hud_.IsActive()) chat_panel_.AddMessage(line);
        if (state_->chat_messages.size() > 50) state_->chat_messages.erase(state_->chat_messages.begin());
        return true;
    }
    case luna::protocol::PacketType_MP_PARTY_CREATE_ACK:
    case luna::protocol::PacketType_MP_PARTY_INVITE_ACK:
    case luna::protocol::PacketType_MP_PARTY_LEAVE_ACK:
    case luna::protocol::PacketType_MP_PARTY_INFO_UPDATE: {
        auto resp = flatbuffers::GetRoot<luna::protocol::PartyResponse>(payload.data());
        ApplyPartyResponse(resp);
        return true;
    }
    case luna::protocol::PacketType_MP_PARTY_CREATE_NACK:
    case luna::protocol::PacketType_MP_PARTY_INVITE_NACK:
        state_->chat_messages.push_back("Party operation failed");
        return true;
    case luna::protocol::PacketType_MP_STORAGE_LIST_ACK:
    case luna::protocol::PacketType_MP_STORAGE_DEPOSIT_ACK:
    case luna::protocol::PacketType_MP_STORAGE_WITHDRAW_ACK: {
        auto resp = flatbuffers::GetRoot<luna::protocol::StorageResponse>(payload.data());
        ApplyStorageResponse(resp);
        return true;
    }
    case luna::protocol::PacketType_MP_STORAGE_LIST_NACK:
        state_->chat_messages.push_back("Storage operation failed");
        return true;
    case luna::protocol::PacketType_MP_FRIEND_LIST_ACK:
    case luna::protocol::PacketType_MP_FRIEND_ADD_ACK:
    case luna::protocol::PacketType_MP_FRIEND_DEL_ACK: {
        auto resp = flatbuffers::GetRoot<luna::protocol::FriendResponse>(payload.data());
        ApplyFriendResponse(resp);
        return true;
    }
    case luna::protocol::PacketType_MP_FRIEND_LIST_NACK:
    case luna::protocol::PacketType_MP_FRIEND_ADD_NACK:
        state_->chat_messages.push_back("Friend operation failed");
        return true;
    case luna::protocol::PacketType_MP_GUILD_CREATE_ACK:
    case luna::protocol::PacketType_MP_GUILD_INFO:
    case luna::protocol::PacketType_MP_GUILD_ADDMEMBER_ACK:
    case luna::protocol::PacketType_MP_GUILD_SECEDE_ACK: {
        auto resp = flatbuffers::GetRoot<luna::protocol::GuildResponse>(payload.data());
        ApplyGuildResponse(resp);
        return true;
    }
    case luna::protocol::PacketType_MP_GUILD_CREATE_NACK:
    case luna::protocol::PacketType_MP_GUILD_ADDMEMBER_NACK:
        state_->chat_messages.push_back("Guild operation failed");
        return true;
    case luna::protocol::PacketType_MP_EXCHANGE_APPLY_NACK:
    case luna::protocol::PacketType_MP_EXCHANGE_APPLY_ACK:
    case luna::protocol::PacketType_MP_EXCHANGE_START:
    case luna::protocol::PacketType_MP_EXCHANGE_ADDITEM_ACK:
    case luna::protocol::PacketType_MP_EXCHANGE_SETGOLD_ACK:
    case luna::protocol::PacketType_MP_EXCHANGE_CONFIRM_ACK:
    case luna::protocol::PacketType_MP_EXCHANGE_STATE_UPDATE:
    case luna::protocol::PacketType_MP_EXCHANGE_COMPLETE:
    case luna::protocol::PacketType_MP_EXCHANGE_CANCEL_ACK: {
        auto resp = flatbuffers::GetRoot<luna::protocol::TradeResponse>(payload.data());
        ApplyTradeResponse(resp);
        return true;
    }
    case luna::protocol::PacketType_MP_CONSIGNMENT_GETLIST_ACK:
    case luna::protocol::PacketType_MP_CONSIGNMENT_SEARCH_ACK:
    case luna::protocol::PacketType_MP_CONSIGNMENT_REGIST_ACK:
    case luna::protocol::PacketType_MP_CONSIGNMENT_UPDATE_ACK:
    case luna::protocol::PacketType_MP_CONSIGNMENT_CANCEL_ACK: {
        auto resp = flatbuffers::GetRoot<luna::protocol::ConsignmentResponse>(payload.data());
        ApplyConsignmentResponse(resp, pending_consignment_mine_, pending_consignment_bids_);
        if (type == luna::protocol::PacketType_MP_CONSIGNMENT_REGIST_ACK ||
            type == luna::protocol::PacketType_MP_CONSIGNMENT_UPDATE_ACK) {
            RequestConsignmentSearch("");
        }
        pending_consignment_mine_ = false;
        pending_consignment_bids_ = false;
        return true;
    }
    case luna::protocol::PacketType_MP_CONSIGNMENT_GETLIST_NACK:
    case luna::protocol::PacketType_MP_CONSIGNMENT_SEARCH_NACK:
    case luna::protocol::PacketType_MP_CONSIGNMENT_REGIST_NACK:
    case luna::protocol::PacketType_MP_CONSIGNMENT_UPDATE_NACK:
        state_->chat_messages.push_back("Consignment operation failed");
        return true;
    case luna::protocol::PacketType_MP_STREETSTALL_OPEN_ACK:
    case luna::protocol::PacketType_MP_STREETSTALL_ADDITEM_ACK:
    case luna::protocol::PacketType_MP_STREETSTALL_BUY_ACK:
    case luna::protocol::PacketType_MP_STREETSTALL_CLOSE_ACK:
    case luna::protocol::PacketType_MP_STREETSTALL_LIST_ACK: {
        auto resp = flatbuffers::GetRoot<luna::protocol::StreetStallResponse>(payload.data());
        ApplyStreetStallResponse(resp);
        return true;
    }
    case luna::protocol::PacketType_MP_CHAR_LIFE_ACK: {
        auto life = flatbuffers::GetRoot<luna::protocol::CharLifeUpdate>(payload.data());
        uint32_t player_id = 0;
        if (!state_->characters.empty() && state_->selected_char < (int)state_->characters.size())
            player_id = state_->characters[state_->selected_char].id;
        if (life->entity_id() == player_id || life->entity_id() == 0) {
            int prev_hp = state_->hp;
            hero_.ApplyServerStats(life->hp(), life->max_hp(), life->mp(), life->max_mp(),
                static_cast<int>(life->gold()), life->exp());
            state_->battle_delay_timer = 10.0f;
            if (life->hp() < prev_hp && life->hp() > 0) {
                effect_mgr_.SpawnDamageNumber(hero_.GetX(), hero_.GetY() + 1.5f, hero_.GetZ(),
                    prev_hp - life->hp(), DamageType::Normal);
            }
        }
        return true;
    }
    case luna::protocol::PacketType_MP_ITEM_STORAGEITEM_INFO:
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
        hero_.ApplyServerStats(state_->hp, state_->max_hp, state_->mp, state_->max_mp, state_->gold);
        return true;
    }
    case luna::protocol::PacketType_MP_INVENTORY_UPDATE: {
        auto upd = flatbuffers::GetRoot<luna::protocol::InventoryUpdate>(payload.data());
        bool found = false;
        for (auto& item : state_->inventory) {
            if (item.slot == upd->slot_index()) {
                item.id = upd->item_id();
                item.count = upd->count();
                found = true;
                break;
            }
        }
        if (!found) {
            InvItem item;
            item.slot = upd->slot_index();
            item.id = upd->item_id();
            item.count = upd->count();
            char buf[64];
            snprintf(buf, sizeof(buf), "Item_%u", item.id);
            item.name = buf;
            state_->inventory.push_back(item);
        }
        state_->chat_messages.push_back("Loot received!");
        if (state_->chat_messages.size() > 50) state_->chat_messages.erase(state_->chat_messages.begin());
        return true;
    }
    case luna::protocol::PacketType_MP_ENTITY_DESPAWN: {
        auto despawn = flatbuffers::GetRoot<luna::protocol::EntityDespawn>(payload.data());
        RemoveNetworkEntity(despawn->entity_id());
        return true;
    }
    case luna::protocol::PacketType_MP_COMBAT_ATTACK_ACK:
    case luna::protocol::PacketType_MP_SKILL_CAST_ACK: {
        auto result = flatbuffers::GetRoot<luna::protocol::AttackResult>(payload.data());
        ApplyNetworkAttackResult(result);
        return true;
    }
    case luna::protocol::PacketType_MP_ENTITY_SPAWN: {
        auto spawn = flatbuffers::GetRoot<luna::protocol::EntitySpawn>(payload.data());
        for (const auto& existing : state_->entities) {
            if (existing.id == spawn->entity_id()) return true;
        }
        RemoteEntity e;
        e.id = spawn->entity_id();
        e.name = spawn->name() ? spawn->name()->str() : "?";
        e.level = spawn->level(); e.hp_pct = (int)(spawn->hp_percent() * 100);
        if (spawn->position()) { e.x = spawn->position()->x(); e.y = spawn->position()->y(); e.z = spawn->position()->z(); }
        state_->entities.push_back(e);
        
        std::string modelPath = spawn->model_id() ? spawn->model_id()->str() : "m224.chx";
        modelPath = ModelResolver::ResolveMonsterModel(modelPath);
        if (modelPath.empty())
            modelPath = VFS::Find("assets/models/monster/monster_placeholder.glb");
        
        uint32_t colors[] = {0xff44cc44, 0xffcc4444, 0xffcccc44, 0xff44cccc, 0xffcc44cc};
        CharRenderer_Spawn(e.id, modelPath, e.x, e.y, e.z, colors[state_->next_entity_id++ % 5]);
        return true;
    }
    case luna::protocol::PacketType_MP_USERCONN_CHANGEMAP_ACK:
    case luna::protocol::PacketType_MP_USERCONN_CHANGEMAP_NACK: {
        auto resp = flatbuffers::GetRoot<luna::protocol::ChangeMapResponse>(payload.data());
        OnChangeMapAck(resp);
        return true;
    }
    case luna::protocol::PacketType_MP_ENTITY_TRANSFORM: {
        auto trans = flatbuffers::GetRoot<luna::protocol::EntityTransform>(payload.data());
        uint32_t eid = trans->entity_id();
        if (!state_->offline_mode && trans->position()) {
            uint32_t player_id = 0;
            if (!state_->characters.empty() && state_->selected_char < (int)state_->characters.size())
                player_id = state_->characters[state_->selected_char].id;
            if (eid == player_id || eid == 0) {
                hero_.SetPosition(trans->position()->x(), trans->position()->y(), trans->position()->z());
                state_->player_x = hero_.GetX();
                state_->player_y = hero_.GetY();
                state_->player_z = hero_.GetZ();
                return true;
            }
        }
        for (auto& e : state_->entities) {
            if (e.id == eid) {
                if (trans->position())
                    BeginEntityInterpolation(eid, trans->position()->x(),
                        trans->position()->y(), trans->position()->z());
                break;
            }
        }
        return true;
    }
    case luna::protocol::PacketType_MP_QUEST_LIST_ACK: {
        auto resp = flatbuffers::GetRoot<luna::protocol::QuestListResponse>(payload.data());
        ApplyQuestListResponse(resp);
        return true;
    }
    case luna::protocol::PacketType_MP_QUEST_START_ACK:
    case luna::protocol::PacketType_MP_QUEST_START_NACK: {
        auto resp = flatbuffers::GetRoot<luna::protocol::QuestStartResponse>(payload.data());
        ApplyQuestStartResponse(resp);
        return true;
    }
    case luna::protocol::PacketType_MP_QUEST_END_ACK:
    case luna::protocol::PacketType_MP_QUEST_END_NACK: {
        auto resp = flatbuffers::GetRoot<luna::protocol::QuestCompleteResponse>(payload.data());
        ApplyQuestCompleteResponse(resp);
        return true;
    }
    case luna::protocol::PacketType_MP_QUEST_UPDATE_NOTIFY: {
        auto resp = flatbuffers::GetRoot<luna::protocol::QuestUpdateNotify>(payload.data());
        ApplyQuestUpdateNotify(resp);
        return true;
    }
    case luna::protocol::PacketType_MP_DUNGEON_ENTRANCE_ACK:
    case luna::protocol::PacketType_MP_DUNGEON_ENTRANCE_NACK: {
        auto resp = flatbuffers::GetRoot<luna::protocol::DungeonEntranceResponse>(payload.data());
        ApplyDungeonEntranceResponse(resp);
        return true;
    }
    case luna::protocol::PacketType_MP_DUNGEON_INFO_ACK:
    case luna::protocol::PacketType_MP_DUNGEON_INFO_NACK: {
        auto resp = flatbuffers::GetRoot<luna::protocol::DungeonInfoResponse>(payload.data());
        ApplyDungeonInfoResponse(resp);
        return true;
    }
    case luna::protocol::PacketType_MP_TRIGGER_NOTIFY: {
        auto resp = flatbuffers::GetRoot<luna::protocol::TriggerNotify>(payload.data());
        ApplyTriggerNotify(resp);
        return true;
    }
    case luna::protocol::PacketType_MP_FAMILY_INFO_ACK:
    case luna::protocol::PacketType_MP_FAMILY_CREATE_ACK:
    case luna::protocol::PacketType_MP_FAMILY_CREATE_NACK:
    case luna::protocol::PacketType_MP_FAMILY_PROPOSE_ACK:
    case luna::protocol::PacketType_MP_FAMILY_PROPOSE_NACK:
    case luna::protocol::PacketType_MP_FAMILY_ACCEPT_ACK:
    case luna::protocol::PacketType_MP_FAMILY_ACTION_ACK:
    case luna::protocol::PacketType_MP_FAMILY_ACTION_NACK: {
        auto resp = flatbuffers::GetRoot<luna::protocol::FamilyResponse>(payload.data());
        ApplyFamilyResponse(resp);
        return true;
    }
    case luna::protocol::PacketType_MP_PET_INFO_ACK:
    case luna::protocol::PacketType_MP_PET_ACTION_ACK:
    case luna::protocol::PacketType_MP_PET_ACTION_NACK: {
        auto resp = flatbuffers::GetRoot<luna::protocol::PetResponse>(payload.data());
        ApplyPetResponse(resp);
        return true;
    }
    case luna::protocol::PacketType_MP_FISHING_CAST_ACK:
    case luna::protocol::PacketType_MP_FISHING_CAST_NACK: {
        auto resp = flatbuffers::GetRoot<luna::protocol::FishingCastResponse>(payload.data());
        ApplyFishingResponse(resp);
        return true;
    }
    case luna::protocol::PacketType_MP_SIEGE_INFO_ACK: {
        auto resp = flatbuffers::GetRoot<luna::protocol::SiegeInfoResponse>(payload.data());
        ApplySiegeInfoResponse(resp);
        return true;
    }
    case luna::protocol::PacketType_MP_SIEGE_ACTION_ACK:
    case luna::protocol::PacketType_MP_SIEGE_ACTION_NACK: {
        auto resp = flatbuffers::GetRoot<luna::protocol::SiegeActionResponse>(payload.data());
        ApplySiegeActionResponse(resp);
        return true;
    }
    case luna::protocol::PacketType_MP_TOURNAMENT_LIST_ACK: {
        auto resp = flatbuffers::GetRoot<luna::protocol::TournamentListResponse>(payload.data());
        ApplyTournamentListResponse(resp);
        return true;
    }
    case luna::protocol::PacketType_MP_TOURNAMENT_REGISTER_ACK: {
        auto resp = flatbuffers::GetRoot<luna::protocol::TournamentRegisterResponse>(payload.data());
        ApplyTournamentRegisterResponse(resp);
        return true;
    }
    case luna::protocol::PacketType_MP_TOURNAMENT_ACTION_ACK:
    case luna::protocol::PacketType_MP_TOURNAMENT_ACTION_NACK: {
        auto resp = flatbuffers::GetRoot<luna::protocol::TournamentActionResponse>(payload.data());
        ApplyTournamentActionResponse(resp);
        return true;
    }
    case luna::protocol::PacketType_MP_HOUSING_INFO_ACK: {
        auto resp = flatbuffers::GetRoot<luna::protocol::HousingInfoResponse>(payload.data());
        ApplyHousingInfoResponse(resp);
        return true;
    }
    case luna::protocol::PacketType_MP_HOUSING_ACTION_ACK:
    case luna::protocol::PacketType_MP_HOUSING_ACTION_NACK: {
        auto resp = flatbuffers::GetRoot<luna::protocol::HousingActionResponse>(payload.data());
        ApplyHousingActionResponse(resp);
        return true;
    }
    case luna::protocol::PacketType_MP_CASHSHOP_LIST_ACK: {
        auto resp = flatbuffers::GetRoot<luna::protocol::CashShopListResponse>(payload.data());
        ApplyCashShopListResponse(resp);
        return true;
    }
    case luna::protocol::PacketType_MP_CASHSHOP_BUY_ACK: {
        auto resp = flatbuffers::GetRoot<luna::protocol::CashShopBuyResponse>(payload.data());
        ApplyCashShopBuyResponse(resp);
        return true;
    }
    case luna::protocol::PacketType_MP_FARM_INFO_ACK:
    case luna::protocol::PacketType_MP_FARM_ACTION_ACK:
    case luna::protocol::PacketType_MP_FARM_ACTION_NACK: {
        auto resp = flatbuffers::GetRoot<luna::protocol::FarmResponse>(payload.data());
        ApplyFarmResponse(resp);
        return true;
    }
    // Vehicle packets
    case luna::protocol::PacketType_MP_VEHICLE_SUMMON_ACK: {
        auto resp = flatbuffers::GetRoot<luna::protocol::VehicleSummonResponse>(payload.data());
        state_->chat_messages.push_back("Vehicle summoned!");
        return true;
    }
    case luna::protocol::PacketType_MP_VEHICLE_SUMMON_NACK:
    case luna::protocol::PacketType_MP_VEHICLE_UNSUMMON_NACK:
    case luna::protocol::PacketType_MP_VEHICLE_MOUNT_REQUEST_NACK:
    case luna::protocol::PacketType_MP_VEHICLE_MOUNT_REJECT_ACK: {
        auto err = flatbuffers::GetRoot<luna::protocol::VehicleErrorResponse>(payload.data());
        (void)err;
        state_->chat_messages.push_back("Vehicle error occurred");
        return true;
    }
    case luna::protocol::PacketType_MP_VEHICLE_UNSUMMON_ACK: {
        auto resp = flatbuffers::GetRoot<luna::protocol::VehicleUnsummonResponse>(payload.data());
        (void)resp;
        state_->chat_messages.push_back("Vehicle dismissed");
        return true;
    }
    case luna::protocol::PacketType_MP_VEHICLE_MOUNT_REQUEST_SYN: {
        auto req = flatbuffers::GetRoot<luna::protocol::VehicleMountAskRequest>(payload.data());
        (void)req;
        state_->chat_messages.push_back("Someone wants to ride your vehicle!");
        return true;
    }
    case luna::protocol::PacketType_MP_VEHICLE_MOUNT_ALLOW_ACK: {
        auto resp = flatbuffers::GetRoot<luna::protocol::VehicleMountAllowResponse>(payload.data());
        (void)resp;
        state_->chat_messages.push_back("You mounted the vehicle!");
        return true;
    }
    case luna::protocol::PacketType_MP_VEHICLE_DISMOUNT_ACK: {
        auto resp = flatbuffers::GetRoot<luna::protocol::VehicleDismountResponse>(payload.data());
        (void)resp;
        state_->chat_messages.push_back("You dismounted the vehicle");
        return true;
    }
    case luna::protocol::PacketType_MP_VEHICLE_GET_OPTION_ACK: {
        auto resp = flatbuffers::GetRoot<luna::protocol::VehicleGetOptionResponse>(payload.data());
        (void)resp;
        state_->chat_messages.push_back("Vehicle toll info received");
        return true;
    }
    case luna::protocol::PacketType_MP_VEHICLE_PASSENGER_ACK: {
        auto resp = flatbuffers::GetRoot<luna::protocol::VehiclePassengerInfo>(payload.data());
        (void)resp;
        state_->chat_messages.push_back("Passenger info received");
        return true;
    }
    case luna::protocol::PacketType_MP_NPC_SPEECH_ACK: {
        auto resp = flatbuffers::GetRoot<luna::protocol::NpcResponse>(payload.data());
        std::string dialog = resp->dialog_text() ? resp->dialog_text()->str() : "\"Hello.\"";
        state_->npc_text = dialog;
        state_->npc_id = static_cast<int>(resp->npc_id());
        npc_dlg_.Open(state_, state_->npc_id, "NPC");
        return true;
    }
    case luna::protocol::PacketType_MP_NPC_SPEECH_NACK: {
        state_->chat_messages.push_back("NPC interaction failed");
        return true;
    }
    default: return false;
    }
}

void GameScreen::ToggleDialog(const std::string& name) {
    dialog_visibility_[name] = !dialog_visibility_[name];
    bool open = dialog_visibility_[name];

    if (name == "inventory") {
        state_->inv_open = open;
        if (open) inv_dlg_.Open(state_, &wm_);
    } else if (name == "skill") {
        state_->skill_open = open;
        if (open) skill_dlg_.Open(state_, &wm_);
    } else if (name == "quest") {
        state_->quest_open = open;
        if (open) {
            SetupQuestNetworkCallbacks();
            quest_dlg_.Open(state_, &wm_);
            if (!state_->offline_mode && network_ && network_->IsConnected())
                RequestQuestList();
        }
    } else if (name == "character") {
        state_->charinfo_open = open;
        if (open) char_dlg_.Open(state_, &wm_);
    } else if (name == "minimap") {
        if (open) minimap_dlg_.Open(&wm_);
        else minimap_dlg_.Close();
    } else if (name == "party") {
        state_->party_open = open;
        if (open) {
            party_dlg_.Open(&wm_);
            if (!state_->offline_mode && network_ && network_->IsConnected() && state_->party_id == 0)
                SendPartyCreate();
        }
    } else if (name == "guild") {
        state_->guild_open = open;
        if (open) {
            guild_dlg_.Open(&wm_, &siege_);
            if (!state_->offline_mode && network_ && network_->IsConnected()) {
                RequestGuildInfo();
                RequestSiegeInfo();
            }
        } else {
            siege_poll_timer_ = 0.0f;
        }
    } else if (name == "pet") {
        state_->pet_open = open;
        if (open) {
            if (state_->offline_mode)
                pet_.Init("Fluffy", 1);
            pet_dlg_.Open(state_, &wm_, &pet_);
            if (!state_->offline_mode && network_ && network_->IsConnected())
                RequestPetInfo();
        } else {
            pet_poll_timer_ = 0.0f;
        }
    } else if (name == "mount") {
        state_->mount_open = open;
        if (open) mount_dlg_.Open(state_, &wm_);
    } else if (name == "weather") {
        if (open) weather_dlg_.Open(&wm_);
        else weather_dlg_.Close();
    }

    if (open && audio_)
        audio_->PlaySFXByCategory(AudioManager::SFX_UI, "window_open1.wav");
}

bool GameScreen::IsDialogOpen(const std::string& name) const {
    auto it = dialog_visibility_.find(name);
    return it != dialog_visibility_.end() && it->second;
}

bool GameScreen::HandleKey(int key, int scancode, int action, int mods) {
    (void)scancode; (void)mods;
    // Accept PRESS and REPEAT for movement keys so holding WASD keeps moving;
    // everything else fires on PRESS only.
    bool is_move_key = key == 87 || key == 83 || key == 65 || key == 68 ||
                       (key >= 262 && key <= 265);
    if (action != 1 && !(action == 2 && is_move_key)) return true;

    // Dialog hotkeys (Old-style mapping: I K Q C M P G T R)
    switch (key) {
        case 73:  ToggleDialog("inventory"); return true;
        case 75:  ToggleDialog("skill"); return true;
        case 81:  ToggleDialog("quest"); return true;
        case 67:  ToggleDialog("character"); return true;
        case 77:  ToggleDialog("minimap"); return true;
        case 80:  ToggleDialog("party"); return true;
        case 71:  ToggleDialog("guild"); return true;
        case 84:  ToggleDialog("pet"); return true;
        case 82:  ToggleDialog("mount"); return true;
        case 293: ToggleDialog("weather"); return true; // F4 (W is movement)
        default: break;
    }

    float speed = 0.5f;
    if (key == 87 || key == 265) { hero_.ClearWaypoint(); state_->player_z -= speed; hero_.Move(0, -speed); }
    else if (key == 83 || key == 264) { hero_.ClearWaypoint(); state_->player_z += speed; hero_.Move(0, speed); }
    else if (key == 65 || key == 263) { hero_.ClearWaypoint(); state_->player_x -= speed; hero_.Move(-speed, 0); }
    else if (key == 68 || key == 262) { hero_.ClearWaypoint(); state_->player_x += speed; hero_.Move(speed, 0); }
    else if (key == 32) {
        if (state_->fishing_open && !state_->offline_mode && network_ && network_->IsConnected())
            SendFishingCast(0);
        else
            SpawnRandomMonster();
    }
    else if (key >= 49 && key <= 57) CastHotbarSkill(key - 49);
    else if (key == 48) CastHotbarSkill(9);
    else if (key == 80) {
        if (pk_dlg_.IsOpen()) pk_dlg_.Close();
        else pk_dlg_.Open(&wm_);
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
        CharRenderer_Spawn(id, "assets/models/monster_placeholder.glb", x, 0, z, 0xffff4444);
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
        if (state_->quest_open) {
            SetupQuestNetworkCallbacks();
            quest_dlg_.Open(state_, &wm_);
            if (!state_->offline_mode && network_ && network_->IsConnected())
                RequestQuestList();
        }
        if (audio_) audio_->PlaySFXByCategory(AudioManager::SFX_UI, "window_open1.wav");
    }
    else if (key == 67) { // C key - Character Dialog
        state_->charinfo_open = !state_->charinfo_open;
        if (state_->charinfo_open) char_dlg_.Open(state_, &wm_);
        if (audio_) audio_->PlaySFXByCategory(AudioManager::SFX_UI, "window_open1.wav");
    }
    else if (key == 80) { // P key - Party
        state_->party_open = !state_->party_open;
        if (state_->party_open) {
            party_dlg_.Open(&wm_);
            if (!state_->offline_mode && network_ && network_->IsConnected() && state_->party_id == 0)
                SendPartyCreate();
        }
        if (audio_) audio_->PlaySFXByCategory(AudioManager::SFX_UI, "window_open1.wav");
    }
    else if (key == 71) { // G key - Guild
        state_->guild_open = !state_->guild_open;
        if (state_->guild_open) {
            guild_dlg_.Open(&wm_, &siege_);
            if (!state_->offline_mode && network_ && network_->IsConnected()) {
                RequestGuildInfo();
                RequestSiegeInfo();
            }
        } else {
            siege_poll_timer_ = 0.0f;
        }
        if (audio_) audio_->PlaySFXByCategory(AudioManager::SFX_UI, "window_open1.wav");
    }
    else if (key == 70) { // F key - Friends
        state_->friend_open = !state_->friend_open;
        if (state_->friend_open) {
            friend_dlg_.Open(&wm_);
            if (!state_->offline_mode && network_ && network_->IsConnected())
                RequestFriendList();
        }
        if (audio_) audio_->PlaySFXByCategory(AudioManager::SFX_UI, "window_open1.wav");
    }
    else if (key == 72) { // H key - Farm / Harvest
        state_->farm_open = !state_->farm_open;
        if (state_->farm_open) {
            farm_.AllocatePlots(9);
            if (!state_->offline_mode && network_ && network_->IsConnected())
                RequestFarmInfo();

            auto on_plant = [this](uint8_t plot_id, uint32_t seed_id) {
                if (!state_->offline_mode && network_ && network_->IsConnected())
                    SendFarmAction(1, plot_id, seed_id);
                else
                    farm_.Plant(plot_id, static_cast<int>(seed_id));
            };
            auto on_water = [this](uint8_t plot_id) {
                if (!state_->offline_mode && network_ && network_->IsConnected())
                    SendFarmAction(2, plot_id, 0);
                else
                    farm_.Water(plot_id);
            };
            auto on_harvest = [this](uint8_t plot_id) {
                if (!state_->offline_mode && network_ && network_->IsConnected())
                    SendFarmAction(3, plot_id, 0);
                else {
                    int item_id = 0, count = 0;
                    if (farm_.Harvest(plot_id, item_id, count)) {
                        if (state_) {
                            char buf[128];
                            snprintf(buf, sizeof(buf), "Harvested: +%d item_%d", count, item_id);
                            state_->chat_messages.push_back(buf);
                        }
                    }
                }
            };
            farm_get_dlg_.Open(state_, &wm_, on_plant, on_water, on_harvest);
        } else {
            farm_get_dlg_.Close();
            farm_poll_timer_ = 0.0f;
        }
        if (audio_) audio_->PlaySFXByCategory(AudioManager::SFX_UI, "window_open1.wav");
    }
    else if (key == 79) { // O key - Options
        state_->options_open = !state_->options_open;
        if (state_->options_open) options_dlg_.Open(&wm_);
        if (audio_) audio_->PlaySFXByCategory(AudioManager::SFX_UI, "window_open1.wav");
    }
    else if (key == 66) { // B key - Bank/Storage
        state_->storage_open = !state_->storage_open;
        if (state_->storage_open) {
            storage_dlg_.Open(state_, &wm_);
            if (!state_->offline_mode && network_ && network_->IsConnected())
                RequestStorageList();
        }
        if (audio_) audio_->PlaySFXByCategory(AudioManager::SFX_UI, "window_open1.wav");
    }
    else if (key == 112) {
        state_->help_open = !state_->help_open;
        if (state_->help_open) helper_dlg_.Open(&wm_);
        else helper_dlg_.Close();
        if (audio_) audio_->PlaySFXByCategory(AudioManager::SFX_UI, "window_open1.wav");
    }
    else if (key == 77) {
        state_->worldmap_open = !state_->worldmap_open;
        if (state_->worldmap_open) worldmap_dlg_.Open(&wm_);
        else worldmap_dlg_.Close();
        if (audio_) audio_->PlaySFXByCategory(AudioManager::SFX_UI, "window_open1.wav");
    }
    else if (key == 85) { // U key - Trade
        state_->trade_open = !state_->trade_open;
        if (state_->trade_open) {
            SetupTradeNetworkCallbacks();
            trade_dlg_.Open(state_, &trading_);
            if (!state_->offline_mode && network_ && network_->IsConnected())
                SendTradeApply("NPC_Trader");
        } else {
            if (state_->trade_active && !state_->offline_mode)
                SendTradeCancel();
            state_->trade_active = false;
        }
        if (audio_) audio_->PlaySFXByCategory(AudioManager::SFX_UI, "window_open1.wav");
    }
    else if (key == 86) { // V key - Fishing
        state_->fishing_open = !state_->fishing_open;
        if (state_->fishing_open) {
            fishing_dlg_.Open(state_, &wm_);
            if (!state_->offline_mode && network_ && network_->IsConnected()) {
                fishing_dlg_.SetCastCallback([this]() { SendFishingCast(0); });
            } else {
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
        }
        if (audio_) audio_->PlaySFXByCategory(AudioManager::SFX_UI, "window_open1.wav");
    }
    else if (key == 89) { // Y key - Auction/Consignment
        state_->consignment_open = !state_->consignment_open;
        if (state_->consignment_open) {
            SetupConsignmentNetworkCallbacks();
            consignment_dlg_.Open(state_, &wm_, &consignment_);
            if (!state_->offline_mode && network_ && network_->IsConnected()) {
                RequestConsignmentSearch("");
                RequestStreetStallList();
            }
        }
        if (audio_) audio_->PlaySFXByCategory(AudioManager::SFX_UI, "window_open1.wav");
    }
    else if (key == 69) { // E key - Mail
        state_->mail_open = !state_->mail_open;
        if (state_->mail_open) mail_dlg_.Open(state_, &wm_);
        if (audio_) audio_->PlaySFXByCategory(AudioManager::SFX_UI, "window_open1.wav");
    }
    else if (key == 81) { // Q key - Cash Shop
        state_->cashshop_open = !state_->cashshop_open;
        if (state_->cashshop_open) {
            cash_shop_dlg_.Open(state_, &wm_, &cash_shop_);
            cashshop_poll_timer_ = 0.0f;
            if (!state_->offline_mode && network_ && network_->IsConnected())
                RequestCashShopList();
        }
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
            if (state_->offline_mode)
                pet_.Init("Fluffy", 1);
            pet_dlg_.Open(state_, &wm_, &pet_);
            if (!state_->offline_mode && network_ && network_->IsConnected())
                RequestPetInfo();
        } else {
            pet_poll_timer_ = 0.0f;
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
        if (state_->tournament_open) {
            tournament_dlg_.Open(state_, &wm_, &tournament_sys_);
            if (!state_->offline_mode && network_ && network_->IsConnected())
                RequestTournamentList();
        } else {
            tournament_poll_timer_ = 0.0f;
        }
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
        if (state_->dungeon_open) {
            SetupDungeonNetworkCallbacks();
            dungeon_dlg_.Open(state_, &wm_, &dungeon_sys_);
        }
        if (audio_) audio_->PlaySFXByCategory(AudioManager::SFX_UI, "window_open1.wav");
    }
    else if (key == 82) { // R key - Housing
        state_->housing_open = !state_->housing_open;
        if (state_->housing_open) {
            housing_dlg_.Open(state_, &wm_);
            housing_poll_timer_ = 0.0f;
            if (!state_->offline_mode && network_ && network_->IsConnected())
                RequestHousingInfo();
        }
        if (audio_) audio_->PlaySFXByCategory(AudioManager::SFX_UI, "window_open1.wav");
    }
    else if (key == 76) { // L key - Family & Couple
        state_->family_open = !state_->family_open;
        if (state_->family_open) {
            family_dlg_.Open(state_, &wm_, &family_);
            if (!state_->offline_mode && network_ && network_->IsConnected())
                RequestFamilyInfo();
        } else {
            family_poll_timer_ = 0.0f;
        }
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
    else if (key == 84) {
        if (legacy_hud_.IsActive()) legacy_hud_.ToggleChat();
        else chat_panel_.Toggle();
    }
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
                        if (map_id > 0) ChangeMap((uint32_t)map_id);
                    } else {
                        state_->chat_messages.push_back("Usage: /teleport [map_id] — Available: 13,14,15,19,20,21,22,23,25,26,27,28,31,32,41,51,55,56,60,63,64,71,74,75,96");
                    }
                } else if (verb == "maps" || verb == "maplist") {
                    state_->chat_messages.push_back("Available maps: 13,14,15,19,20,21,22,23,25,26,27,28,31,32,41,51,55,56,60,63,64,71,74,75,96");
                } else if (verb == "party") {
                    if (arg.rfind("invite ", 0) == 0) {
                        SendPartyInvite(arg.substr(7));
                    } else if (arg == "leave" || arg == "quit") {
                        SendPartyLeave();
                    } else if (arg == "create") {
                        SendPartyCreate();
                    } else {
                        state_->chat_messages.push_back("Party: /party create | invite [name] | leave");
                    }
                } else if (verb == "p") {
                    SendPartyInvite(arg);
                } else if (verb == "deposit" || verb == "dep") {
                    int slot = arg.empty() ? 0 : std::atoi(arg.c_str());
                    SendStorageDeposit(static_cast<uint8_t>(slot), 1);
                } else if (verb == "withdraw" || verb == "wd") {
                    int slot = arg.empty() ? 0 : std::atoi(arg.c_str());
                    SendStorageWithdraw(static_cast<uint8_t>(slot), 1);
                } else if (verb == "w" || verb == "whisper") {
                    size_t sp = arg.find(' ');
                    if (sp == std::string::npos || arg.empty()) {
                        state_->chat_messages.push_back("Usage: /w [name] [message]");
                    } else if (network_ && network_->IsConnected()) {
                        std::string target = arg.substr(0, sp);
                        std::string text = arg.substr(sp + 1);
                        flatbuffers::FlatBufferBuilder fbb;
                        auto chatMsg = luna::protocol::CreateChatMessageDirect(
                            fbb, GetSelectedCharId(), target.c_str(), text.c_str(),
                            luna::protocol::ChatChannel_Whisper, 0);
                        fbb.Finish(chatMsg);
                        network_->SendPacket(luna::protocol::PacketType_MP_CHAT_WHISPER_SYN,
                            fbb.GetBufferPointer(), fbb.GetSize());
                    }
                } else if (verb == "pchat" && network_ && network_->IsConnected()) {
                    flatbuffers::FlatBufferBuilder fbb;
                    auto chatMsg = luna::protocol::CreateChatMessageDirect(
                        fbb, GetSelectedCharId(), state_->name.c_str(), arg.c_str(),
                        luna::protocol::ChatChannel_Party, 0);
                    fbb.Finish(chatMsg);
                    network_->SendPacket(luna::protocol::PacketType_MP_CHAT_PARTY_SYN,
                        fbb.GetBufferPointer(), fbb.GetSize());
                } else if (verb == "gchat" && network_ && network_->IsConnected()) {
                    flatbuffers::FlatBufferBuilder fbb;
                    auto chatMsg = luna::protocol::CreateChatMessageDirect(
                        fbb, GetSelectedCharId(), state_->name.c_str(), arg.c_str(),
                        luna::protocol::ChatChannel_Guild, 0);
                    fbb.Finish(chatMsg);
                    network_->SendPacket(luna::protocol::PacketType_MP_CHAT_GUILD_SYN,
                        fbb.GetBufferPointer(), fbb.GetSize());
                } else if (verb == "friend") {
                    if (arg.rfind("add ", 0) == 0) {
                        SendFriendAdd(arg.substr(4));
                    } else if (arg.rfind("del ", 0) == 0) {
                        SendFriendDelete(static_cast<uint32_t>(std::atoi(arg.substr(4).c_str())));
                    } else if (arg == "list") {
                        RequestFriendList();
                    } else {
                        state_->chat_messages.push_back("Friend: /friend add [name] | del [id] | list");
                    }
                } else if (verb == "guild") {
                    if (arg.rfind("create ", 0) == 0) {
                        SendGuildCreate(arg.substr(7));
                    } else if (arg.rfind("invite ", 0) == 0) {
                        SendGuildInvite(arg.substr(7));
                    } else if (arg == "leave" || arg == "quit") {
                        SendGuildLeave();
                    } else if (arg == "info") {
                        RequestGuildInfo();
                    } else {
                        state_->chat_messages.push_back("Guild: /guild create [name] | invite [name] | leave | info");
                    }
                } else if (verb == "trade") {
                    if (arg.rfind("apply ", 0) == 0) {
                        state_->trade_open = true;
                        SetupTradeNetworkCallbacks();
                        trade_dlg_.Open(state_, &trading_);
                        SendTradeApply(arg.substr(6));
                    } else if (arg == "cancel") {
                        SendTradeCancel();
                    } else if (arg == "confirm") {
                        SendTradeConfirm();
                    } else {
                        state_->chat_messages.push_back("Trade: /trade apply [name] | confirm | cancel");
                    }
                } else if (verb == "ui") {
                    if (arg.empty()) {
                        std::string help = "UI dialogs (" + std::to_string(script_dialogs_.RegisteredCount()) + "): ";
                        auto keys = script_dialogs_.ListKeys();
                        for (size_t i = 0; i < keys.size(); ++i) {
                            if (i) help += "|";
                            help += keys[i];
                            if (help.size() > 220) { help += "|..."; break; }
                        }
                        state_->chat_messages.push_back(help);
                    } else {
                        script_dialogs_.Toggle(arg, &wm_);
                    }
                } else if (verb == "ah" || verb == "auction") {
                    if (arg == "list" || arg.empty()) RequestConsignmentSearch("");
                    else RequestConsignmentSearch(arg);
                } else if (verb == "stall") {
                    if (arg.rfind("open ", 0) == 0) SendStreetStallOpen(arg.substr(5));
                    else if (arg.rfind("add ", 0) == 0) {
                        int price = std::atoi(arg.substr(4).c_str());
                        if (!state_->inventory.empty())
                            SendStreetStallAdd(static_cast<uint8_t>(state_->inventory[0].slot),
                                static_cast<uint32_t>(price > 0 ? price : 50));
                    } else if (arg.rfind("buy ", 0) == 0) {
                        int owner = 9200, slot = 0;
                        sscanf(arg.substr(4).c_str(), "%d %d", &owner, &slot);
                        SendStreetStallBuy(static_cast<uint32_t>(owner), slot);
                    } else if (arg == "close") SendStreetStallClose();
                    else if (arg == "list") RequestStreetStallList();
                    else state_->chat_messages.push_back("Stall: open [title] | add [price] | buy [owner slot] | close | list");
                } else if (verb == "help" || verb == "h") {
                    state_->chat_messages.push_back(
                        "Cmds: /party, /guild, /friend, /trade, /ah, /stall, /deposit, /w, /tp [map]");
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
                    ChangeMap((uint32_t)next);
                } else if (verb == "prevmap") {
                    int available[] = {13,14,15,19,20,21,22,23,25,26,27,28,31,32,41,51,55,56,60,63,64,71,74,75,96};
                    int count = sizeof(available)/sizeof(available[0]);
                    int cur = state_->map_id;
                    int prev = available[count - 1];
                    for (int i = 0; i < count; i++) {
                        if (available[i] == cur && i > 0) { prev = available[i - 1]; break; }
                    }
                    ChangeMap((uint32_t)prev);
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
        const bool online = !state_->offline_mode && network_ && network_->IsConnected();
        std::vector<SeedData> seeds;
        if (!state_->network_farm_seeds.empty()) {
            for (const auto& s : state_->network_farm_seeds) {
                seeds.push_back({static_cast<int>(s.seed_id), s.name,
                    static_cast<int>(s.growth_time_sec),
                    static_cast<int>(s.harvest_item_id), 1, 3});
            }
        } else {
            seeds = farm_.GetAllSeeds();
        }
        if (key >= 49 && key <= 53) { // 1-5: plant seed N on plot N
            int idx = key - 49;
            if (idx < (int)seeds.size()) {
                if (online)
                    SendFarmAction(1, static_cast<uint8_t>(idx), static_cast<uint32_t>(seeds[idx].id));
                else if (farm_.Plant(idx, seeds[idx].id))
                    state_->chat_messages.push_back("Planted " + seeds[idx].name + " on plot " + std::to_string(idx + 1));
                else
                    state_->chat_messages.push_back("Cannot plant on plot " + std::to_string(idx + 1));
            }
        }
        else if (key == 87) { // W: water first plot that needs it
            if (online) {
                for (int i = 0; i < 9; i++) {
                    const auto* plot = farm_.GetPlot(i);
                    if (plot && plot->seed_id > 0 && !plot->watered
                        && plot->growth_stage < plot->max_stages) {
                        SendFarmAction(2, static_cast<uint8_t>(i));
                        break;
                    }
                }
            } else {
                bool watered = false;
                for (int i = 0; i < 9; i++) {
                    auto* plot = farm_.GetPlot(i);
                    if (plot && plot->seed_id > 0 && !plot->watered && !plot->harvested) {
                        farm_.Water(i);
                        state_->chat_messages.push_back("Watered plot " + std::to_string(i + 1));
                        watered = true; break;
                    }
                }
                if (!watered) state_->chat_messages.push_back("Nothing to water.");
            }
        }
        else if (key == 82) { // R: harvest first ready plot
            if (online) {
                for (int i = 0; i < 9; i++) {
                    if (i < (int)state_->network_farm_plots.size()
                        && state_->network_farm_plots[i].ready) {
                        SendFarmAction(3, static_cast<uint8_t>(i));
                        break;
                    }
                    const auto* plot = farm_.GetPlot(i);
                    if (plot && plot->growth_stage >= plot->max_stages && !plot->harvested) {
                        SendFarmAction(3, static_cast<uint8_t>(i));
                        break;
                    }
                }
            } else {
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

void GameScreen::InitializeWorld() {
    std::string player_model = VFS::Find("assets/models/character/d_man.glb");
    if (player_model.empty()) player_model = "assets/models/d_man.glb";
    CharRenderer_Spawn(0, player_model, state_->player_x, state_->player_y, state_->player_z, 0xffffffff);
    hero_.Init(state_, audio_);
    hero_.SetPosition(state_->player_x, state_->player_y, state_->player_z);
    if (state_->inventory.empty()) {
        state_->inventory.push_back({1001, "Iron Sword", 1, 0, 0});
        state_->inventory.push_back({21000001, "Health Potion", 5, 1, 0});
    }
    if (state_->hotbar_skills[0] == 0) {
        const uint32_t presets[][3] = {{1, 10, 11}, {1, 20, 21}, {1, 30, 31}, {1, 2, 3}};
        int cid = std::min(3, std::max(0, state_->class_id));
        state_->hotbar_skills[0] = presets[cid][0];
        state_->hotbar_skills[1] = presets[cid][1];
        state_->hotbar_skills[2] = presets[cid][2];
    }
    if (state_->learned_skills.empty()) {
        state_->learned_skills.push_back(static_cast<int>(state_->hotbar_skills[0]));
        if (state_->hotbar_skills[1]) state_->learned_skills.push_back(static_cast<int>(state_->hotbar_skills[1]));
    }
    if (state_->offline_mode)
        SpawnMonstersFromMap();
    fade_dlg_.FadeIn(0.8f);
    spdlog::info("GameScreen: world initialized on map {}", state_->map_id);
}

void GameScreen::CastHotbarSkill(int slot) {
    if (slot < 0 || slot >= 10) return;
    if (state_->hotbar_cooldowns[slot] > 0.0f) return;
    uint32_t skill_id = state_->hotbar_skills[slot];
    if (!skill_id) return;
    if (!hero_.UseSkill(static_cast<int>(skill_id))) return;
    state_->hotbar_cooldowns[slot] = 2.5f;
    CharRenderer_Move(0, hero_.GetX(), hero_.GetY(), hero_.GetZ(), false, CharAnim::Attack);
    if (network_ && network_->IsConnected() && !state_->offline_mode) {
        flatbuffers::FlatBufferBuilder fbb;
        luna::protocol::Vec3 pos{hero_.GetX(), hero_.GetY(), hero_.GetZ()};
        auto req = luna::protocol::CreateAttackRequest(fbb, hero_.GetTarget(), static_cast<uint16_t>(skill_id), &pos);
        fbb.Finish(req);
        network_->SendPacket(luna::protocol::PacketType_MP_SKILL_CAST_SYN,
            fbb.GetBufferPointer(), fbb.GetSize());
    }
}

void GameScreen::BeginEntityInterpolation(uint32_t entity_id, float x, float y, float z) {
    EntityInterp ip;
    auto it = entity_interp_.find(entity_id);
    if (it != entity_interp_.end() && it->second.t < 1.0f) {
        glm::vec3 cur = it->second.from + (it->second.to - it->second.from) * it->second.t;
        ip.from = cur;
    } else {
        for (auto& e : state_->entities) {
            if (e.id == entity_id) {
                ip.from = {e.x, e.y, e.z};
                break;
            }
        }
    }
    ip.to = {x, y, z};
    ip.t = 0.0f;
    entity_interp_[entity_id] = ip;
    for (auto& e : state_->entities) {
        if (e.id == entity_id) {
            e.x = x;
            e.y = y;
            e.z = z;
            break;
        }
    }
}

void GameScreen::UpdateEntityInterpolation(float dt) {
    for (auto& [id, ip] : entity_interp_) {
        if (ip.t >= 1.0f) continue;
        ip.t = std::min(1.0f, ip.t + dt / ip.duration);
        glm::vec3 p = ip.from + (ip.to - ip.from) * ip.t;
        CharRenderer_Move(id, p.x, p.y, p.z, true, CharAnim::Walk);
    }
}

void GameScreen::RemoveNetworkEntity(uint32_t entity_id) {
    entity_interp_.erase(entity_id);
    CharRenderer_Remove(entity_id);
    state_->entities.erase(
        std::remove_if(state_->entities.begin(), state_->entities.end(),
            [&](const RemoteEntity& e) { return e.id == entity_id; }),
        state_->entities.end());
    monsters_.erase(
        std::remove_if(monsters_.begin(), monsters_.end(),
            [&](const Monster& m) { return m.GetID() == entity_id; }),
        monsters_.end());
}

void GameScreen::ApplyNetworkAttackResult(const luna::protocol::AttackResult* result) {
    if (!result) return;
    uint32_t target_id = result->target_id();
    int dmg = result->damage();
    bool miss = result->is_miss();

    for (auto& m : monsters_) {
        if (m.GetID() == target_id) {
            if (!miss) {
                m.TakeDamage(dmg);
                effect_mgr_.SpawnDamageNumber(m.GetX(), m.GetY() + 1.5f, m.GetZ(), dmg,
                    result->is_critical() ? DamageType::Crit : DamageType::Normal);
            } else {
                effect_mgr_.SpawnDamageNumber(m.GetX(), m.GetY() + 1.5f, m.GetZ(), 0, DamageType::Miss);
            }
            if (!m.IsAlive()) RemoveNetworkEntity(target_id);
            return;
        }
    }
    for (auto& e : state_->entities) {
        if (e.id == target_id) {
            e.hp_pct = result->target_hp_remaining() > 0
                ? (int)(100.0f * result->target_hp_remaining() / std::max(1, result->target_hp_remaining() + dmg))
                : 0;
            if (result->target_hp_remaining() <= 0) RemoveNetworkEntity(target_id);
            else if (!miss)
                effect_mgr_.SpawnDamageNumber(e.x, e.y + 1.5f, e.z, dmg,
                    result->is_critical() ? DamageType::Crit : DamageType::Normal);
            return;
        }
    }
}

void GameScreen::ApplySkillDamage(uint32_t skill_id) {
    if (!skill_id) return;
    if (!state_->offline_mode) return;
    int base = 25 + hero_.GetLevel() * 3;
    float range = 6.0f;
    bool aoe = false;
    auto skills = ClassAdvancement::GetSkillsForClass(state_->class_id);
    for (const auto& sk : skills) {
        if (static_cast<uint32_t>(sk.skill_id) == skill_id) {
            base = 30 + sk.required_level * 5;
            if (skill_id == 12 || skill_id == 30) aoe = true;
            if (skill_id == 22) range = 12.0f;
            break;
        }
    }
    int hits = 0;
    for (auto& m : monsters_) {
        if (!m.IsAlive()) continue;
        float dist = m.GetDistance(hero_.GetX(), hero_.GetZ());
        if (dist > range) continue;
        int dmg = base + (aoe ? 10 : 20);
        m.TakeDamage(dmg);
        effect_mgr_.SpawnDamageNumber(m.GetX(), m.GetY() + 1.5f, m.GetZ(), dmg, DamageType::Crit);
        hits++;
        if (!aoe) break;
    }
    if (hits > 0) {
        effect_mgr_.SpawnCameraShake(5.0f, 0.2f);
        state_->chat_messages.push_back("Skill hit " + std::to_string(hits) + " target(s)");
    }
    state_->pending_skill_id = 0;
}

void GameScreen::SendMovementUpdate(float dt) {
    if (!network_ || !network_->IsConnected() || state_->offline_mode) return;
    move_send_timer_ += dt;
    if (move_send_timer_ < 0.12f) return;
    move_send_timer_ = 0.0f;

    flatbuffers::FlatBufferBuilder fbb;
    luna::protocol::Vec3 dir{0, 0, 0};
    luna::protocol::Vec3 pos{hero_.GetX(), hero_.GetY(), hero_.GetZ()};
    auto req = luna::protocol::CreateMoveRequest(
        fbb, &dir, &pos,
        hero_.GetState() == HeroState::Run ? luna::protocol::MoveMode_Run : luna::protocol::MoveMode_Walk);
    fbb.Finish(req);
    network_->SendPacket(luna::protocol::PacketType_MP_MOVE_WALK,
        fbb.GetBufferPointer(), fbb.GetSize());
}

uint32_t GameScreen::GetSelectedCharId() const {
    if (!state_->characters.empty() && state_->selected_char < (int)state_->characters.size())
        return state_->characters[state_->selected_char].id;
    return 0;
}

void GameScreen::ApplyPartyResponse(const luna::protocol::PartyResponse* resp) {
    if (!resp || !resp->party()) return;
    auto party = resp->party();
    state_->party_id = party->party_id();
    state_->party_members.clear();
    if (party->members()) {
        for (auto m : *party->members()) {
            PartyMember pm;
            pm.id = m->character_id();
            pm.name = m->name() ? m->name()->str() : "?";
            pm.level = m->level();
            pm.hp = m->hp();
            pm.max_hp = m->max_hp();
            pm.map_id = m->map_id();
            pm.is_leader = m->is_leader();
            state_->party_members.push_back(pm);
        }
    }
    if (resp->result() != 0) {
        state_->chat_messages.push_back("Party action failed (code " + std::to_string(resp->result()) + ")");
    } else if (!state_->party_members.empty()) {
        state_->chat_messages.push_back("Party updated (" +
            std::to_string(state_->party_members.size()) + " members)");
    } else {
        state_->party_id = 0;
        state_->chat_messages.push_back("Left party");
    }
    if (state_->chat_messages.size() > 50) state_->chat_messages.erase(state_->chat_messages.begin());
}

void GameScreen::ApplyStorageResponse(const luna::protocol::StorageResponse* resp) {
    if (!resp || !resp->storage()) return;
    auto storage = resp->storage();
    state_->storage_gold = storage->gold();
    state_->storage_items.clear();
    if (storage->slots()) {
        for (auto s : *storage->slots()) {
            InvItem item;
            item.slot = s->slot_index();
            item.id = s->item_id();
            item.count = s->count();
            char buf[64];
            snprintf(buf, sizeof(buf), "Item_%u", item.id);
            item.name = buf;
            state_->storage_items.push_back(item);
        }
    }
    if (resp->result() != 0)
        state_->chat_messages.push_back("Storage action failed");
    else if (state_->storage_open)
        storage_dlg_.UpdateFromState(state_);
}

void GameScreen::SendPartyCreate() {
    if (!network_ || !network_->IsConnected() || state_->offline_mode) return;
    flatbuffers::FlatBufferBuilder fbb;
    auto req = luna::protocol::CreatePartyCreateRequest(fbb, GetSelectedCharId());
    fbb.Finish(req);
    network_->SendPacket(luna::protocol::PacketType_MP_PARTY_CREATE_SYN,
        fbb.GetBufferPointer(), fbb.GetSize());
}

void GameScreen::SendPartyInvite(const std::string& name) {
    if (!network_ || !network_->IsConnected() || state_->offline_mode || name.empty()) return;
    flatbuffers::FlatBufferBuilder fbb;
    auto req = luna::protocol::CreatePartyInviteRequestDirect(
        fbb, GetSelectedCharId(), name.c_str());
    fbb.Finish(req);
    network_->SendPacket(luna::protocol::PacketType_MP_PARTY_INVITE_SYN,
        fbb.GetBufferPointer(), fbb.GetSize());
}

void GameScreen::SendPartyLeave() {
    if (!network_ || !network_->IsConnected() || state_->offline_mode || state_->party_id == 0) return;
    flatbuffers::FlatBufferBuilder fbb;
    auto req = luna::protocol::CreatePartyLeaveRequest(fbb, GetSelectedCharId(), state_->party_id);
    fbb.Finish(req);
    network_->SendPacket(luna::protocol::PacketType_MP_PARTY_LEAVE_SYN,
        fbb.GetBufferPointer(), fbb.GetSize());
}

void GameScreen::RequestStorageList() {
    if (!network_ || !network_->IsConnected() || state_->offline_mode) return;
    flatbuffers::FlatBufferBuilder fbb;
    auto req = luna::protocol::CreateStorageListRequest(fbb, GetSelectedCharId());
    fbb.Finish(req);
    network_->SendPacket(luna::protocol::PacketType_MP_STORAGE_LIST_SYN,
        fbb.GetBufferPointer(), fbb.GetSize());
}

void GameScreen::SendStorageDeposit(uint8_t inv_slot, uint16_t count) {
    if (!network_ || !network_->IsConnected() || state_->offline_mode) return;
    flatbuffers::FlatBufferBuilder fbb;
    auto req = luna::protocol::CreateStorageDepositRequest(
        fbb, GetSelectedCharId(), inv_slot, 0, count);
    fbb.Finish(req);
    network_->SendPacket(luna::protocol::PacketType_MP_STORAGE_DEPOSIT_SYN,
        fbb.GetBufferPointer(), fbb.GetSize());
}

void GameScreen::SendStorageWithdraw(uint8_t storage_slot, uint16_t count) {
    if (!network_ || !network_->IsConnected() || state_->offline_mode) return;
    flatbuffers::FlatBufferBuilder fbb;
    auto req = luna::protocol::CreateStorageWithdrawRequest(
        fbb, GetSelectedCharId(), storage_slot, 0, count);
    fbb.Finish(req);
    network_->SendPacket(luna::protocol::PacketType_MP_STORAGE_WITHDRAW_SYN,
        fbb.GetBufferPointer(), fbb.GetSize());
}

void GameScreen::ApplyFriendResponse(const luna::protocol::FriendResponse* resp) {
    if (!resp || !resp->list()) return;
    state_->friends.clear();
    if (resp->list()->friends()) {
        for (auto f : *resp->list()->friends()) {
            FriendEntry entry;
            entry.id = f->character_id();
            entry.name = f->name() ? f->name()->str() : "?";
            entry.level = f->level();
            entry.online = f->online();
            entry.map_id = f->map_id();
            state_->friends.push_back(entry);
        }
    }
    if (resp->result() != 0)
        state_->chat_messages.push_back("Friend action failed (code " + std::to_string(resp->result()) + ")");
    else if (state_->friend_open)
        friend_dlg_.UpdateFromState(state_);
}

void GameScreen::ApplyGuildResponse(const luna::protocol::GuildResponse* resp) {
    if (!resp || !resp->guild()) return;
    auto guild = resp->guild();
    state_->guild_id = guild->guild_id();
    state_->guild_name = guild->name() ? guild->name()->str() : "";
    state_->guild_level = guild->level();
    state_->guild_gp = guild->gp();
    state_->guild_members.clear();
    if (guild->members()) {
        for (auto m : *guild->members()) {
            GuildMemberEntry entry;
            entry.id = m->character_id();
            entry.name = m->name() ? m->name()->str() : "?";
            entry.level = m->level();
            entry.rank = m->rank();
            entry.online = m->online();
            state_->guild_members.push_back(entry);
        }
    }
    if (resp->result() != 0)
        state_->chat_messages.push_back("Guild action failed (code " + std::to_string(resp->result()) + ")");
    else if (!state_->guild_name.empty())
        state_->chat_messages.push_back("Guild: " + state_->guild_name +
            " (" + std::to_string(state_->guild_members.size()) + " members)");
    else if (state_->guild_id == 0)
        state_->chat_messages.push_back("Left guild");
    if (state_->guild_open)
        guild_dlg_.UpdateFromState(state_);
}

void GameScreen::RequestFriendList() {
    if (!network_ || !network_->IsConnected() || state_->offline_mode) return;
    flatbuffers::FlatBufferBuilder fbb;
    auto req = luna::protocol::CreateFriendListRequest(fbb, GetSelectedCharId());
    fbb.Finish(req);
    network_->SendPacket(luna::protocol::PacketType_MP_FRIEND_LIST_SYN,
        fbb.GetBufferPointer(), fbb.GetSize());
}

void GameScreen::SendFriendAdd(const std::string& name) {
    if (!network_ || !network_->IsConnected() || state_->offline_mode || name.empty()) return;
    flatbuffers::FlatBufferBuilder fbb;
    auto req = luna::protocol::CreateFriendAddRequestDirect(fbb, GetSelectedCharId(), name.c_str());
    fbb.Finish(req);
    network_->SendPacket(luna::protocol::PacketType_MP_FRIEND_ADD_SYN,
        fbb.GetBufferPointer(), fbb.GetSize());
}

void GameScreen::SendFriendDelete(uint32_t friend_id) {
    if (!network_ || !network_->IsConnected() || state_->offline_mode || !friend_id) return;
    flatbuffers::FlatBufferBuilder fbb;
    auto req = luna::protocol::CreateFriendDeleteRequest(fbb, GetSelectedCharId(), friend_id);
    fbb.Finish(req);
    network_->SendPacket(luna::protocol::PacketType_MP_FRIEND_DEL_SYN,
        fbb.GetBufferPointer(), fbb.GetSize());
}

void GameScreen::SendGuildCreate(const std::string& name) {
    if (!network_ || !network_->IsConnected() || state_->offline_mode || name.empty()) return;
    flatbuffers::FlatBufferBuilder fbb;
    auto req = luna::protocol::CreateGuildCreateRequestDirect(fbb, GetSelectedCharId(), name.c_str());
    fbb.Finish(req);
    network_->SendPacket(luna::protocol::PacketType_MP_GUILD_CREATE_SYN,
        fbb.GetBufferPointer(), fbb.GetSize());
}

void GameScreen::RequestGuildInfo() {
    if (!network_ || !network_->IsConnected() || state_->offline_mode) return;
    flatbuffers::FlatBufferBuilder fbb;
    auto req = luna::protocol::CreateGuildInfoRequest(fbb, GetSelectedCharId());
    fbb.Finish(req);
    network_->SendPacket(luna::protocol::PacketType_MP_GUILD_INFO_SYN,
        fbb.GetBufferPointer(), fbb.GetSize());
}

void GameScreen::SendGuildInvite(const std::string& name) {
    if (!network_ || !network_->IsConnected() || state_->offline_mode || name.empty()) return;
    flatbuffers::FlatBufferBuilder fbb;
    auto req = luna::protocol::CreateGuildInviteRequestDirect(fbb, GetSelectedCharId(), name.c_str());
    fbb.Finish(req);
    network_->SendPacket(luna::protocol::PacketType_MP_GUILD_ADDMEMBER_SYN,
        fbb.GetBufferPointer(), fbb.GetSize());
}

void GameScreen::SendGuildLeave() {
    if (!network_ || !network_->IsConnected() || state_->offline_mode || state_->guild_id == 0) return;
    flatbuffers::FlatBufferBuilder fbb;
    auto req = luna::protocol::CreateGuildLeaveRequest(fbb, GetSelectedCharId(), state_->guild_id);
    fbb.Finish(req);
    network_->SendPacket(luna::protocol::PacketType_MP_GUILD_SECEDE_SYN,
        fbb.GetBufferPointer(), fbb.GetSize());
}

static void FillTradeSide(GameState* state, bool self,
    const luna::protocol::TradeSide* side) {
    if (!state || !side) return;
    auto& items = self ? state->trade_my_items : state->trade_their_items;
    items.clear();
    if (side->items()) {
        for (auto it : *side->items()) {
            TradeOfferItem offer;
            offer.trade_slot = it->trade_slot();
            offer.item_id = it->item_id();
            offer.count = it->count();
            offer.inv_slot = it->inv_slot();
            char buf[64];
            snprintf(buf, sizeof(buf), "Item_%u", offer.item_id);
            offer.name = buf;
            items.push_back(offer);
        }
    }
    if (self) {
        state->trade_my_gold = side->gold();
        state->trade_my_confirmed = side->confirmed();
    } else {
        state->trade_their_gold = side->gold();
        state->trade_their_confirmed = side->confirmed();
        if (side->name()) state->trade_partner_name = side->name()->str();
    }
}

void GameScreen::ApplyTradeResponse(const luna::protocol::TradeResponse* resp) {
    if (!resp) return;
    if (resp->result() != 0) {
        state_->chat_messages.push_back("Trade failed (code " + std::to_string(resp->result()) + ")");
        if (state_->chat_messages.size() > 50) state_->chat_messages.erase(state_->chat_messages.begin());
        return;
    }
    if (!resp->session()) return;
    auto session = resp->session();
    state_->trade_session_id = session->session_id();
    state_->trade_active = session->session_id() != 0;
    state_->trade_completed = session->completed();
    if (session->self_side()) FillTradeSide(state_, true, session->self_side());
    if (session->partner_side()) FillTradeSide(state_, false, session->partner_side());
    if (session->completed()) {
        state_->trade_active = false;
        state_->chat_messages.push_back("Trade completed with " + state_->trade_partner_name + "!");
    }
    if (state_->trade_open)
        trade_dlg_.UpdateFromState(state_);
}

void GameScreen::SetupTradeNetworkCallbacks() {
    TradeNetworkCallbacks cb;
    cb.enabled = network_ && network_->IsConnected() && !state_->offline_mode;
    if (!cb.enabled) {
        trade_dlg_.SetNetworkCallbacks(cb);
        return;
    }
    cb.on_add_item = [this](uint8_t inv_slot) { SendTradeAddItem(inv_slot); };
    cb.on_add_gold = [this](uint32_t add) {
        uint32_t next = state_->trade_my_gold + add;
        if (next <= static_cast<uint32_t>(state_->gold)) SendTradeSetGold(next);
    };
    cb.on_confirm = [this]() { SendTradeConfirm(); };
    cb.on_cancel = [this]() { SendTradeCancel(); };
    trade_dlg_.SetNetworkCallbacks(cb);
}

void GameScreen::SendTradeApply(const std::string& target) {
    if (!network_ || !network_->IsConnected() || state_->offline_mode) return;
    flatbuffers::FlatBufferBuilder fbb;
    auto req = luna::protocol::CreateTradeApplyRequestDirect(
        fbb, GetSelectedCharId(), target.c_str());
    fbb.Finish(req);
    network_->SendPacket(luna::protocol::PacketType_MP_EXCHANGE_APPLY_SYN,
        fbb.GetBufferPointer(), fbb.GetSize());
}

void GameScreen::SendTradeCancel() {
    if (!network_ || !network_->IsConnected() || state_->offline_mode) return;
    if (!state_->trade_session_id) return;
    flatbuffers::FlatBufferBuilder fbb;
    auto req = luna::protocol::CreateTradeSessionRequest(
        fbb, state_->trade_session_id, GetSelectedCharId());
    fbb.Finish(req);
    network_->SendPacket(luna::protocol::PacketType_MP_EXCHANGE_CANCEL_SYN,
        fbb.GetBufferPointer(), fbb.GetSize());
    state_->trade_active = false;
    state_->trade_session_id = 0;
}

void GameScreen::SendTradeAddItem(uint8_t inv_slot) {
    if (!network_ || !network_->IsConnected() || state_->offline_mode || !state_->trade_active) return;
    flatbuffers::FlatBufferBuilder fbb;
    auto req = luna::protocol::CreateTradeAddItemRequest(
        fbb, state_->trade_session_id, GetSelectedCharId(), inv_slot, 0, 1);
    fbb.Finish(req);
    network_->SendPacket(luna::protocol::PacketType_MP_EXCHANGE_ADDITEM_SYN,
        fbb.GetBufferPointer(), fbb.GetSize());
}

void GameScreen::SendTradeSetGold(uint32_t gold) {
    if (!network_ || !network_->IsConnected() || state_->offline_mode || !state_->trade_active) return;
    flatbuffers::FlatBufferBuilder fbb;
    auto req = luna::protocol::CreateTradeSetGoldRequest(
        fbb, state_->trade_session_id, GetSelectedCharId(), gold);
    fbb.Finish(req);
    network_->SendPacket(luna::protocol::PacketType_MP_EXCHANGE_SETGOLD_SYN,
        fbb.GetBufferPointer(), fbb.GetSize());
}

void GameScreen::SendTradeConfirm() {
    if (!network_ || !network_->IsConnected() || state_->offline_mode || !state_->trade_active) return;
    flatbuffers::FlatBufferBuilder fbb;
    auto req = luna::protocol::CreateTradeSessionRequest(
        fbb, state_->trade_session_id, GetSelectedCharId());
    fbb.Finish(req);
    network_->SendPacket(luna::protocol::PacketType_MP_EXCHANGE_CONFIRM_SYN,
        fbb.GetBufferPointer(), fbb.GetSize());
}

static AuctionListing FromConsignmentFb(const luna::protocol::ConsignmentListing* l) {
    AuctionListing a;
    if (!l) return a;
    a.id = l->listing_id();
    a.seller_id = l->seller_id();
    a.seller_name = l->seller_name() ? l->seller_name()->str() : "";
    a.item_id = l->item_id();
    a.item_name = l->item_name() ? l->item_name()->str() : "";
    a.item_count = l->item_count();
    a.enchant_level = l->enchant_level();
    a.bid_price = l->bid_price();
    a.buyout_price = l->buyout_price();
    a.current_bid = l->current_bid();
    a.bidder_id = l->bidder_id();
    a.bidder_name = l->bidder_name() ? l->bidder_name()->str() : "";
    a.time_remaining = l->time_remaining();
    a.sold = l->sold();
    a.active = l->active();
    return a;
}

static StreetStallView FromStallFb(const luna::protocol::StreetStallInfo* s) {
    StreetStallView view;
    if (!s) return view;
    view.owner_id = s->owner_id();
    view.owner_name = s->owner_name() ? s->owner_name()->str() : "";
    view.title = s->title() ? s->title()->str() : "";
    view.open = s->open();
    if (s->items()) {
        for (auto it : *s->items()) {
            StreetStallSlot slot;
            slot.slot = it->slot();
            slot.item_id = it->item_id();
            slot.count = it->count();
            slot.price = it->price();
            slot.item_name = it->item_name() ? it->item_name()->str() : "";
            view.items.push_back(slot);
        }
    }
    return view;
}

void GameScreen::ApplyConsignmentResponse(const luna::protocol::ConsignmentResponse* resp,
    bool mine_only, bool bids_only) {
    if (!resp) return;
    if (resp->result() != 0) {
        state_->chat_messages.push_back("Auction action failed (code " +
            std::to_string(resp->result()) + ")");
        return;
    }
    std::vector<AuctionListing> listings;
    if (resp->listings() && resp->listings()->listings()) {
        for (auto l : *resp->listings()->listings())
            listings.push_back(FromConsignmentFb(l));
    }
    if (mine_only) state_->consignment_mine = listings;
    else if (bids_only) state_->consignment_bids = listings;
    else state_->consignment_browse = listings;
    if (state_->consignment_open)
        consignment_dlg_.UpdateFromState(state_);
}

void GameScreen::ApplyStreetStallResponse(const luna::protocol::StreetStallResponse* resp) {
    if (!resp) return;
    if (resp->result() != 0) {
        state_->chat_messages.push_back("Street stall failed (code " +
            std::to_string(resp->result()) + ")");
        return;
    }
    state_->my_stall = FromStallFb(resp->stall());
    state_->nearby_stalls.clear();
    if (resp->nearby()) {
        for (auto s : *resp->nearby())
            state_->nearby_stalls.push_back(FromStallFb(s));
    }
    if (state_->consignment_open)
        consignment_dlg_.UpdateFromState(state_);
}

void GameScreen::SetupConsignmentNetworkCallbacks() {
    ConsignmentNetworkCallbacks cb;
    cb.enabled = network_ && network_->IsConnected() && !state_->offline_mode;
    if (!cb.enabled) {
        consignment_dlg_.SetNetworkCallbacks(cb);
        return;
    }
    cb.on_search = [this](const std::string& q) { RequestConsignmentSearch(q); };
    cb.on_refresh = [this](bool mine, bool bids) { RequestConsignmentRefresh(mine, bids); };
    cb.on_trade = [this](uint64_t id, bool buyout, uint32_t amount) {
        SendConsignmentTrade(id, buyout, amount);
    };
    cb.on_list = [this](uint8_t slot, uint16_t count, uint32_t bid, uint32_t buyout) {
        SendConsignmentList(slot, count, bid, buyout);
    };
    cb.on_cancel = [this](uint64_t id) { SendConsignmentCancel(id); };
    cb.on_stall_open = [this](const std::string& title) { SendStreetStallOpen(title); };
    cb.on_stall_add = [this](uint8_t inv_slot, uint32_t price) {
        SendStreetStallAdd(inv_slot, price);
    };
    cb.on_stall_buy = [this](uint32_t owner, uint8_t slot) {
        SendStreetStallBuy(owner, slot);
    };
    cb.on_stall_close = [this]() { SendStreetStallClose(); };
    cb.on_stall_list = [this]() { RequestStreetStallList(); };
    consignment_dlg_.SetNetworkCallbacks(cb);
}

void GameScreen::RequestConsignmentSearch(const std::string& query) {
    if (!network_ || !network_->IsConnected() || state_->offline_mode) return;
    pending_consignment_mine_ = false;
    pending_consignment_bids_ = false;
    flatbuffers::FlatBufferBuilder fbb;
    auto req = luna::protocol::CreateConsignmentListRequestDirect(
        fbb, GetSelectedCharId(), query.c_str(), false, false);
    fbb.Finish(req);
    network_->SendPacket(luna::protocol::PacketType_MP_CONSIGNMENT_SEARCH_SYN,
        fbb.GetBufferPointer(), fbb.GetSize());
}

void GameScreen::RequestConsignmentRefresh(bool mine, bool bids) {
    if (!network_ || !network_->IsConnected() || state_->offline_mode) return;
    pending_consignment_mine_ = mine;
    pending_consignment_bids_ = bids;
    flatbuffers::FlatBufferBuilder fbb;
    auto req = luna::protocol::CreateConsignmentListRequestDirect(
        fbb, GetSelectedCharId(), "", mine, bids);
    fbb.Finish(req);
    network_->SendPacket(luna::protocol::PacketType_MP_CONSIGNMENT_GETLIST_SYN,
        fbb.GetBufferPointer(), fbb.GetSize());
}

void GameScreen::SendConsignmentList(uint8_t inv_slot, uint16_t count,
    uint32_t bid, uint32_t buyout) {
    if (!network_ || !network_->IsConnected() || state_->offline_mode) return;
    flatbuffers::FlatBufferBuilder fbb;
    auto req = luna::protocol::CreateConsignmentRegisterRequest(
        fbb, GetSelectedCharId(), inv_slot, count, bid, buyout);
    fbb.Finish(req);
    network_->SendPacket(luna::protocol::PacketType_MP_CONSIGNMENT_REGIST_SYN,
        fbb.GetBufferPointer(), fbb.GetSize());
}

void GameScreen::SendConsignmentTrade(uint64_t listing_id, bool buyout, uint32_t amount) {
    if (!network_ || !network_->IsConnected() || state_->offline_mode) return;
    flatbuffers::FlatBufferBuilder fbb;
    auto req = luna::protocol::CreateConsignmentTradeRequest(
        fbb, GetSelectedCharId(), listing_id, amount, buyout);
    fbb.Finish(req);
    network_->SendPacket(luna::protocol::PacketType_MP_CONSIGNMENT_UPDATE_SYN,
        fbb.GetBufferPointer(), fbb.GetSize());
}

void GameScreen::SendConsignmentCancel(uint64_t listing_id) {
    if (!network_ || !network_->IsConnected() || state_->offline_mode) return;
    flatbuffers::FlatBufferBuilder fbb;
    auto req = luna::protocol::CreateConsignmentCancelRequest(
        fbb, GetSelectedCharId(), listing_id);
    fbb.Finish(req);
    network_->SendPacket(luna::protocol::PacketType_MP_CONSIGNMENT_CANCEL_SYN,
        fbb.GetBufferPointer(), fbb.GetSize());
}

void GameScreen::SendStreetStallOpen(const std::string& title) {
    if (!network_ || !network_->IsConnected() || state_->offline_mode) return;
    flatbuffers::FlatBufferBuilder fbb;
    auto req = luna::protocol::CreateStreetStallOpenRequestDirect(
        fbb, GetSelectedCharId(), title.c_str());
    fbb.Finish(req);
    network_->SendPacket(luna::protocol::PacketType_MP_STREETSTALL_OPEN_SYN,
        fbb.GetBufferPointer(), fbb.GetSize());
}

void GameScreen::SendStreetStallAdd(uint8_t inv_slot, uint32_t price) {
    if (!network_ || !network_->IsConnected() || state_->offline_mode) return;
    flatbuffers::FlatBufferBuilder fbb;
    auto req = luna::protocol::CreateStreetStallAddItemRequest(
        fbb, GetSelectedCharId(), inv_slot, 0, 1, price);
    fbb.Finish(req);
    network_->SendPacket(luna::protocol::PacketType_MP_STREETSTALL_ADDITEM_SYN,
        fbb.GetBufferPointer(), fbb.GetSize());
}

void GameScreen::SendStreetStallBuy(uint32_t owner_id, uint8_t stall_slot) {
    if (!network_ || !network_->IsConnected() || state_->offline_mode) return;
    flatbuffers::FlatBufferBuilder fbb;
    auto req = luna::protocol::CreateStreetStallBuyRequest(
        fbb, GetSelectedCharId(), owner_id, stall_slot, 1);
    fbb.Finish(req);
    network_->SendPacket(luna::protocol::PacketType_MP_STREETSTALL_BUY_SYN,
        fbb.GetBufferPointer(), fbb.GetSize());
}

void GameScreen::SendStreetStallClose() {
    if (!network_ || !network_->IsConnected() || state_->offline_mode) return;
    flatbuffers::FlatBufferBuilder fbb;
    auto req = luna::protocol::CreateStreetStallCloseRequest(fbb, GetSelectedCharId());
    fbb.Finish(req);
    network_->SendPacket(luna::protocol::PacketType_MP_STREETSTALL_CLOSE_SYN,
        fbb.GetBufferPointer(), fbb.GetSize());
}

void GameScreen::RequestStreetStallList() {
    if (!network_ || !network_->IsConnected() || state_->offline_mode) return;
    flatbuffers::FlatBufferBuilder fbb;
    auto req = luna::protocol::CreateStreetStallListRequest(fbb, GetSelectedCharId());
    fbb.Finish(req);
    network_->SendPacket(luna::protocol::PacketType_MP_STREETSTALL_LIST_SYN,
        fbb.GetBufferPointer(), fbb.GetSize());
}

void GameScreen::ClearNetworkEntities() {
    for (const auto& e : state_->entities)
        CharRenderer_Remove(e.id);
    state_->entities.clear();
    entity_interp_.clear();
    monsters_.clear();
}

void GameScreen::OnChangeMapAck(const luna::protocol::ChangeMapResponse* resp) {
    changemap_pending_ = false;
    if (!resp || resp->result() != 0) {
        state_->map_changing = false;
        state_->chat_messages.push_back("Change map failed (code " +
            std::to_string(resp ? resp->result() : 255) + ")");
        if (state_->chat_messages.size() > 50) state_->chat_messages.erase(state_->chat_messages.begin());
        return;
    }

    pending_map_id_ = resp->map_id();
    state_->map_server_port = resp->map_port();
    if (resp->position()) {
        state_->player_x = resp->position()->x();
        state_->player_y = resp->position()->y();
        state_->player_z = resp->position()->z();
    }

    fade_dlg_.FadeOut(0.6f);
    fade_dlg_.SetCallback([this]() {
        network_->Disconnect();
        ClearNetworkEntities();
        state_->map_id = pending_map_id_;
        if (map_) {
            map_->Unload();
            map_->Load(std::to_string(pending_map_id_));
        }
        hero_.SetPosition(state_->player_x, state_->player_y, state_->player_z);
        state_->current_state = ClientState::MapChange;
        if (state_->offline_mode)
            SpawnMonstersFromMap();
        fade_dlg_.FadeIn(0.6f);
        state_->current_state = ClientState::GameIn;
        state_->connecting = true;
        state_->chat_messages.push_back("Entering map " + std::to_string(pending_map_id_) + "...");
        if (state_->chat_messages.size() > 50) state_->chat_messages.erase(state_->chat_messages.begin());
    });
}

void GameScreen::ChangeMap(uint32_t map_id) {
    if (map_id == state_->map_id) return;
    pending_map_id_ = map_id;

    if (!state_->offline_mode && network_ && network_->IsConnected()) {
        uint32_t char_id = 0;
        if (!state_->characters.empty() && state_->selected_char < (int)state_->characters.size())
            char_id = state_->characters[state_->selected_char].id;
        flatbuffers::FlatBufferBuilder fbb;
        auto req = luna::protocol::CreateChangeMapRequestDirect(
            fbb, state_->session_token.c_str(), char_id, static_cast<uint16_t>(map_id));
        fbb.Finish(req);
        network_->SendPacket(luna::protocol::PacketType_MP_USERCONN_CHANGEMAP_SYN,
            fbb.GetBufferPointer(), fbb.GetSize());
        changemap_pending_ = true;
        state_->map_changing = true;
        state_->chat_messages.push_back("Requesting map change to " + std::to_string(map_id) + "...");
        if (state_->chat_messages.size() > 50) state_->chat_messages.erase(state_->chat_messages.begin());
        return;
    }

    fade_dlg_.FadeOut(0.6f);
    fade_dlg_.SetCallback([this]() {
        state_->map_id = pending_map_id_;
        if (map_) {
            map_->Unload();
            map_->Load(std::to_string(pending_map_id_));
        }
        state_->current_state = ClientState::MapChange;
        SpawnMonstersFromMap();
        fade_dlg_.FadeIn(0.6f);
        state_->current_state = ClientState::GameIn;
        state_->chat_messages.push_back("Entered map " + std::to_string(pending_map_id_));
    });
}

void GameScreen::Update(float dt) {
    fade_dlg_.Update(dt);
    pk_dlg_.Update(dt);
    helper_dlg_.Update(dt);
    minimap_dlg_.Update(dt);
    effect_mgr_.Update(dt);
    sky_.Update(dt);
    weather_.Update(dt, state_->player_x, state_->player_z);
    state_->shake_x = effect_mgr_.GetShakeOffsetX();
    state_->shake_y = effect_mgr_.GetShakeOffsetY();
    if (state_->connecting) {
        state_->connecting = false;
        if (network_->IsConnected())
            network_->Disconnect();
        uint32_t mid = state_->map_id ? state_->map_id : 51;
        uint16_t map_port = state_->map_server_port;
        if (!map_port) map_port = static_cast<uint16_t>(8200 + mid);
        state_->map_server_port = 0;
        if (network_->Connect("127.0.0.1", map_port)) {
            spdlog::info("Connected to MapServer on port {} (map {})", map_port, mid);
            if (!state_->characters.empty() && state_->selected_char < (int)state_->characters.size()) {
                auto& ch = state_->characters[state_->selected_char];
                flatbuffers::FlatBufferBuilder fbb;
                auto req = luna::protocol::CreateEnterWorldRequestDirect(
                    fbb, state_->session_token.c_str(), ch.id);
                fbb.Finish(req);
                network_->SendPacket(luna::protocol::PacketType_MP_USERCONN_GAMEIN_SYN,
                    fbb.GetBufferPointer(), fbb.GetSize());
            }
        }
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
    if (ui_) legacy_hud_.Update(state_, ui_->logicalWidth, ui_->logicalHeight);
    state_->player_y = hero_.GetY();
    SendMovementUpdate(dt);
    if (!state_->offline_mode)
        UpdateEntityInterpolation(dt);

    for (int i = 0; i < 10; ++i) {
        if (state_->hotbar_cooldowns[i] > 0.0f)
            state_->hotbar_cooldowns[i] = std::max(0.0f, state_->hotbar_cooldowns[i] - dt);
    }

    if (state_->offline_mode) {
        if (hero_.GetState() == HeroState::Skill && !skill_damage_applied_) {
            ApplySkillDamage(state_->pending_skill_id);
            skill_damage_applied_ = true;
        } else if (hero_.GetState() != HeroState::Skill) {
            skill_damage_applied_ = false;
        }
    } else if (hero_.GetState() != HeroState::Skill) {
        skill_damage_applied_ = false;
    }

    for (auto& m : monsters_) {
        float mh = terrain_ ? terrain_->GetHeight(m.GetX(), m.GetZ()) + 0.5f : 0.5f;
        m.SetPosition(m.GetX(), mh, m.GetZ());
        m.Update(dt, hero_.GetX(), hero_.GetZ());
        if (m.JustAggroed()) {
            for (auto& ally : monsters_) {
                if (ally.GetID() == m.GetID() || !ally.IsAlive()) continue;
                if (ally.GetDistance(m.GetX(), m.GetZ()) < 15.0f)
                    ally.ForceAggro(hero_.GetX(), hero_.GetZ());
            }
            m.ClearAggroFlag();
        }
        if (m.IsAlive() && m.GetDistance(hero_.GetX(), hero_.GetZ()) < 12.0f)
            hero_.SetTarget(m.GetID());
    }

    if (state_->offline_mode)
        DoCombat(dt);

    minimap_dlg_.SetPlayerPos(hero_.GetX(), hero_.GetZ());
    minimap_dlg_.ClearEntities();
    for (auto& m : monsters_) {
        if (m.IsAlive())
            minimap_dlg_.AddEntity(m.GetID(), m.GetX(), m.GetZ(), 0xffff6633, 4.0f);
    }

    if (state_->click_marker_time > 0.0f)
        state_->click_marker_time = std::max(0.0f, state_->click_marker_time - dt);

    for (auto& df : state_->damage_floats) df.life -= dt;
    state_->damage_floats.erase(
        std::remove_if(state_->damage_floats.begin(), state_->damage_floats.end(),
            [](auto& df) { return df.life <= 0; }), state_->damage_floats.end());

    { entt::registry dummy; particleSys_.Update(dummy, dt); }

    DoLevelUp();

    if (legacy_hud_.IsActive()) legacy_hud_.SyncChatInput(state_);
    else chat_panel_.Update(state_, dt);

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
    pet_.Update(dt, state_->player_x, state_->player_y, state_->player_z);
    if (state_->farm_open && !state_->offline_mode && network_ && network_->IsConnected()) {
        farm_poll_timer_ += dt;
        if (farm_poll_timer_ >= 5.0f) {
            farm_poll_timer_ = 0.0f;
            RequestFarmInfo();
        }
        farm_local_tick_ += dt;
        if (farm_local_tick_ >= 1.0f) {
            farm_local_tick_ = 0.0f;
            for (auto& np : state_->network_farm_plots) {
                if (np.seed_id == 0 || np.ready || !np.watered) continue;
                if (auto* plot = farm_.GetPlot(np.plot_id)) {
                    plot->growth_timer += 1.0f;
                    if (plot->growth_time > 0 && plot->growth_timer >= plot->growth_time
                        && plot->growth_stage < plot->max_stages) {
                        plot->growth_timer = 0;
                        plot->growth_stage++;
                        plot->watered = false;
                        np.watered = false;
                    }
                }
            }
        }
    }
    if (state_->pet_open && !state_->offline_mode && network_ && network_->IsConnected()) {
        pet_poll_timer_ += dt;
        if (pet_poll_timer_ >= 5.0f) {
            pet_poll_timer_ = 0.0f;
            RequestPetInfo();
        }
    }
    if (state_->family_open && !state_->offline_mode && network_ && network_->IsConnected()) {
        family_poll_timer_ += dt;
        if (family_poll_timer_ >= 5.0f) {
            family_poll_timer_ = 0.0f;
            RequestFamilyInfo();
        }
    }
    if (state_->guild_open && !state_->offline_mode && network_ && network_->IsConnected()) {
        siege_poll_timer_ += dt;
        if (siege_poll_timer_ >= 5.0f) {
            siege_poll_timer_ = 0.0f;
            RequestSiegeInfo();
        }
    }
    if (state_->tournament_open && !state_->offline_mode && network_ && network_->IsConnected()) {
        tournament_poll_timer_ += dt;
        if (tournament_poll_timer_ >= 5.0f) {
            tournament_poll_timer_ = 0.0f;
            RequestTournamentList();
        }
    }
    if (state_->cashshop_open && !state_->offline_mode && network_ && network_->IsConnected()) {
        cashshop_poll_timer_ += dt;
        if (cashshop_poll_timer_ >= 5.0f) {
            cashshop_poll_timer_ = 0.0f;
            RequestCashShopList();
        }
    }
    if (state_->housing_open && !state_->offline_mode && network_ && network_->IsConnected()) {
        housing_poll_timer_ += dt;
        if (housing_poll_timer_ >= 5.0f) {
            housing_poll_timer_ = 0.0f;
            RequestHousingInfo();
        }
    }
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
    if (state_->battle_delay_timer > 0.0f) {
        state_->battle_delay_timer = std::max(0.0f, state_->battle_delay_timer - dt);
        return;
    }

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

    // Old Luna auto-attack cadence: 1.2s per swing, damage lands mid-swing
    // (0.7s wind-up) followed by a short 0.3s recovery lock.
    state_->combat_timer += dt;
    if (state_->combat_timer < 1.2f) return;
    state_->combat_timer = 0;

    state_->combat_cast_time = 0.7f;
    state_->combat_anim_lock = 0.3f;
    CharRenderer_Move(0, hero_.GetX(), hero_.GetY(), hero_.GetZ(), false, CharAnim::Attack);

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
                InvItem drop{21000001, "Health Potion", 1, (int)state_->inventory.size(), 0};
                for (auto& item : state_->inventory) {
                    if (item.id == drop.id) { item.count += drop.count; drop.count = 0; break; }
                }
                if (drop.count > 0) state_->inventory.push_back(drop);
                // Short out-of-combat beat after a kill (Old felt ~2s, the
                // previous 10s froze the player and read as a hang)
                state_->battle_delay_timer = 2.0f;
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

void GameScreen::SpawnMonstersFromMap() {
    if (!gamedb_) return;

    uint32_t map_id = state_->map_id ? state_->map_id : 51;
    monsters_.clear();

    auto spawns = gamedb_->GetMonsterSpawns(map_id);
    for (const auto& s : spawns) {
        const MonsterData* tmpl = gamedb_->GetMonsterLegacy(s.monster_id);
        if (!tmpl) continue;
        std::string model = ModelResolver::ResolveMonsterModel(tmpl->model_file);
        for (uint16_t i = 0; i < s.count && monsters_.size() < 24; ++i) {
            float ox = (static_cast<float>(rand() % 200) - 100.0f) * s.spawn_radius / 100.0f;
            float oz = (static_cast<float>(rand() % 200) - 100.0f) * s.spawn_radius / 100.0f;
            uint32_t id = state_->next_entity_id++;
            monsters_.emplace_back(id, tmpl->name, ox, oz, tmpl->level, MonsterType::Normal, model);
            monsters_.back().SetNavMesh(&navmesh_);
        }
    }

    if (monsters_.empty() && map_id == 51) {
        struct DemoSpawn { uint32_t monster_id; float x, z; } demos[] = {
            {798, 18.0f, 22.0f}, {798, 5.0f, -15.0f}, {107, -12.0f, 28.0f},
        };
        for (const auto& d : demos) {
            const MonsterData* tmpl = gamedb_->GetMonsterLegacy(d.monster_id);
            if (!tmpl) continue;
            std::string model = ModelResolver::ResolveMonsterModel(tmpl->model_file);
            uint32_t id = state_->next_entity_id++;
            monsters_.emplace_back(id, tmpl->name, d.x, d.z, tmpl->level, MonsterType::Normal, model);
            monsters_.back().SetNavMesh(&navmesh_);
        }
    }

    navmesh_.ClearObstacles();
    navmesh_.AddObstacle(0.0f, 0.0f, 3.0f);
    for (const auto& m : monsters_)
        navmesh_.AddObstacle(m.GetX(), m.GetZ(), 1.5f);

    spdlog::info("GameScreen: {} monsters spawned for map {}", monsters_.size(), map_id);
}

void GameScreen::SpawnRandomMonster() {
    uint32_t id = state_->next_entity_id++;
    float x = (float)(rand() % 80) - 40, z = (float)(rand() % 80) - 40;
    std::string name = "Slime";
    int level = 1 + (rand() % 20);
    std::string model;
    if (gamedb_) {
        auto all = gamedb_->GetAllMonsters();
        if (!all.empty()) {
            const auto& m = all[rand() % all.size()];
            name = m.name;
            level = m.level;
            model = ModelResolver::ResolveMonsterModel(m.model_file);
        }
    }
    monsters_.emplace_back(id, name, x, z, level, MonsterType::Normal, model);
}

void GameScreen::Render(UIRenderer& ui) {
    RenderUI(ui);
}

void GameScreen::Render(UIRenderer& ui, const glm::mat4& view, const glm::mat4& proj) {
    // 3D world + sky rendered in main.cpp before this call.
    last_view_ = view;
    last_proj_ = proj;
    have_camera_matrices_ = true;
    std::vector<glm::vec3> ppos;
    std::vector<uint32_t> pcol;
    std::vector<float> psiz;
    for (auto& p : particleSys_.GetActiveParticles()) {
        ppos.push_back(p.position);
        pcol.push_back(p.color);
        psiz.push_back(p.size);
    }
    if (particles_) particles_->Render(view, proj, ppos, pcol, psiz);

    RenderUI(ui);
}

void GameScreen::RenderUI(UIRenderer& ui) {
    float lw = ui.logicalWidth;
    float lh = ui.logicalHeight;

    // Weather overlay
    weather_.Render(ui, state_->player_x, state_->player_z);

    legacy_hud_.Render(ui);
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

    // Target frame — Old Luna places it top-center under the hero plate
    if (state_->target_entity >= 0) {
        for (auto& m : monsters_) {
            if ((int32_t)m.GetID() == state_->target_entity) {
                float tf_w = 260, tf_x = (lw - tf_w) * 0.5f, tf_y = 8;
                ui.DrawRect(tf_x, tf_y, tf_w, 46, {20, 14, 14, 210});
                ui.DrawBorder(tf_x, tf_y, tf_w, 46, {200, 90, 90, 180});
                const char* type_tag = m.IsBoss() ? "[Boss] " : "";
                ui.DrawText(tf_x + 8, tf_y + 4, m.IsBoss() ? 0xff5555ff : 0xffffffff,
                            "%s%s  Lv.%d", type_tag, m.GetName().c_str(), m.GetLevel());
                float hp = (float)m.GetHP() / std::max(1, m.GetMaxHP());
                ui.DrawBar(tf_x + 8, tf_y + 24, tf_w - 16, 14, hp, {255, 60, 60, 255}, {60, 0, 0, 180});
                ui.DrawText(tf_x + 12, tf_y + 24, 0xffffffff, "%d/%d", m.GetHP(), m.GetMaxHP());
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
        mmName + ".png");
    
    if (bgfx::isValid(mmTex.handle)) {
        ui.DrawImage(mm_x, mm_y, mm_size, mm_size, mmTex.handle);
    } else {
        ui.DrawRect(mm_x, mm_y, mm_size, mm_size, {20, 30, 50, 220});
        ui.DrawText(mm_x + 4, mm_y + 2, 0x88ffffff, "MINIMAP");
    }
    // Old-style double frame + gold corner ticks and N compass mark
    ui.DrawBorder(mm_x - 1, mm_y - 1, mm_size + 2, mm_size + 2, {10, 12, 24, 220}, 1);
    ui.DrawBorder(mm_x, mm_y, mm_size, mm_size, {100, 180, 255, 170}, 1);
    ui.DrawRect(mm_x, mm_y, 10, 2, {212, 175, 96, 230});
    ui.DrawRect(mm_x, mm_y, 2, 10, {212, 175, 96, 230});
    ui.DrawRect(mm_x + mm_size - 10, mm_y, 10, 2, {212, 175, 96, 230});
    ui.DrawRect(mm_x + mm_size - 2, mm_y, 2, 10, {212, 175, 96, 230});
    ui.DrawRect(mm_x, mm_y + mm_size - 2, 10, 2, {212, 175, 96, 230});
    ui.DrawRect(mm_x, mm_y + mm_size - 10, 2, 10, {212, 175, 96, 230});
    ui.DrawRect(mm_x + mm_size - 10, mm_y + mm_size - 2, 10, 2, {212, 175, 96, 230});
    ui.DrawRect(mm_x + mm_size - 2, mm_y + mm_size - 10, 2, 10, {212, 175, 96, 230});
    ui.DrawText(mm_cx - 4, mm_y + 3, 0xffffd780, "N");
    // Map name label under the minimap
    ui.DrawText(mm_x + 4, mm_y + mm_size + 4, 0xffd0d8ff, "Map %u", state_->map_id ? state_->map_id : 51);
    
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

    // === Click-to-move destination marker (Old Luna ground ring) ===
    if (state_->click_marker_time > 0.0f && have_camera_matrices_) {
        float gy = terrain_ ? terrain_->GetHeight(state_->click_marker_x, state_->click_marker_z) : state_->player_y;
        glm::vec4 clip = last_proj_ * last_view_ *
            glm::vec4(state_->click_marker_x, gy + 0.05f, state_->click_marker_z, 1.0f);
        if (clip.w > 0.0f) {
            float sx = (clip.x / clip.w * 0.5f + 0.5f) * lw;
            float sy = (1.0f - (clip.y / clip.w * 0.5f + 0.5f)) * lh;
            float t = state_->click_marker_time;            // 1 → 0
            float ring = 14.0f + (1.0f - t) * 10.0f;        // expands as it fades
            uint8_t a = (uint8_t)(200 * t);
            ui.DrawBorder(sx - ring * 0.5f, sy - ring * 0.25f, ring, ring * 0.5f,
                          {120, 255, 120, a}, 2.0f);
            ui.DrawRect(sx - 2, sy - 1, 4, 2, {180, 255, 180, a});
        }
    }

    // === Effects (damage numbers projected with the real camera) ===
    if (have_camera_matrices_)
        effect_mgr_.Render(ui, last_view_, last_proj_);
    else
        effect_mgr_.Render(ui, glm::mat4(1), glm::mat4(1));

    if (!legacy_hud_.IsActive())
        chat_panel_.Render(ui, state_);

    if (!legacy_hud_.IsActive()) {
    // === Hotbar (bottom center) with cooldown overlay ===
    {
        float hb_y = lh - 50.0f, hb_s = 40, hb_p = 4;
        float hb_x = (lw - (10 * (hb_s + hb_p))) / 2;
        // Backplate strip behind the slots (Old quickbar plate)
        ui.DrawRect(hb_x - 6, hb_y - 4, 10 * (hb_s + hb_p) + 8, hb_s + 12, {12, 14, 28, 190});
        ui.DrawBorder(hb_x - 6, hb_y - 4, 10 * (hb_s + hb_p) + 8, hb_s + 12, {90, 120, 170, 140});
        for (int s = 0; s < 10; s++) {
            float sx = hb_x + s * (hb_s + hb_p);
            bool has_skill = state_->hotbar_skills[s] != 0;
            ui.DrawRect(sx, hb_y, hb_s, hb_s, has_skill ? UIColor{40, 40, 70, 220} : UIColor{30, 30, 50, 200});
            // Beveled slot frame
            ui.DrawBorder(sx, hb_y, hb_s, hb_s, {16, 20, 36, 220}, 1);
            ui.DrawBorder(sx + 1, hb_y + 1, hb_s - 2, hb_s - 2, {120, 150, 200, 150}, 1);
            if (has_skill) {
                ui.DrawText(sx + 4, hb_y + 4, 0xff88ccff, "%u", state_->hotbar_skills[s]);
            }
            if (state_->hotbar_cooldowns[s] > 0.0f) {
                float pct = std::min(1.0f, state_->hotbar_cooldowns[s] / 2.5f);
                // Bottom-up dark sweep + remaining seconds, like Old cooldown
                ui.DrawRect(sx, hb_y + hb_s * (1.0f - pct), hb_s, hb_s * pct, {0, 0, 0, 170});
                ui.DrawText(sx + 12, hb_y + 12, 0xffffe080, "%.0f", ceilf(state_->hotbar_cooldowns[s]));
            }
            // Keybind number in the corner (Old shows it bottom-right)
            char buf[8]; snprintf(buf, 8, "%d", (s + 1) % 10);
            ui.DrawText(sx + hb_s - 12, hb_y + hb_s - 16, 0xffcccccc, "%s", buf);
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
        if (!m.IsAlive()) continue;
        if (have_camera_matrices_) m.RenderOverhead(ui, &last_view_, &last_proj_);
        else m.RenderOverhead(ui);
    }
    pet_.RenderOverhead(ui);

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
            "TIP: Plant seeds with [H] then [1-5] on matching plots!",
        };
        float ly = 88;
        for (auto* line : lines) {
            ui.DrawText(220, ly, 0xffcccccc, "%s", line);
            ly += 18;
        }
        ui.DrawText(220, ly + 10, 0xff888888, "[F1] Close");
    }

    // === Farm Panel / Dialog ===
    if (state_->farm_open) {
        if (farm_get_dlg_.GetWindow()) {
            farm_get_dlg_.Refresh(state_);
            farm_get_dlg_.GetWindow()->Render(ui);
        } else {
            // Fallback inline rendering when script dialog unavailable
            float fx = 80, fy = 80, fw = 420, fh = 320;
            ui.DrawRect(fx, fy, fw, fh, {20, 40, 20, 235});
            ui.DrawBorder(fx, fy, fw, fh, {100, 200, 100, 200});
            ui.DrawText(fx + 8, fy + 6, 0xaaffaa, "FARM  (server sync)");
            float ly = fy + 28;
            ui.DrawText(fx + 8, ly, 0xffccffcc, "Seeds — press [1-5] to plant on matching plot:");
            ly += 18;
            if (!state_->network_farm_seeds.empty()) {
                for (size_t i = 0; i < state_->network_farm_seeds.size(); ++i) {
                    const auto& s = state_->network_farm_seeds[i];
                    ui.DrawText(fx + 12, ly, 0xffffffff, "[%zu] %s  (%us grow)",
                        i + 1, s.name.c_str(), s.growth_time_sec);
                    ly += 16;
                }
            } else {
                auto seeds = farm_.GetAllSeeds();
                for (size_t i = 0; i < seeds.size(); ++i) {
                    ui.DrawText(fx + 12, ly, 0xffffffff, "[%zu] %s  (%ds)", i + 1,
                        seeds[i].name.c_str(), seeds[i].growth_time);
                    ly += 16;
                }
            }
            ly += 4;
            ui.DrawText(fx + 8, ly, 0xffaaaaaa, "[W] Water   [R] Harvest   [H] Close");
            ly += 20;
            for (int p = 0; p < 9; p++) {
                float px = fx + 12 + (p % 3) * 132;
                float py = ly + (p / 3) * 58;
                const GameState::NetworkFarmPlot* np = (p < (int)state_->network_farm_plots.size())
                    ? &state_->network_farm_plots[p] : nullptr;
                auto* plot = farm_.GetPlot(p);
                bool empty = !plot || plot->seed_id <= 0;
                uint32_t col = empty ? 0xff2a3a2a : (np && np->ready ? 0xffddaa22 : 0xff44aa44);
                ui.DrawRect(px, py, 120, 48, {(uint8_t)(col & 0xff), (uint8_t)((col >> 8) & 0xff),
                    (uint8_t)((col >> 16) & 0xff), 220});
                ui.DrawBorder(px, py, 120, 48, {120, 200, 120, 180});
                char title[64];
                snprintf(title, sizeof(title), "Plot %d", p + 1);
                ui.DrawText(px + 4, py + 2, 0xffffffff, "%s", title);
                if (!empty && plot) {
                    ui.DrawText(px + 4, py + 14, 0xffeeeeee, "%s", plot->plant_name.c_str());
                    uint8_t pct = np ? np->growth_pct : 0;
                    if (plot->max_stages > 0 && pct == 0)
                        pct = static_cast<uint8_t>((plot->growth_stage * 100) / plot->max_stages);
                    ui.DrawRect(px + 4, py + 28, 112, 8, {30, 30, 30, 200});
                    ui.DrawRect(px + 4, py + 28, 112.0f * pct / 100.0f, 8, {80, 180, 80, 255});
                    char stg[32];
                    snprintf(stg, sizeof(stg), "%u%% S%d/%d", pct, plot->growth_stage, plot->max_stages);
                    ui.DrawText(px + 4, py + 38, 0xffcccccc, "%s%s", stg,
                        plot->watered ? " ~" : (np && np->ready ? " READY" : ""));
                } else {
                    ui.DrawText(px + 4, py + 20, 0xff888888, "Empty");
                }
            }
        }
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
        party_dlg_.UpdateFromState(state_);
        party_dlg_.GetWindow()->Render(ui);
    }
    if (state_->guild_open && guild_dlg_.GetWindow()) {
        guild_dlg_.UpdateFromState(state_);
        guild_dlg_.GetWindow()->Render(ui);
    }
    if (state_->friend_open && friend_dlg_.GetWindow()) {
        friend_dlg_.UpdateFromState(state_);
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
    
    minimap_dlg_.Render(ui);
    if (pk_dlg_.IsOpen()) pk_dlg_.Render(ui);
    if (helper_dlg_.IsOpen()) helper_dlg_.Render(ui);
    if (worldmap_dlg_.IsOpen()) worldmap_dlg_.Render(ui);
    fade_dlg_.Render(ui);

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
    
    script_dialogs_.Render(ui, state_);
    wm_.Render(ui);
}

void GameScreen::InitScriptDialogs() {
    auto reg = [this](const char* key, const char* path, const char* title,
                      float x, float y, float w, float h, bool* flag = nullptr) {
        script_dialogs_.Register(key, {path, title, x, y, w, h, flag});
    };

    reg("revival", "assets/interface/Windows/Revival.bin.txt", "Revive", 280, 180, 360, 300, &state_->revival_open);
    reg("mix", "assets/interface/Windows/MixDialog.bin.txt", "Item Mix", 200, 100, 420, 380, &state_->mix_open);
    reg("party_invite", "assets/interface/Windows/PartyInvite.bin.txt", "Party Invite", 320, 160, 340, 260, &state_->party_invite_open);
    reg("guild_notice", "assets/interface/Windows/GuildNotice.bin.txt", "Guild Notice", 300, 120, 400, 320, &state_->guild_notice_open);
    reg("stall_sell", "assets/interface/Windows/StallSell.bin.txt", "Street Stall", 200, 80, 480, 400, &state_->stall_sell_open);
    reg("siege_flag", "assets/interface/Windows/SiegeWarFlagDlg.bin.txt", "Siege Flag", 350, 150, 380, 300, &state_->siege_flag_open);
    reg("item_shop", "assets/interface/Windows/ItemmallBtnDlg.bin.txt", "Item Mall", 120, 60, 520, 480, &state_->cashshop_open);

    reg("bank", "assets/interface/Windows/Bank.bin.txt", "Bank", 220, 120, 420, 340);
    reg("bigmap", "assets/interface/Windows/BigMap.bin.txt", "Big Map", 80, 60, 640, 480);
    reg("change_class", "assets/interface/Windows/ChangeClass.bin.txt", "Change Class", 260, 140, 400, 320);
    reg("char_select", "assets/interface/Windows/CharSelect.bin.txt", "Character Select", 200, 100, 500, 400);
    reg("enchant", "assets/interface/Windows/EnchantDialog.bin.txt", "Enchant", 240, 120, 420, 360);
    reg("family_create", "assets/interface/Windows/FamilyCreate.bin.txt", "Create Family", 300, 150, 380, 280);
    reg("family_invite", "assets/interface/Windows/FamilyInvite.bin.txt", "Family Invite", 320, 160, 360, 260);
    reg("farm_manage", "assets/interface/Windows/FarmManage.bin.txt", "Farm", 200, 100, 480, 400);
    reg("dissolve", "assets/interface/Windows/DissolveDialog.bin.txt", "Dissolve", 280, 160, 400, 300);
    reg("divide", "assets/interface/Windows/DivideBox.bin.txt", "Divide Item", 300, 180, 360, 240);
    reg("store_search", "assets/interface/Windows/StoreSearchDlg.bin.txt", "Store Search", 200, 80, 500, 420);
    reg("challenge_zone", "assets/interface/Windows/ChallengeZoneListDlg.bin.txt", "Challenge Zone", 180, 100, 520, 400);
    reg("stall_buy", "assets/interface/Windows/StallBuy.bin.txt", "Stall Buy", 200, 100, 480, 380);
    reg("party_matching", "assets/interface/Windows/PartyMatchingDlg.bin.txt", "Party Matching", 260, 120, 420, 340);
    reg("channel", "assets/interface/Windows/Channel.bin.txt", "Channel", 300, 140, 380, 280);
    reg("char_profile", "assets/interface/Windows/CharMakeProfile.bin.txt", "Profile", 240, 120, 440, 360);
    reg("fishing_point", "assets/interface/Windows/FishingPointDlg.bin.txt", "Fishing Point", 280, 160, 400, 300);
    reg("guild_warehouse", "assets/interface/Windows/GuildWarehouse.bin.txt", "Guild Warehouse", 200, 100, 480, 400);
    reg("party_war", "assets/interface/Windows/PartyWarDlg.bin.txt", "Party War", 260, 120, 420, 340);
    reg("ally_note", "assets/interface/Windows/AllyNote.bin.txt", "Ally Note", 300, 140, 380, 300);
    reg("auto_note", "assets/interface/Windows/AutoNoteDlg.bin.txt", "Auto Note", 300, 140, 380, 300);
    reg("additional_btn", "assets/interface/Windows/AdditionalButtonDlg.bin.txt", "Additional", 400, 200, 300, 200);
    reg("change_name", "assets/interface/Windows/ChangeNameDlg.bin.txt", "Change Name", 300, 160, 380, 260);
    reg("changejob", "assets/interface/Windows/Changejob.bin.txt", "Change Job", 260, 140, 400, 320);
    reg("consignment_guide", "assets/interface/Windows/Consignment_Guide.bin.txt", "Auction Guide", 200, 100, 480, 400);
    reg("farm_get", "assets/interface/Windows/Farm_Get.bin.txt", "Farm Harvest", 300, 160, 380, 280);
    reg("farm_upgrade", "assets/interface/Windows/Farm_Upgrade.bin.txt", "Farm Upgrade", 300, 160, 380, 280);
    reg("favor_icon", "assets/interface/Windows/FavorIconDlg.bin.txt", "Favor", 320, 180, 360, 260);
    reg("date_matching", "assets/interface/Windows/DateMatchingDlg.bin.txt", "Date Matching", 240, 120, 440, 360);
    reg("date_zone", "assets/interface/Windows/DateZoneListDlg.bin.txt", "Date Zone", 240, 120, 440, 360);
}

void GameScreen::SetupQuestNetworkCallbacks() {
    if (!network_ || !network_->IsConnected() || state_->offline_mode) {
        quest_dlg_.SetNetworkCallbacks({}, {}, {});
        return;
    }
    quest_dlg_.SetNetworkCallbacks(
        [this](uint32_t qid) { SendQuestStart(qid); },
        [this](uint32_t qid) { SendQuestComplete(qid); },
        [this]() { RequestQuestList(); });
}

void GameScreen::SetupDungeonNetworkCallbacks() {
    if (!network_ || !network_->IsConnected() || state_->offline_mode) {
        dungeon_dlg_.SetNetworkCallbacks({}, {});
        return;
    }
    dungeon_dlg_.SetNetworkCallbacks(
        [this](uint32_t tid) { SendDungeonEntrance(tid); },
        [this](uint32_t iid) { RequestDungeonInfo(iid); });
}

void GameScreen::RequestQuestList() {
    if (!network_ || !network_->IsConnected() || state_->offline_mode) return;
    flatbuffers::FlatBufferBuilder fbb;
    auto req = luna::protocol::CreateQuestListRequest(fbb, GetSelectedCharId());
    fbb.Finish(req);
    network_->SendPacket(luna::protocol::PacketType_MP_QUEST_LIST_SYN,
        fbb.GetBufferPointer(), fbb.GetSize());
}

void GameScreen::SendQuestStart(uint32_t quest_id, uint32_t npc_id) {
    if (!network_ || !network_->IsConnected() || state_->offline_mode) return;
    flatbuffers::FlatBufferBuilder fbb;
    auto req = luna::protocol::CreateQuestStartRequest(fbb, GetSelectedCharId(), quest_id, npc_id);
    fbb.Finish(req);
    network_->SendPacket(luna::protocol::PacketType_MP_QUEST_START_SYN,
        fbb.GetBufferPointer(), fbb.GetSize());
}

void GameScreen::SendQuestComplete(uint32_t quest_id) {
    if (!network_ || !network_->IsConnected() || state_->offline_mode) return;
    flatbuffers::FlatBufferBuilder fbb;
    auto req = luna::protocol::CreateQuestCompleteRequest(fbb, GetSelectedCharId(), quest_id);
    fbb.Finish(req);
    network_->SendPacket(luna::protocol::PacketType_MP_QUEST_END_SYN,
        fbb.GetBufferPointer(), fbb.GetSize());
}

void GameScreen::SendDungeonEntrance(uint32_t template_id) {
    if (!network_ || !network_->IsConnected() || state_->offline_mode) return;
    flatbuffers::FlatBufferBuilder fbb;
    auto req = luna::protocol::CreateDungeonEntranceRequest(
        fbb, GetSelectedCharId(), template_id, state_->party_id);
    fbb.Finish(req);
    network_->SendPacket(luna::protocol::PacketType_MP_DUNGEON_ENTRANCE_SYN,
        fbb.GetBufferPointer(), fbb.GetSize());
}

void GameScreen::RequestDungeonInfo(uint32_t instance_id) {
    if (!network_ || !network_->IsConnected() || state_->offline_mode) return;
    flatbuffers::FlatBufferBuilder fbb;
    auto req = luna::protocol::CreateDungeonInfoRequest(fbb, GetSelectedCharId(), instance_id);
    fbb.Finish(req);
    network_->SendPacket(luna::protocol::PacketType_MP_DUNGEON_INFO_SYN,
        fbb.GetBufferPointer(), fbb.GetSize());
}

void GameScreen::ApplyQuestListResponse(const luna::protocol::QuestListResponse* resp) {
    if (!resp) return;
    state_->network_quests.clear();
    state_->completed_quest_ids.clear();
    if (resp->active()) {
        for (auto q : *resp->active()) {
            GameState::NetworkQuestEntry entry;
            entry.quest_id = q->quest_id();
            entry.name = q->name() ? q->name()->str() : "Quest";
            entry.is_completed = q->is_completed();
            entry.is_reward_taken = q->is_reward_taken();
            if (q->objectives()) {
                for (auto o : *q->objectives()) {
                    GameState::NetworkQuestObjective obj;
                    obj.type = o->objective_type();
                    obj.target_id = o->target_id();
                    obj.current = o->current_count();
                    obj.required = o->required_count();
                    entry.objectives.push_back(obj);
                }
            }
            state_->network_quests.push_back(std::move(entry));
        }
    }
    if (resp->completed_ids()) {
        for (auto cid : *resp->completed_ids())
            state_->completed_quest_ids.push_back(cid);
    }
    if (state_->quest_open)
        quest_dlg_.UpdateFromState(state_);
}

void GameScreen::ApplyQuestStartResponse(const luna::protocol::QuestStartResponse* resp) {
    if (!resp) return;
    if (resp->result() != 0) {
        std::string msg = resp->message() ? resp->message()->str() : "Cannot start quest";
        state_->chat_messages.push_back(msg);
    } else {
        state_->chat_messages.push_back("Quest started (#" + std::to_string(resp->quest_id()) + ")");
        RequestQuestList();
    }
    if (state_->chat_messages.size() > 50) state_->chat_messages.erase(state_->chat_messages.begin());
}

void GameScreen::ApplyQuestCompleteResponse(const luna::protocol::QuestCompleteResponse* resp) {
    if (!resp) return;
    if (resp->result() != 0) {
        state_->chat_messages.push_back("Quest turn-in failed (#" + std::to_string(resp->quest_id()) + ")");
    } else {
        state_->chat_messages.push_back("Quest completed! Rewards granted.");
        RequestQuestList();
    }
    if (state_->chat_messages.size() > 50) state_->chat_messages.erase(state_->chat_messages.begin());
}

void GameScreen::ApplyQuestUpdateNotify(const luna::protocol::QuestUpdateNotify* resp) {
    if (!resp) return;
    for (auto& q : state_->network_quests) {
        if (q.quest_id != resp->quest_id()) continue;
        if (resp->objective_index() < q.objectives.size()) {
            auto& obj = q.objectives[resp->objective_index()];
            obj.current = resp->current_count();
            obj.required = resp->required_count();
        }
        char buf[128];
        snprintf(buf, sizeof(buf), "Quest progress: %u/%u", resp->current_count(), resp->required_count());
        state_->chat_messages.push_back(buf);
        break;
    }
    if (state_->quest_open)
        quest_dlg_.UpdateFromState(state_);
    if (state_->chat_messages.size() > 50) state_->chat_messages.erase(state_->chat_messages.begin());
}

void GameScreen::ApplyDungeonEntranceResponse(const luna::protocol::DungeonEntranceResponse* resp) {
    if (!resp) return;
    if (resp->result() != 0) {
        std::string msg = resp->message() ? resp->message()->str() : "Dungeon entrance denied";
        state_->chat_messages.push_back(msg);
        return;
    }
    state_->dungeon_instance_id = resp->instance_id();
    state_->dungeon_template_id = resp->dungeon_template_id();
    state_->in_dungeon = true;
    state_->dungeon_map = resp->map_id();
    dungeon_sys_.ApplyEntranceResponse(resp->instance_id(), resp->dungeon_template_id(),
        resp->map_id(), resp->time_limit_sec());
    state_->chat_messages.push_back("Entered dungeon instance #" + std::to_string(resp->instance_id()));
    if (state_->dungeon_open)
        dungeon_dlg_.UpdateFromState(state_);
}

void GameScreen::ApplyDungeonInfoResponse(const luna::protocol::DungeonInfoResponse* resp) {
    if (!resp || resp->result() != 0) return;
    state_->dungeon_instance_id = resp->instance_id();
    state_->dungeon_state = resp->state();
    state_->dungeon_elapsed_sec = resp->elapsed_sec();
    dungeon_sys_.ApplyInfoResponse(resp->instance_id(), resp->state(),
        resp->elapsed_sec(), resp->boss_active());
    if (state_->dungeon_open)
        dungeon_dlg_.UpdateFromState(state_);
}

void GameScreen::ApplyTriggerNotify(const luna::protocol::TriggerNotify* resp) {
    if (!resp) return;
    std::string msg = resp->message() ? resp->message()->str() : "";
    if (msg.empty())
        msg = "Trigger #" + std::to_string(resp->trigger_id()) + " activated";
    state_->chat_messages.push_back("[Trigger] " + msg);
    if (state_->chat_messages.size() > 50) state_->chat_messages.erase(state_->chat_messages.begin());
}

void GameScreen::SetupSecondaryNetworkCallbacks() {
    if (!network_ || !network_->IsConnected() || state_->offline_mode) {
        family_dlg_.SetNetworkCallbacks({}, {}, {}, {}, {}, {}, {});
        pet_dlg_.SetNetworkCallbacks({}, {}, {}, {}, {});
        fishing_dlg_.SetCastCallback({});
        cash_shop_dlg_.SetNetworkCallbacks({}, {});
        guild_dlg_.SetSiegeNetworkCallbacks({}, {});
        tournament_dlg_.SetNetworkCallbacks({}, {}, {}, {});
        return;
    }
    guild_dlg_.SetSiegeNetworkCallbacks(
        [this](uint8_t action, uint32_t tid, uint32_t param) {
            SendSiegeAction(action, tid, param);
        },
        [this]() { RequestSiegeInfo(); });
    family_dlg_.SetNetworkCallbacks(
        [this](const std::string& name) { SendFamilyCreate(name); },
        [this](uint32_t id, const std::string& nm) { SendFamilyPropose(id, nm); },
        [this]() { SendFamilyAccept(); },
        [this]() { SendFamilyAction(1); },
        [this]() { SendFamilyAction(0); },
        [this]() { SendFamilyAction(2); },
        [this]() { RequestFamilyInfo(); });
    pet_dlg_.SetNetworkCallbacks(
        [this]() { SendPetAction(0); },
        [this]() { SendPetAction(1); },
        [this]() { SendPetAction(2); },
        [this]() { SendPetAction(3); },
        [this]() { RequestPetInfo(); });
    fishing_dlg_.SetCastCallback([this]() { SendFishingCast(0); });
    cash_shop_dlg_.SetNetworkCallbacks(
        [this](uint32_t item_id) { SendCashShopBuy(item_id); },
        [this]() { RequestCashShopList(); });
    housing_dlg_.SetNetworkCallbacks(
        [this](uint8_t template_id) { SendHousingAction(0, 0, template_id); },
        [this](uint32_t house_id) { SendHousingAction(1, house_id); },
        [this](uint32_t house_id, uint32_t item_id, float x, float y) {
            SendHousingAction(2, house_id, 0, item_id, x, y);
        },
        [this]() { RequestHousingInfo(); });
    tournament_dlg_.SetNetworkCallbacks(
        [this](uint32_t tid) { SendTournamentRegister(tid); },
        [this](uint32_t tid) { SendTournamentAction(0, tid); },
        [this](uint32_t tid) { SendTournamentAction(1, tid); },
        [this]() { RequestTournamentList(); });
}

void GameScreen::RequestFamilyInfo() {
    if (!network_ || !network_->IsConnected() || state_->offline_mode) return;
    flatbuffers::FlatBufferBuilder fbb;
    auto req = luna::protocol::CreateFamilyInfoRequest(fbb, GetSelectedCharId());
    fbb.Finish(req);
    network_->SendPacket(luna::protocol::PacketType_MP_FAMILY_INFO_SYN,
        fbb.GetBufferPointer(), fbb.GetSize());
}

void GameScreen::SendFamilyCreate(const std::string& name) {
    if (!network_ || !network_->IsConnected() || state_->offline_mode) return;
    flatbuffers::FlatBufferBuilder fbb;
    auto req = luna::protocol::CreateFamilyCreateRequestDirect(
        fbb, GetSelectedCharId(), name.c_str());
    fbb.Finish(req);
    network_->SendPacket(luna::protocol::PacketType_MP_FAMILY_CREATE_SYN,
        fbb.GetBufferPointer(), fbb.GetSize());
}

void GameScreen::SendFamilyPropose(uint32_t target_id, const std::string& target_name) {
    if (!network_ || !network_->IsConnected() || state_->offline_mode) return;
    flatbuffers::FlatBufferBuilder fbb;
    auto req = luna::protocol::CreateFamilyProposeRequestDirect(
        fbb, GetSelectedCharId(), target_id, target_name.c_str());
    fbb.Finish(req);
    network_->SendPacket(luna::protocol::PacketType_MP_FAMILY_PROPOSE_SYN,
        fbb.GetBufferPointer(), fbb.GetSize());
}

void GameScreen::SendFamilyAccept() {
    if (!network_ || !network_->IsConnected() || state_->offline_mode) return;
    flatbuffers::FlatBufferBuilder fbb;
    auto req = luna::protocol::CreateFamilyAcceptRequest(fbb, GetSelectedCharId());
    fbb.Finish(req);
    network_->SendPacket(luna::protocol::PacketType_MP_FAMILY_ACCEPT_SYN,
        fbb.GetBufferPointer(), fbb.GetSize());
}

void GameScreen::SendFamilyAction(uint8_t action) {
    if (!network_ || !network_->IsConnected() || state_->offline_mode) return;
    flatbuffers::FlatBufferBuilder fbb;
    auto req = luna::protocol::CreateFamilyActionRequest(fbb, GetSelectedCharId(), action);
    fbb.Finish(req);
    network_->SendPacket(luna::protocol::PacketType_MP_FAMILY_ACTION_SYN,
        fbb.GetBufferPointer(), fbb.GetSize());
}

void GameScreen::RequestPetInfo() {
    if (!network_ || !network_->IsConnected() || state_->offline_mode) return;
    flatbuffers::FlatBufferBuilder fbb;
    auto req = luna::protocol::CreatePetInfoRequest(fbb, GetSelectedCharId());
    fbb.Finish(req);
    network_->SendPacket(luna::protocol::PacketType_MP_PET_INFO_SYN,
        fbb.GetBufferPointer(), fbb.GetSize());
}

void GameScreen::SendPetAction(uint8_t action) {
    if (!network_ || !network_->IsConnected() || state_->offline_mode) return;
    flatbuffers::FlatBufferBuilder fbb;
    auto req = luna::protocol::CreatePetActionRequest(fbb, GetSelectedCharId(), action);
    fbb.Finish(req);
    network_->SendPacket(luna::protocol::PacketType_MP_PET_ACTION_SYN,
        fbb.GetBufferPointer(), fbb.GetSize());
}

void GameScreen::SendFishingCast(uint32_t spot_id) {
    if (!network_ || !network_->IsConnected() || state_->offline_mode) return;
    flatbuffers::FlatBufferBuilder fbb;
    auto req = luna::protocol::CreateFishingCastRequest(fbb, GetSelectedCharId(), spot_id);
    fbb.Finish(req);
    network_->SendPacket(luna::protocol::PacketType_MP_FISHING_CAST_SYN,
        fbb.GetBufferPointer(), fbb.GetSize());
}

void GameScreen::RequestSiegeInfo() {
    if (!network_ || !network_->IsConnected() || state_->offline_mode) return;
    flatbuffers::FlatBufferBuilder fbb;
    auto req = luna::protocol::CreateSiegeInfoRequest(fbb, GetSelectedCharId());
    fbb.Finish(req);
    network_->SendPacket(luna::protocol::PacketType_MP_SIEGE_INFO_SYN,
        fbb.GetBufferPointer(), fbb.GetSize());
}

void GameScreen::SendSiegeAction(uint8_t action, uint32_t territory_id, uint32_t param) {
    if (!network_ || !network_->IsConnected() || state_->offline_mode) return;
    flatbuffers::FlatBufferBuilder fbb;
    auto req = luna::protocol::CreateSiegeActionRequest(
        fbb, GetSelectedCharId(), action, territory_id, param);
    fbb.Finish(req);
    network_->SendPacket(luna::protocol::PacketType_MP_SIEGE_ACTION_SYN,
        fbb.GetBufferPointer(), fbb.GetSize());
}

void GameScreen::RequestTournamentList() {
    if (!network_ || !network_->IsConnected() || state_->offline_mode) return;
    flatbuffers::FlatBufferBuilder fbb;
    auto req = luna::protocol::CreateTournamentListRequest(fbb, GetSelectedCharId());
    fbb.Finish(req);
    network_->SendPacket(luna::protocol::PacketType_MP_TOURNAMENT_LIST_SYN,
        fbb.GetBufferPointer(), fbb.GetSize());
}

void GameScreen::SendTournamentRegister(uint32_t tournament_id) {
    if (!network_ || !network_->IsConnected() || state_->offline_mode) return;
    flatbuffers::FlatBufferBuilder fbb;
    auto req = luna::protocol::CreateTournamentRegisterRequest(
        fbb, GetSelectedCharId(), tournament_id, state_->guild_id);
    fbb.Finish(req);
    network_->SendPacket(luna::protocol::PacketType_MP_TOURNAMENT_REGISTER_SYN,
        fbb.GetBufferPointer(), fbb.GetSize());
}

void GameScreen::SendTournamentAction(uint8_t action, uint32_t tournament_id) {
    if (!network_ || !network_->IsConnected() || state_->offline_mode) return;
    flatbuffers::FlatBufferBuilder fbb;
    auto req = luna::protocol::CreateTournamentActionRequest(
        fbb, GetSelectedCharId(), action, tournament_id);
    fbb.Finish(req);
    network_->SendPacket(luna::protocol::PacketType_MP_TOURNAMENT_ACTION_SYN,
        fbb.GetBufferPointer(), fbb.GetSize());
}

void GameScreen::RequestHousingInfo() {
    if (!network_ || !network_->IsConnected() || state_->offline_mode) return;
    flatbuffers::FlatBufferBuilder fbb;
    auto req = luna::protocol::CreateHousingInfoRequest(fbb, GetSelectedCharId());
    fbb.Finish(req);
    network_->SendPacket(luna::protocol::PacketType_MP_HOUSING_INFO_SYN,
        fbb.GetBufferPointer(), fbb.GetSize());
}

void GameScreen::RequestCashShopList() {
    if (!network_ || !network_->IsConnected() || state_->offline_mode) return;
    flatbuffers::FlatBufferBuilder fbb;
    auto req = luna::protocol::CreateCashShopListRequest(fbb, GetSelectedCharId());
    fbb.Finish(req);
    network_->SendPacket(luna::protocol::PacketType_MP_CASHSHOP_LIST_SYN,
        fbb.GetBufferPointer(), fbb.GetSize());
}

void GameScreen::SendCashShopBuy(uint32_t item_id) {
    if (!network_ || !network_->IsConnected() || state_->offline_mode) return;
    flatbuffers::FlatBufferBuilder fbb;
    auto req = luna::protocol::CreateCashShopBuyRequest(fbb, GetSelectedCharId(), item_id);
    fbb.Finish(req);
    network_->SendPacket(luna::protocol::PacketType_MP_CASHSHOP_BUY_SYN,
        fbb.GetBufferPointer(), fbb.GetSize());
}

void GameScreen::SyncFamilyFromNetwork() {
    if (state_->network_family_members.empty()) return;
    std::vector<NetworkFamilyMemberView> views;
    views.reserve(state_->network_family_members.size());
    for (const auto& m : state_->network_family_members) {
        NetworkFamilyMemberView v;
        v.character_id = m.character_id;
        v.name = m.name;
        v.relation = m.relation;
        v.partner_id = m.partner_id;
        v.partner_name = m.partner_name;
        v.married_date = m.married_date;
        v.is_master = m.is_master;
        views.push_back(std::move(v));
    }
    family_.SyncFromNetwork(GetSelectedCharId(), state_->network_family_id,
        state_->network_family_name, state_->network_family_master_id, views);
}

void GameScreen::SyncPetFromNetwork() {
    const auto& p = state_->network_pet;
    if (p.pet_id == 0 && p.name.empty()) return;
    pet_.Init(p.name.empty() ? "Fluffy" : p.name, p.pet_id ? p.pet_id : 1);
    pet_.ApplyNetworkState(p.level, p.hp, p.max_hp, p.satiation, p.evolution, p.exp, p.exp_to_next);
    if (p.summoned) pet_.Summon();
    else pet_.Dismiss();
}

void GameScreen::ApplyFamilyResponse(const luna::protocol::FamilyResponse* resp) {
    if (!resp) return;
    if (resp->message()) {
        std::string msg = resp->message()->str();
        if (!msg.empty()) state_->chat_messages.push_back(msg);
    }
    if (resp->family()) {
        state_->network_family_id = resp->family()->family_id();
        state_->network_family_name = resp->family()->family_name()
            ? resp->family()->family_name()->str() : "";
        state_->network_family_master_id = resp->family()->master_id();
        state_->network_can_accept_marriage = resp->family()->can_accept_marriage();
        state_->network_can_reject_proposal = resp->family()->can_reject_proposal();
        state_->network_can_divorce = resp->family()->can_divorce();
        state_->network_can_leave_family = resp->family()->can_leave_family();
        state_->network_can_create_family = resp->family()->can_create_family();
        state_->network_engaged_partner_id = resp->family()->engaged_partner_id();
        state_->network_engaged_partner_name = resp->family()->engaged_partner_name()
            ? resp->family()->engaged_partner_name()->str() : "";
        state_->network_family_members.clear();
        if (resp->family()->members()) {
            for (auto m : *resp->family()->members()) {
                GameState::NetworkFamilyMember entry;
                entry.character_id = m->character_id();
                entry.name = m->name() ? m->name()->str() : "";
                entry.relation = m->relation();
                entry.partner_id = m->partner_id();
                entry.partner_name = m->partner_name() ? m->partner_name()->str() : "";
                entry.married_date = m->married_date();
                entry.is_master = m->is_master();
                state_->network_family_members.push_back(std::move(entry));
            }
        }
        SyncFamilyFromNetwork();
    }
    if (state_->family_open) family_dlg_.UpdateFromState(state_);
    if (state_->chat_messages.size() > 50) state_->chat_messages.erase(state_->chat_messages.begin());
}

void GameScreen::ApplyPetResponse(const luna::protocol::PetResponse* resp) {
    if (!resp) return;
    if (resp->message()) {
        std::string msg = resp->message()->str();
        if (!msg.empty()) state_->chat_messages.push_back(msg);
    }
    if (resp->pet()) {
        state_->network_pet.pet_id = resp->pet()->pet_id();
        state_->network_pet.template_id = resp->pet()->template_id();
        state_->network_pet.name = resp->pet()->name() ? resp->pet()->name()->str() : "Fluffy";
        state_->network_pet.level = resp->pet()->level();
        state_->network_pet.hp = resp->pet()->hp();
        state_->network_pet.max_hp = resp->pet()->max_hp();
        state_->network_pet.satiation = resp->pet()->satiation();
        state_->network_pet.summoned = resp->pet()->summoned();
        state_->network_pet.evolution = resp->pet()->evolution();
        state_->network_pet.exp = resp->pet()->exp();
        state_->network_pet.exp_to_next = resp->pet()->exp_to_next();
        state_->network_pet.feed_cost = resp->pet()->feed_cost();
        state_->network_pet.evolve_cost = resp->pet()->evolve_cost();
        state_->network_pet.can_summon = resp->pet()->can_summon();
        SyncPetFromNetwork();
    }
    if (state_->pet_open) pet_dlg_.UpdateFromState(state_);
    if (state_->chat_messages.size() > 50) state_->chat_messages.erase(state_->chat_messages.begin());
}

void GameScreen::ApplyFishingResponse(const luna::protocol::FishingCastResponse* resp) {
    if (!resp) return;
    std::string msg = resp->message() ? resp->message()->str() : "";
    if (resp->result() == 0) {
        if (msg.empty()) msg = "Caught " + (resp->fish_name() ? resp->fish_name()->str() : "a fish") + "!";
        state_->chat_messages.push_back(msg);
        fishing_dlg_.Close();
        state_->fishing_open = false;
    } else {
        state_->chat_messages.push_back(msg.empty() ? "Fishing failed" : msg);
    }
    if (state_->chat_messages.size() > 50) state_->chat_messages.erase(state_->chat_messages.begin());
}

void GameScreen::ApplySiegeState(const luna::protocol::SiegeInfoResponse* resp) {
    if (!resp) return;
    state_->network_siege_player_guild_id = resp->player_guild_id();
    state_->network_siege_player_guild_name = resp->player_guild_name()
        ? resp->player_guild_name()->str() : "";
    state_->network_can_declare_siege = resp->can_declare_siege();
    state_->network_can_set_tax = resp->can_set_tax();
    state_->network_siege_territories.clear();
    state_->network_siege_schedules.clear();
    if (resp->territories()) {
        for (auto t : *resp->territories()) {
            GameState::NetworkSiegeTerritory entry;
            entry.territory_id = t->territory_id();
            entry.name = t->name() ? t->name()->str() : "";
            entry.owner_guild_id = t->owner_guild_id();
            entry.owner_guild_name = t->owner_guild_name() ? t->owner_guild_name()->str() : "";
            entry.tax_rate = t->tax_rate();
            entry.siege_time = t->siege_time();
            entry.is_castle = t->is_castle();
            entry.defense_bonus = t->defense_bonus();
            entry.attacker_guild_id = t->attacker_guild_id();
            entry.attacker_guild_name = t->attacker_guild_name() ? t->attacker_guild_name()->str() : "";
            entry.seconds_until_siege = t->seconds_until_siege();
            entry.tax_accumulated = t->tax_accumulated();
            entry.owned_by_player_guild = t->owned_by_player_guild();
            entry.can_attack = t->can_attack();
            entry.can_manage_tax = t->can_manage_tax();
            state_->network_siege_territories.push_back(std::move(entry));
        }
    }
    if (resp->schedules()) {
        for (auto s : *resp->schedules()) {
            GameState::NetworkSiegeSchedule entry;
            entry.territory_id = s->territory_id();
            entry.territory_name = s->territory_name() ? s->territory_name()->str() : "";
            entry.attacker_guild_id = s->attacker_guild_id();
            entry.attacker_guild_name = s->attacker_guild_name() ? s->attacker_guild_name()->str() : "";
            entry.defender_guild_id = s->defender_guild_id();
            entry.defender_guild_name = s->defender_guild_name() ? s->defender_guild_name()->str() : "";
            entry.siege_time = s->siege_time();
            entry.seconds_until = s->seconds_until();
            state_->network_siege_schedules.push_back(std::move(entry));
        }
    }
    SyncSiegeFromNetwork();
    if (state_->guild_open) guild_dlg_.UpdateFromState(state_);
}

void GameScreen::SyncSiegeFromNetwork() {
    if (state_->network_siege_territories.empty()) return;
    std::vector<NetworkSiegeTerritoryView> terr;
    std::vector<NetworkSiegeScheduleView> sched;
    terr.reserve(state_->network_siege_territories.size());
    for (const auto& t : state_->network_siege_territories) {
        NetworkSiegeTerritoryView v;
        v.territory_id = t.territory_id;
        v.name = t.name;
        v.owner_guild_id = t.owner_guild_id;
        v.owner_guild_name = t.owner_guild_name;
        v.tax_rate = t.tax_rate;
        v.is_castle = t.is_castle;
        v.defense_bonus = t.defense_bonus;
        v.attacker_guild_id = t.attacker_guild_id;
        v.attacker_guild_name = t.attacker_guild_name;
        v.seconds_until_siege = t.seconds_until_siege;
        v.tax_accumulated = t.tax_accumulated;
        terr.push_back(std::move(v));
    }
    for (const auto& s : state_->network_siege_schedules) {
        NetworkSiegeScheduleView v;
        v.territory_id = s.territory_id;
        v.territory_name = s.territory_name;
        v.attacker_guild_id = s.attacker_guild_id;
        v.attacker_guild_name = s.attacker_guild_name;
        v.defender_guild_id = s.defender_guild_id;
        v.defender_guild_name = s.defender_guild_name;
        v.seconds_until = s.seconds_until;
        sched.push_back(std::move(v));
    }
    siege_.SyncFromNetwork(terr, sched);
}

void GameScreen::ApplySiegeInfoResponse(const luna::protocol::SiegeInfoResponse* resp) {
    if (!resp) return;
    if (resp->message()) {
        std::string msg = resp->message()->str();
        if (!msg.empty()) state_->chat_messages.push_back(msg);
    }
    if (resp->result() != 0) return;
    ApplySiegeState(resp);
    if (state_->chat_messages.size() > 50) state_->chat_messages.erase(state_->chat_messages.begin());
}

void GameScreen::ApplySiegeActionResponse(const luna::protocol::SiegeActionResponse* resp) {
    if (!resp) return;
    if (resp->message()) {
        std::string msg = resp->message()->str();
        if (!msg.empty()) state_->chat_messages.push_back(msg);
    }
    if (resp->info()) ApplySiegeState(resp->info());
    if (state_->chat_messages.size() > 50) state_->chat_messages.erase(state_->chat_messages.begin());
}

void GameScreen::ApplyTournamentState(const luna::protocol::TournamentListResponse* resp) {
    if (!resp) return;
    state_->network_tournament_player_guild_id = resp->player_guild_id();
    state_->network_tournaments.clear();
    state_->network_tournament_teams.clear();
    state_->network_tournament_matches.clear();
    if (resp->tournaments()) {
        for (auto t : *resp->tournaments()) {
            GameState::NetworkTournamentEntry entry;
            entry.tournament_id = t->tournament_id();
            entry.name = t->name() ? t->name()->str() : "";
            entry.state = t->state();
            entry.registered = t->registered_teams();
            entry.max_teams = t->max_teams();
            entry.prize_gold = t->prize_gold();
            entry.registration_end = t->registration_end();
            entry.current_round = t->current_round();
            entry.min_team_size = t->min_team_size();
            entry.max_team_size = t->max_team_size();
            entry.winner_guild_id = t->winner_guild_id();
            entry.player_registered = t->player_registered();
            entry.can_register = t->can_register();
            entry.can_unregister = t->can_unregister();
            entry.can_claim_prize = t->can_claim_prize();
            entry.seconds_until_start = t->seconds_until_start();
            state_->network_tournaments.push_back(std::move(entry));
        }
    }
    if (resp->teams()) {
        for (auto team : *resp->teams()) {
            GameState::NetworkTournamentTeam entry;
            entry.tournament_id = team->tournament_id();
            entry.guild_id = team->guild_id();
            entry.guild_name = team->guild_name() ? team->guild_name()->str() : "";
            entry.seed = team->seed();
            entry.eliminated = team->eliminated();
            state_->network_tournament_teams.push_back(std::move(entry));
        }
    }
    if (resp->matches()) {
        for (auto m : *resp->matches()) {
            GameState::NetworkTournamentMatch entry;
            entry.tournament_id = m->tournament_id();
            entry.round = m->round();
            entry.match_index = m->match_index();
            entry.team1_guild_id = m->team1_guild_id();
            entry.team2_guild_id = m->team2_guild_id();
            entry.winner_guild_id = m->winner_guild_id();
            entry.completed = m->completed();
            state_->network_tournament_matches.push_back(std::move(entry));
        }
    }
    SyncTournamentFromNetwork();
    if (state_->tournament_open) tournament_dlg_.UpdateFromState(state_);
}

void GameScreen::SyncTournamentFromNetwork() {
    if (state_->network_tournaments.empty()) return;
    std::vector<NetworkTournamentView> tours;
    std::vector<NetworkTournamentTeamView> teams;
    std::vector<NetworkTournamentMatchView> matches;
    for (const auto& t : state_->network_tournaments) {
        NetworkTournamentView v;
        v.tournament_id = t.tournament_id;
        v.name = t.name;
        v.state = t.state;
        v.registered = t.registered;
        v.max_teams = t.max_teams;
        v.prize_gold = t.prize_gold;
        v.current_round = t.current_round;
        tours.push_back(std::move(v));
    }
    for (const auto& t : state_->network_tournament_teams) {
        NetworkTournamentTeamView v;
        v.tournament_id = t.tournament_id;
        v.guild_id = t.guild_id;
        v.guild_name = t.guild_name;
        v.seed = t.seed;
        v.eliminated = t.eliminated;
        teams.push_back(std::move(v));
    }
    for (const auto& m : state_->network_tournament_matches) {
        NetworkTournamentMatchView v;
        v.tournament_id = m.tournament_id;
        v.round = m.round;
        v.match_index = m.match_index;
        v.team1_guild_id = m.team1_guild_id;
        v.team2_guild_id = m.team2_guild_id;
        v.winner_guild_id = m.winner_guild_id;
        v.completed = m.completed;
        matches.push_back(std::move(v));
    }
    tournament_sys_.SyncFromNetwork(tours, teams, matches);
}

void GameScreen::ApplyTournamentListResponse(const luna::protocol::TournamentListResponse* resp) {
    if (!resp) return;
    if (resp->message()) {
        std::string msg = resp->message()->str();
        if (!msg.empty()) state_->chat_messages.push_back(msg);
    }
    if (resp->result() != 0) return;
    ApplyTournamentState(resp);
    if (state_->chat_messages.size() > 50) state_->chat_messages.erase(state_->chat_messages.begin());
}

void GameScreen::ApplyTournamentRegisterResponse(
    const luna::protocol::TournamentRegisterResponse* resp) {
    if (!resp) return;
    std::string msg = resp->message() ? resp->message()->str() : "";
    if (!msg.empty()) state_->chat_messages.push_back(msg);
    if (resp->list()) ApplyTournamentState(resp->list());
    else RequestTournamentList();
    if (state_->chat_messages.size() > 50) state_->chat_messages.erase(state_->chat_messages.begin());
}

void GameScreen::ApplyTournamentActionResponse(
    const luna::protocol::TournamentActionResponse* resp) {
    if (!resp) return;
    std::string msg = resp->message() ? resp->message()->str() : "";
    if (!msg.empty()) state_->chat_messages.push_back(msg);
    if (resp->list()) ApplyTournamentState(resp->list());
    if (state_->chat_messages.size() > 50) state_->chat_messages.erase(state_->chat_messages.begin());
}

void GameScreen::ApplyHousingState(const luna::protocol::HousingInfoResponse* resp) {
    if (!resp) return;
    state_->gold = static_cast<int>(resp->player_gold());
    state_->network_selected_house_id = resp->selected_house_id();
    state_->network_can_buy_house = resp->can_buy();

    state_->network_houses.clear();
    if (resp->houses()) {
        for (auto h : *resp->houses()) {
            GameState::NetworkHouseInfo entry;
            entry.house_id = h->house_id();
            entry.owner_id = h->owner_id();
            entry.name = h->name() ? h->name()->str() : "House";
            entry.map_id = h->map_id();
            entry.pos_x = h->pos_x();
            entry.pos_y = h->pos_y();
            entry.house_type = h->house_type();
            entry.furniture_count = h->furniture_count();
            entry.max_furniture = h->max_furniture();
            entry.is_owner = h->is_owner();
            entry.can_enter = h->can_enter();
            if (h->furniture()) {
                for (auto f : *h->furniture()) {
                    GameState::NetworkHousingFurniture fi;
                    fi.furniture_id = f->furniture_id();
                    fi.item_id = f->item_id();
                    fi.name = f->name() ? f->name()->str() : "Furniture";
                    fi.pos_x = f->pos_x();
                    fi.pos_y = f->pos_y();
                    fi.rot_y = f->rot_y();
                    entry.furniture.push_back(std::move(fi));
                }
            }
            state_->network_houses.push_back(std::move(entry));
        }
    }

    state_->network_house_templates.clear();
    if (resp->templates()) {
        for (auto t : *resp->templates()) {
            GameState::NetworkHouseTemplate entry;
            entry.template_id = t->template_id();
            entry.name = t->name() ? t->name()->str() : "House";
            entry.price = t->price();
            entry.max_furniture = t->max_furniture();
            entry.map_id = t->map_id();
            state_->network_house_templates.push_back(std::move(entry));
        }
    }
    if (state_->housing_open) housing_dlg_.UpdateFromState(state_);
}

void GameScreen::ApplyHousingInfoResponse(const luna::protocol::HousingInfoResponse* resp) {
    if (!resp || resp->result() != 0) return;
    ApplyHousingState(resp);
}

void GameScreen::ApplyHousingActionResponse(const luna::protocol::HousingActionResponse* resp) {
    if (!resp) return;
    std::string msg = resp->message() ? resp->message()->str() : "";
    if (!msg.empty()) state_->chat_messages.push_back(msg);
    if (resp->info()) ApplyHousingState(resp->info());
    if (resp->result() == 0 && resp->action() == 1 && !state_->offline_mode
        && network_ && network_->IsConnected()) {
        const uint32_t hid = state_->network_selected_house_id;
        for (const auto& h : state_->network_houses) {
            if (h.house_id == hid && h.map_id > 0) {
                ChangeMap(h.map_id);
                break;
            }
        }
    }
    if (state_->chat_messages.size() > 50) state_->chat_messages.erase(state_->chat_messages.begin());
}

void GameScreen::ApplyCashShopState(const luna::protocol::CashShopListResponse* resp) {
    if (!resp) return;
    state_->gold = static_cast<int>(resp->player_gold());
    state_->luna_points = static_cast<int>(resp->player_luna_points());
    state_->network_battle_pass_level = resp->battle_pass_level();
    state_->network_battle_pass_xp = resp->battle_pass_xp();
    state_->network_battle_pass_max_xp = resp->battle_pass_max_xp();
    state_->network_battle_pass_active = resp->battle_pass_level() > 0;
    state_->network_season_name = resp->season_name() ? resp->season_name()->str() : "Season 1";

    state_->network_cashshop_items.clear();
    std::vector<CashItem> synced;
    if (resp->items()) {
        for (auto i : *resp->items()) {
            GameState::NetworkCashShopItem entry;
            entry.item_id = i->item_id();
            entry.name = i->name() ? i->name()->str() : "";
            entry.description = i->description() ? i->description()->str() : "";
            entry.price = i->price();
            entry.category = i->category() ? i->category()->str() : "Misc";
            entry.currency_type = i->currency_type();
            entry.stack_count = i->stack_count();
            entry.max_purchase = i->max_purchase();
            entry.purchased_count = i->purchased_count();
            entry.can_afford = i->can_afford();
            entry.on_sale = i->on_sale();
            state_->network_cashshop_items.push_back(std::move(entry));

            CashItem ci;
            ci.id = i->item_id();
            ci.item_id = i->item_id();
            ci.name = entry.name;
            ci.description = entry.description;
            ci.price_luna = entry.currency_type == 1 ? static_cast<int>(entry.price) : 0;
            ci.item_count = entry.stack_count;
            ci.max_purchase = entry.max_purchase;
            ci.category = entry.category;
            synced.push_back(std::move(ci));
        }
    }
    cash_shop_.SyncFromNetwork(synced, state_->luna_points,
        state_->network_battle_pass_level, static_cast<int>(state_->network_battle_pass_xp),
        static_cast<int>(state_->network_battle_pass_max_xp),
        state_->network_battle_pass_active, state_->network_season_name);
    if (state_->cashshop_open) cash_shop_dlg_.UpdateFromState(state_);
}

void GameScreen::ApplyCashShopListResponse(const luna::protocol::CashShopListResponse* resp) {
    if (!resp || resp->result() != 0) return;
    ApplyCashShopState(resp);
}

void GameScreen::ApplyCashShopBuyResponse(const luna::protocol::CashShopBuyResponse* resp) {
    if (!resp) return;
    std::string msg = resp->message() ? resp->message()->str() : "";
    if (resp->result() == 0) {
        state_->chat_messages.push_back(msg.empty() ? "Purchase complete" : msg);
    } else {
        state_->chat_messages.push_back(msg.empty() ? "Purchase failed" : msg);
    }
    if (resp->list()) ApplyCashShopState(resp->list());
    if (state_->chat_messages.size() > 50) state_->chat_messages.erase(state_->chat_messages.begin());
}

void GameScreen::SendHousingAction(uint8_t action, uint32_t house_id, uint8_t template_id,
                                   uint32_t furniture_item_id, float pos_x, float pos_y) {
    if (!network_ || !network_->IsConnected() || state_->offline_mode) return;
    flatbuffers::FlatBufferBuilder fbb;
    auto req = luna::protocol::CreateHousingActionRequest(
        fbb, GetSelectedCharId(), action, house_id, template_id,
        furniture_item_id, pos_x, pos_y);
    fbb.Finish(req);
    network_->SendPacket(luna::protocol::PacketType_MP_HOUSING_ACTION_SYN,
        fbb.GetBufferPointer(), fbb.GetSize());
}

void GameScreen::RequestFarmInfo() {
    if (!network_ || !network_->IsConnected() || state_->offline_mode) return;
    flatbuffers::FlatBufferBuilder fbb;
    auto req = luna::protocol::CreateFarmInfoRequest(fbb, GetSelectedCharId());
    fbb.Finish(req);
    network_->SendPacket(luna::protocol::PacketType_MP_FARM_INFO_SYN,
        fbb.GetBufferPointer(), fbb.GetSize());
}

void GameScreen::SendFarmAction(uint8_t action, uint8_t plot_id, uint32_t seed_id) {
    if (!network_ || !network_->IsConnected() || state_->offline_mode) return;
    flatbuffers::FlatBufferBuilder fbb;
    auto req = luna::protocol::CreateFarmActionRequest(
        fbb, GetSelectedCharId(), action, plot_id, seed_id);
    fbb.Finish(req);
    network_->SendPacket(luna::protocol::PacketType_MP_FARM_ACTION_SYN,
        fbb.GetBufferPointer(), fbb.GetSize());
}

void GameScreen::SyncFarmFromNetwork() {
    farm_.AllocatePlots(9);
    for (int i = 0; i < 9; ++i) {
        if (auto* plot = farm_.GetPlot(i)) {
            *plot = FarmPlot{};
            plot->id = i;
        }
    }
    for (const auto& np : state_->network_farm_plots) {
        if (np.plot_id >= 9) continue;
        if (auto* plot = farm_.GetPlot(np.plot_id)) {
            plot->id = np.plot_id;
            plot->seed_id = static_cast<int>(np.seed_id);
            plot->plant_name = np.plant_name;
            plot->growth_stage = np.growth_stage;
            plot->max_stages = np.max_stages;
            plot->growth_timer = np.growth_timer;
            plot->growth_time = np.growth_time > 0 ? np.growth_time : 60.0f;
            plot->watered = np.watered;
            plot->harvested = np.harvested;
        }
    }
}

void GameScreen::ApplyFarmResponse(const luna::protocol::FarmResponse* resp) {
    if (!resp) return;
    if (resp->message()) {
        std::string msg = resp->message()->str();
        if (!msg.empty()) state_->chat_messages.push_back(msg);
    }
    state_->network_farm_plots.clear();
    state_->network_farm_seeds.clear();
    if (resp->seeds()) {
        for (auto s : *resp->seeds()) {
            GameState::NetworkFarmSeed entry;
            entry.seed_id = s->seed_id();
            entry.name = s->name() ? s->name()->str() : "Seed";
            entry.growth_time_sec = s->growth_time_sec();
            entry.harvest_item_id = s->harvest_item_id();
            state_->network_farm_seeds.push_back(std::move(entry));
        }
    }
    if (resp->plots()) {
        state_->network_farm_plots.resize(9);
        for (int i = 0; i < 9; ++i)
            state_->network_farm_plots[i].plot_id = static_cast<uint8_t>(i);
        for (auto p : *resp->plots()) {
            if (p->plot_id() >= 9) continue;
            auto& entry = state_->network_farm_plots[p->plot_id()];
            entry.plot_id = p->plot_id();
            entry.seed_id = p->seed_id();
            entry.plant_name = p->plant_name() ? p->plant_name()->str() : "";
            entry.growth_stage = p->growth_stage();
            entry.max_stages = p->max_stages();
            entry.growth_pct = p->growth_pct();
            entry.growth_timer = p->growth_timer();
            entry.growth_time = p->growth_time();
            entry.watered = p->watered();
            entry.harvested = p->harvested();
            entry.ready = p->ready();
        }
    }
    SyncFarmFromNetwork();
    if (resp->result() == 0 && resp->harvest_count() > 0) {
        state_->chat_messages.push_back("Harvested x" + std::to_string(resp->harvest_count())
            + " (item #" + std::to_string(resp->harvest_item_id()) + ")");
    }
    if (state_->chat_messages.size() > 50) state_->chat_messages.erase(state_->chat_messages.begin());
}
