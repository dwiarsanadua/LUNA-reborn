#include "NpcDialogSystem.hpp"
#include <spdlog/spdlog.h>

void NpcDialogSystem::LoadFromDB(class GameDataDB& db) {
    (void)db;
    BuildDefaultDialogs();
    spdlog::info("NpcDialogSystem: {} dialogs loaded", dialogs_.size());
}

void NpcDialogSystem::BuildDefaultDialogs() {
    // Blacksmith (npc_id = 1)
    {
        NpcDialog dialog;
        dialog.npc_id = 1;
        dialog.npc_name = "Blacksmith";
        dialog.greeting = "\"Need something? I'm the town Blacksmith.\"";
        
        dialog.nodes = {
            {0, "Blacksmith", dialog.greeting, {{1, "Buy Items"}, {2, "Sell Items"}, {3, "Repair"}, {4, "Enchant"}, {5, "Compose"}, {6, "Leave"}}, -1, ""},
            {1, "Blacksmith", "Browse my wares.", {{0, "Back"}}, -1, "shop", 1},
            {2, "Blacksmith", "What do you want to sell?", {{0, "Back"}}, -1, "sell"},
            {3, "Blacksmith", "Repair costs 10% of item value.", {{0, "Back"}}, -1, "repair"},
            {4, "Blacksmith", "Select an item to enchant.", {{0, "Back"}}, -1, "enchant"},
            {5, "Blacksmith", "Combine 3 identical items to upgrade.", {{0, "Back"}}, -1, "compose"},
            {6, "Blacksmith", "Come back anytime!", {}, -1, ""}
        };
        dialog.start_node = 0;
        dialogs_[1] = dialog;
    }
    
    // Quest NPC (npc_id = 2)
    {
        NpcDialog dialog;
        dialog.npc_id = 2;
        dialog.npc_name = "Village Elder";
        dialog.greeting = "\"Ah, a brave adventurer! I have tasks for you.\"";
        
        dialog.nodes = {
            {0, "Village Elder", dialog.greeting, {{1, "Accept Quest"}, {2, "I'm just looking"}, {3, "Goodbye"}}, -1, ""},
            {1, "Village Elder", "Which quest interests you?", {{10, "Kill 5 Goblins"}, {11, "Collect Herbs"}, {0, "Back"}}, -1, "quest_list"},
            {2, "Village Elder", "Take your time, adventurer.", {{0, "Back"}}, -1, ""},
            {3, "Village Elder", "Safe travels!", {}, -1, ""},
            {10, "Village Elder", "Defeat 5 Goblins and bring me their ears!", {{0, "Back"}}, -1, "quest", 1},
            {11, "Village Elder", "I need 3 Moon Herbs for a potion.", {{0, "Back"}}, -1, "quest", 2}
        };
        dialog.start_node = 0;
        dialogs_[2] = dialog;
    }
    
    spdlog::info("NpcDialogSystem: {} default dialogs built", dialogs_.size());
}

const NpcDialog* NpcDialogSystem::GetDialog(int npc_id) const {
    auto it = dialogs_.find(npc_id);
    if (it != dialogs_.end()) return &it->second;
    return nullptr;
}

const NpcDialogNode* NpcDialogSystem::GetNode(int npc_id, int node_id) const {
    auto it = dialogs_.find(npc_id);
    if (it == dialogs_.end()) return nullptr;
    for (auto& node : it->second.nodes) {
        if (node.id == node_id) return &node;
    }
    return nullptr;
}

int NpcDialogSystem::GetStartNode(int npc_id) const {
    auto it = dialogs_.find(npc_id);
    if (it != dialogs_.end()) return it->second.start_node;
    return -1;
}

std::vector<int> NpcDialogSystem::GetNpcIDs() const {
    std::vector<int> ids;
    for (auto& [id, _] : dialogs_) ids.push_back(id);
    return ids;
}

void NpcDialogSystem::RegisterDialog(const NpcDialog& dialog) {
    dialogs_[dialog.npc_id] = dialog;
}
