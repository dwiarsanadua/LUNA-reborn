#include "ClientFlow.hpp"
#include "ScreenManager.hpp"
#include "GameState.hpp"
#include <network/NetworkClient.hpp>
#include <engine/EngineMap.hpp>
#include <Character_generated.h>
#include <Login_generated.h>
#include <PacketType_generated.h>
#include <flatbuffers/flatbuffers.h>
#include <spdlog/spdlog.h>

static ScreenManager* g_screens = nullptr;
static EngineMap* g_map = nullptr;

namespace ClientFlow {

void Init(ScreenManager* screens, EngineMap* map) {
    g_screens = screens;
    g_map = map;
}

void StartOffline(GameState& state) {
    state.offline_mode = true;
    state.login_ok = true;
    state.session_token = "offline";
    state.login_error.clear();
    state.characters.clear();
    state.characters.push_back({1, "Hero", 10, 51, 0.0f, 0.0f, 0.0f});
    state.characters.push_back({2, "Mage", 8, 51, 5.0f, 0.0f, 5.0f});
    state.selected_char = 0;
    state.current_state = ClientState::CharSelect;
    if (g_screens) g_screens->SwitchTo("charselect");
    spdlog::info("ClientFlow: offline demo — character select");
}

void OnLoginSuccess(GameState& state) {
    state.current_state = ClientState::CharSelect;
    if (g_screens) g_screens->SwitchTo("charselect");
}

void BeginEnterGame(GameState& state) {
    if (!state.characters.empty() && state.selected_char < (int)state.characters.size()) {
        auto& ch = state.characters[state.selected_char];
        state.name = ch.name;
        state.level = ch.level;
        if (!state.map_id) state.map_id = ch.map_id ? ch.map_id : 51;
        state.hp = 500;
        state.max_hp = 500 + ch.level * 20;
        state.mp = 100;
        state.max_mp = 100 + ch.level * 5;
    }
    state.enter_game_pending = true;
    state.enter_game_load_timer = 0.0f;
    state.current_state = ClientState::Loading;
    if (g_screens) g_screens->SwitchTo("loading");
}

void OnEnterWorldData(GameState& state, uint32_t map_id, float x, float y, float z) {
    state.map_id = map_id;
    state.player_x = x;
    state.player_y = y;
    state.player_z = z;
    if (g_map) {
        g_map->Unload();
        g_map->Load(std::to_string(map_id ? map_id : 51));
    }
}

void Update(GameState& state, float dt) {
    if (!state.enter_game_pending || !g_screens) return;
    if (g_screens->CurrentName() != "loading") return;

    state.enter_game_load_timer += dt;
    if (state.enter_game_load_timer < 1.5f) return;

    state.enter_game_pending = false;
    state.enter_game_load_timer = 0.0f;
    state.current_state = ClientState::GameIn;
    g_screens->SwitchTo("game");
    spdlog::info("ClientFlow: entered game (map {})", state.map_id);
}

bool OnPacket(uint16_t type, const std::vector<uint8_t>& payload,
              GameState& state, NetworkClient& network) {
    using namespace luna::protocol;

    if (type == PacketType_MP_USERCONN_LOGIN_ACK) {
        auto resp = flatbuffers::GetRoot<LoginResponse>(payload.data());
        if (resp->result() == LoginResult_Success) {
            state.login_ok = true;
            if (resp->session_token()) state.session_token = resp->session_token()->str();
            OnLoginSuccess(state);
            flatbuffers::FlatBufferBuilder fbb;
            auto req = CreateCharacterListRequestDirect(fbb, state.session_token.c_str());
            fbb.Finish(req);
            network.SendPacket(PacketType_MP_USERCONN_CHARACTERLIST_SYN,
                fbb.GetBufferPointer(), fbb.GetSize());
        }
        return true;
    }

    if (type == PacketType_MP_USERCONN_GAMEIN_ACK || type == 0x0208) {
        uint32_t map_id = state.map_id ? state.map_id : 51;
        float x = 0, y = 0, z = 0;
        auto resp = flatbuffers::GetRoot<EnterWorldResponse>(payload.data());
        if (resp) {
            if (resp->position()) {
                x = resp->position()->x();
                y = resp->position()->y();
                z = resp->position()->z();
            }
            if (resp->map_id()) map_id = resp->map_id();
        }
        if (state.map_changing) {
            state.map_changing = false;
            state.player_x = x;
            state.player_y = y;
            state.player_z = z;
            state.map_id = map_id;
            return true;
        }
        OnEnterWorldData(state, map_id, x, y, z);
        state.connecting = !state.offline_mode;
        BeginEnterGame(state);
        return true;
    }

    return false;
}

}
