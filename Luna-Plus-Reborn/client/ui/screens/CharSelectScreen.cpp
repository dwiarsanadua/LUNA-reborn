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
#include <string>

extern ScreenManager* g_screen_mgr;

// Delete confirmation state (inline since header cannot be modified)
static bool s_delete_confirm_pending = false;
static int s_delete_target_slot = -1;
static float s_delete_error_timer = 0.0f;
static std::string s_delete_error_msg;
static float s_delete_confirm_timer = 0.0f;

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
    if (type == PacketType_MP_USERCONN_CHARACTER_DELETE_ACK) {
        // Character deleted successfully, refresh list
        s_delete_confirm_pending = false;
        s_delete_target_slot = -1;
        s_delete_confirm_timer = 0.0f;
        waiting_for_list_ = true;
        flatbuffers::FlatBufferBuilder fbb;
        auto req = CreateCharacterListRequestDirect(fbb, state_->session_token.c_str());
        fbb.Finish(req);
        network_->SendPacket(PacketType_MP_USERCONN_CHARACTERLIST_SYN, fbb.GetBufferPointer(), fbb.GetSize());
        spdlog::info("CharSelect: character deleted successfully");
        return true;
    }
    if (type == PacketType_MP_USERCONN_CHARACTER_REMOVE_NACK) {
        // Delete failed
        s_delete_confirm_pending = false;
        s_delete_target_slot = -1;
        s_delete_confirm_timer = 0.0f;
        s_delete_error_msg = "Failed to delete character";
        s_delete_error_timer = 5.0f;
        spdlog::warn("CharSelect: character delete failed");
        return true;
    }
    return false;
}

void CharSelectScreen::Update(float dt) {
    // Delete confirmation timeout
    if (s_delete_confirm_pending) {
        s_delete_confirm_timer += dt;
        if (s_delete_confirm_timer > 10.0f) {
            s_delete_confirm_pending = false;
            s_delete_target_slot = -1;
            s_delete_confirm_timer = 0.0f;
        }
    }

    // Error message timer
    if (s_delete_error_timer > 0.0f) {
        s_delete_error_timer -= dt;
        if (s_delete_error_timer <= 0.0f) {
            s_delete_error_msg.clear();
        }
    }
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

    // Delete confirmation overlay
    if (s_delete_confirm_pending) {
        float cw = 400, ch = 160;
        float cx = lw * 0.5f - cw * 0.5f;
        float cy = lh * 0.5f - ch * 0.5f;
        ui.DrawRect(cx, cy, cw, ch, UIColor{40, 20, 20, 240});
        ui.DrawBorder(cx, cy, cw, ch, UIColor{200, 60, 60, 255});
        ui.DrawTextCentered(cy + 20, 0xFFFF6666, "Delete Character?");
        if (s_delete_target_slot >= 0 && s_delete_target_slot < (int)slots_.size() && slots_[s_delete_target_slot].exists) {
            ui.DrawTextCentered(cy + 48, 0xFFFFCC88, "Name: %s", slots_[s_delete_target_slot].name.c_str());
            ui.DrawTextCentered(cy + 68, 0xFFCCCCCC, "Level %d %s", slots_[s_delete_target_slot].level, "");
        }
        ui.DrawTextCentered(cy + 96, 0xFFAAAAAA, "Press ENTER to confirm, ESC to cancel");
        float remaining = 10.0f - s_delete_confirm_timer;
        ui.DrawTextCentered(cy + 118, 0xFF666666, "Auto-cancel in %.0fs", remaining);
    }

    // Error message
    if (s_delete_error_timer > 0.0f && !s_delete_error_msg.empty()) {
        ui.DrawTextCentered(lh * 0.80f, 0xFFFF4444, "%s", s_delete_error_msg.c_str());
    }

    // Bottom hint
    ui.DrawTextCentered(lh * 0.90f, 0xFF888888, "Arrows: Select  Enter: Start  C: Create  Del: Delete  Esc: Back");

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

    // Handle delete confirmation dialog
    if (s_delete_confirm_pending) {
        if (key == 257) { // Enter — confirm delete
            s_delete_confirm_pending = false;
            s_delete_confirm_timer = 0.0f;
            if (s_delete_target_slot >= 0 && s_delete_target_slot < (int)slots_.size() && slots_[s_delete_target_slot].exists) {
                uint32_t char_id = slots_[s_delete_target_slot].entity_id;
                flatbuffers::FlatBufferBuilder fbb;
                auto req = luna::protocol::CreateDeleteCharacterRequestDirect(
                    fbb, state_->session_token.c_str(), char_id);
                fbb.Finish(req);
                network_->SendPacket(luna::protocol::PacketType_MP_USERCONN_CHARACTER_DELETE_SYN,
                    fbb.GetBufferPointer(), fbb.GetSize());
                spdlog::info("CharSelect: requesting delete of character {}", char_id);
            }
            return true;
        }
        if (key == 256) { // Escape — cancel delete
            s_delete_confirm_pending = false;
            s_delete_target_slot = -1;
            s_delete_confirm_timer = 0.0f;
            return true;
        }
        return false;
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
    if (key == 261 || key == 127) { // Delete key or Backspace — delete character
        if (!slots_.empty() && selected_slot_ >= 0 && selected_slot_ < (int)slots_.size() && slots_[selected_slot_].exists) {
            s_delete_confirm_pending = true;
            s_delete_target_slot = selected_slot_;
            s_delete_confirm_timer = 0.0f;
            spdlog::info("CharSelect: delete confirmation for slot {}: {}", selected_slot_, slots_[selected_slot_].name);
        }
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
