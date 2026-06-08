#include "CharSelectScreen.hpp"
#include <ui/GameState.hpp>
#include <network/NetworkClient.hpp>
#include <flatbuffers/flatbuffers.h>
#include <Character_generated.h>
#include <PacketType_generated.h>
#include <spdlog/spdlog.h>

void CharSelectScreen::Init(GameState* state, NetworkClient* network) {
    state_ = state;
    network_ = network;
}

void CharSelectScreen::Enter() {
    spdlog::info("Entering Character Selection Screen");
}

void CharSelectScreen::Exit() {
    spdlog::info("Exiting Character Selection Screen");
}

bool CharSelectScreen::HandlePacket(uint16_t type, const std::vector<uint8_t>& payload) {
    using namespace luna::protocol;
    if (type == PacketType_MP_USERCONN_CHARACTERLIST_ACK) {
        auto resp = flatbuffers::GetRoot<CharacterListResponse>(payload.data());
        state_->characters.clear();
        if (resp->characters()) {
            for (auto c : *resp->characters()) {
                CharInfo info;
                info.id = c->id();
                info.name = c->name() ? c->name()->str() : "Unnamed";
                info.level = c->level();
                info.map_id = c->map_id();
                state_->characters.push_back(info);
            }
        }
        return true;
    }
    if (type == PacketType_MP_USERCONN_CHARACTER_MAKE_ACK) {
        // Character created, refresh list
        flatbuffers::FlatBufferBuilder fbb;
        auto req = CreateCharacterListRequestDirect(fbb, state_->session_token.c_str());
        fbb.Finish(req);
        network_->SendPacket(PacketType_MP_USERCONN_CHARACTERLIST_SYN, fbb.GetBufferPointer(), fbb.GetSize());
        return true;
    }
    return false;
}

void CharSelectScreen::Update(float dt) {
    (void)dt;
}

void CharSelectScreen::Render(UIRenderer& ui) {
    ui.DrawRect(0, 0, 1280, 720, {10, 10, 30, 255});
    ui.DrawTextCentered(100, 0xffffcc88, "Select Character");

    for (size_t i = 0; i < state_->characters.size(); i++) {
        auto& ch = state_->characters[i];
        float y = 200 + i * 60;
        bool sel = (i == (size_t)state_->selected_char);
        ui.DrawRect(440, y, 400, 50, sel ? UIColor{50, 50, 80, 255} : UIColor{30, 30, 50, 255});
        ui.DrawText(460, y + 15, 0xffffffff, "%s (Lv.%d)", ch.name.c_str(), ch.level);
    }

    ui.DrawTextCentered(600, 0xff888888, "Arrows: Select  Enter: Start  C: Create");
}

bool CharSelectScreen::HandleKey(int key, int scancode, int action, int mods) {
    if (action != 1) return false;

    if (key == 257) { // Enter
        if (state_->selected_char < (int)state_->characters.size()) {
            auto& ch = state_->characters[state_->selected_char];
            flatbuffers::FlatBufferBuilder fbb;
            auto req = luna::protocol::CreateEnterWorldRequestDirect(fbb, state_->session_token.c_str(), ch.id);
            fbb.Finish(req);
            network_->SendPacket(luna::protocol::PacketType_MP_USERCONN_GAMEIN_SYN, fbb.GetBufferPointer(), fbb.GetSize());
            return true;
        }
    }

    if (key == 67) { // C - Create
        std::string name = "Player" + std::to_string(rand() % 1000);
        flatbuffers::FlatBufferBuilder fbb;
        auto req = luna::protocol::CreateCreateCharacterRequestDirect(fbb, state_->session_token.c_str(), name.c_str(), 0, 0, 0, 0, 0);
        fbb.Finish(req);
        network_->SendPacket(luna::protocol::PacketType_MP_USERCONN_CHARACTER_MAKE_SYN, fbb.GetBufferPointer(), fbb.GetSize());
        return true;
    }

    if (key == 264) { // Down
        if (!state_->characters.empty())
            state_->selected_char = (state_->selected_char + 1) % state_->characters.size();
        return true;
    }
    if (key == 265) { // Up
        if (!state_->characters.empty())
            state_->selected_char = (state_->selected_char + state_->characters.size() - 1) % state_->characters.size();
        return true;
    }

    return false;
}
