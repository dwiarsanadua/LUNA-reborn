#include "CharSelectScreen.hpp"
#include <ui/ClientFlow.hpp>
#include <ui/GameState.hpp>
#include <ui/WindowManager.hpp>
#include <ui/ScreenManager.hpp>
#include <network/NetworkClient.hpp>
#include <flatbuffers/flatbuffers.h>
#include <Character_generated.h>
#include <PacketType_generated.h>
#include <spdlog/spdlog.h>

extern ScreenManager* g_screen_mgr;

void CharSelectScreen::Init(GameState* state, NetworkClient* network) {
    state_ = state;
    network_ = network;
}

void CharSelectScreen::Enter() {
    spdlog::info("Entering Character Selection Screen");
    waiting_for_list_ = true;
    selected_slot_ = 0;
    slots_.clear();
    RequestCharList();
}

void CharSelectScreen::RequestCharList() {
    if (!network_->IsConnected()) return;
    flatbuffers::FlatBufferBuilder fbb;
    auto req = luna::protocol::CreateCharacterListRequestDirect(fbb, state_->session_token.c_str());
    fbb.Finish(req);
    network_->SendPacket(luna::protocol::PacketType_MP_USERCONN_CHARACTERLIST_SYN,
                         fbb.GetBufferPointer(), fbb.GetSize());
}

void CharSelectScreen::Exit() {
    spdlog::info("Exiting Character Selection Screen");
}

bool CharSelectScreen::HandlePacket(uint16_t type, const std::vector<uint8_t>& payload) {
    using namespace luna::protocol;
    if (type == PacketType_MP_USERCONN_CHARACTERLIST_ACK) {
        auto resp = flatbuffers::GetRoot<CharacterListResponse>(payload.data());
        state_->characters.clear();
        slots_.clear();
        if (resp->characters()) {
            for (auto c : *resp->characters()) {
                CharInfo info;
                info.id = c->id();
                info.name = c->name() ? c->name()->str() : "Unnamed";
                info.level = c->level();
                info.map_id = c->map_id();
                state_->characters.push_back(info);

                CharSlotInfo slot;
                slot.exists = true;
                slot.entity_id = c->id();
                slot.name = c->name() ? c->name()->str() : "Unnamed";
                slot.level = c->level();
                slot.char_class = c->class_();
                slots_.push_back(slot);
            }
        }
        // Fill remaining empty slots (minimum 4)
        while ((int)slots_.size() < 4) {
            slots_.push_back(CharSlotInfo{});
        }
        waiting_for_list_ = false;
        return true;
    }
    if (type == PacketType_MP_USERCONN_CHARACTER_MAKE_ACK) {
        // Character created, refresh list
        waiting_for_list_ = true;
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
    float lw = ui.logicalWidth;
    float lh = ui.logicalHeight;

    // Dark background
    ui.DrawRect(0, 0, lw, lh, UIColor{10, 10, 20, 255});

    // Title
    ui.DrawTextCentered(lh * 0.06f, 0xFFFFCC88, "Select Character");

    // Waiting / loading
    if (waiting_for_list_) {
        ui.DrawTextCentered(lh * 0.5f, 0xFF888888, "Loading characters...");
        if (charmake_open_) charmake_dlg_.Render(ui);
        return;
    }

    // Character slots grid
    float slot_size = 110;
    float gap = 20;
    float total_w = (float)slots_.size() * (slot_size + gap) - gap;
    float start_x = lw * 0.5f - total_w * 0.5f;

    for (size_t i = 0; i < slots_.size(); i++) {
        float sx = start_x + (float)i * (slot_size + gap);
        float sy = lh * 0.18f;

        bool sel = (i == (size_t)selected_slot_);

        // Slot background
        ui.DrawRect(sx, sy, slot_size, slot_size * 1.4f,
                    sel ? UIColor{60, 60, 100, 200} : UIColor{30, 30, 50, 200});
        ui.DrawBorder(sx, sy, slot_size, slot_size * 1.4f,
                      sel ? UIColor{150, 150, 255, 255} : UIColor{80, 80, 120, 150});

        if (slots_[i].exists) {
            ui.DrawTextCentered(sy + slot_size * 1.4f + 10, 0xFFFFFFFF,
                               "%s", slots_[i].name.c_str());
            ui.DrawTextCentered(sy + slot_size * 1.4f + 28, 0xFFAAAAAA,
                               "Lv.%d", slots_[i].level);
        } else {
            ui.DrawTextCentered(sy + slot_size * 0.6f, 0xFF666666, "Empty");
            ui.DrawTextCentered(sy + slot_size * 1.4f + 10, 0xFF666666, "---");
        }
    }

    // Buttons
    float by = lh * 0.72f;
    ui.DrawButton(lw * 0.5f - 250, by, 130, 36, "Create", false);
    ui.DrawButton(lw * 0.5f - 100, by, 130, 36, "Delete", false);
    bool can_enter = !slots_.empty() && selected_slot_ >= 0 && selected_slot_ < (int)slots_.size() && slots_[selected_slot_].exists;
    ui.DrawButton(lw * 0.5f + 50, by, 200, 36, "Enter Game", can_enter);

    // Bottom hint
    ui.DrawTextCentered(lh * 0.90f, 0xFF888888, "Arrows: Select  Enter: Start  C: Create  Esc: Back");

    if (charmake_open_) charmake_dlg_.Render(ui);
}

bool CharSelectScreen::HandleKey(int key, int scancode, int action, int mods) {
    if (action != 1) return false;

    if (charmake_open_) {
        if (charmake_dlg_.HandleKey(key, action)) {
            if (!charmake_dlg_.IsOpen()) {
                charmake_open_ = false;
                state_->current_state = ClientState::CharSelect;
                return true;
            }
            if (charmake_dlg_.IsComplete()) {
                CharInfo created{};
                if (charmake_dlg_.FinalizeInto(*state_, created)) {
                    if (!state_->offline_mode && network_->IsConnected()) {
                        flatbuffers::FlatBufferBuilder fbb;
                        auto req = luna::protocol::CreateCreateCharacterRequestDirect(
                            fbb, state_->session_token.c_str(), created.name.c_str(),
                            state_->class_id, state_->race, 0, 0, 0);
                        fbb.Finish(req);
                        network_->SendPacket(luna::protocol::PacketType_MP_USERCONN_CHARACTER_MAKE_SYN,
                            fbb.GetBufferPointer(), fbb.GetSize());
                    }
                }
                charmake_dlg_.Close();
                charmake_open_ = false;
                state_->current_state = ClientState::CharSelect;
            }
            return true;
        }
    }

    if (key == 262) { // Right
        if (!slots_.empty())
            selected_slot_ = (selected_slot_ + 1) % (int)slots_.size();
        return true;
    }
    if (key == 263) { // Left
        if (!slots_.empty())
            selected_slot_ = (selected_slot_ - 1 + (int)slots_.size()) % (int)slots_.size();
        return true;
    }
    if (key == 257) { // Enter
        if (!slots_.empty() && selected_slot_ >= 0 && selected_slot_ < (int)slots_.size() && slots_[selected_slot_].exists) {
            SendCharSelect(selected_slot_);
        }
        return true;
    }
    if (key == 256) { // Escape
        if (g_screen_mgr) g_screen_mgr->SwitchTo("login");
        return true;
    }
    if (key == 67) { // C - Character creation
        charmake_open_ = true;
        charmake_dlg_.Open(nullptr);
        state_->current_state = ClientState::CharMake;
        return true;
    }

    return false;
}

void CharSelectScreen::SendCharSelect(int slot) {
    if (slot < 0 || slot >= (int)slots_.size() || !slots_[slot].exists) return;

    if (state_->offline_mode) {
        ClientFlow::OnEnterWorldData(*state_, 51, 0, 0, 0);
        ClientFlow::BeginEnterGame(*state_);
        return;
    }

    flatbuffers::FlatBufferBuilder fbb;
    auto req = luna::protocol::CreateEnterWorldRequestDirect(fbb, state_->session_token.c_str(), slots_[slot].entity_id);
    fbb.Finish(req);
    network_->SendPacket(luna::protocol::PacketType_MP_USERCONN_GAMEIN_SYN,
                         fbb.GetBufferPointer(), fbb.GetSize());
    state_->current_state = ClientState::Loading;
}

bool CharSelectScreen::HandleChar(unsigned int codepoint) {
    if (charmake_open_) return charmake_dlg_.HandleChar(codepoint);
    return false;
}
