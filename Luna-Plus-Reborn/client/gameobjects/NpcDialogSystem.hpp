#pragma once
#include <string>
#include <vector>
#include <unordered_map>

class GameDataDB;

struct NpcDialogNode {
    int id = 0;
    std::string speaker;
    std::string text;
    std::vector<std::pair<int, std::string>> choices; // node_id, choice_text
    int next_node = -1; // -1 = end dialog
    std::string action; // "shop", "quest", "repair", "enchant", "compose", "storage"
    int action_param = 0; // shop_id, quest_id, etc.
};

struct NpcDialog {
    int npc_id = 0;
    std::string npc_name;
    std::string greeting;
    std::vector<NpcDialogNode> nodes;
    int start_node = 0;
};

class NpcDialogSystem {
public:
    void LoadFromDB(GameDataDB& db);

    // Get dialog tree for an NPC
    const NpcDialog* GetDialog(int npc_id) const;

    // Get a specific node
    const NpcDialogNode* GetNode(int npc_id, int node_id) const;

    // Get the starting node for an NPC
    int GetStartNode(int npc_id) const;

    // Get all NPC IDs that have dialogs
    std::vector<int> GetNpcIDs() const;

    // Register a custom dialog (for NPCs not in the database)
    void RegisterDialog(const NpcDialog& dialog);

private:
    std::unordered_map<int, NpcDialog> dialogs_;
    void BuildDefaultDialogs();
};
