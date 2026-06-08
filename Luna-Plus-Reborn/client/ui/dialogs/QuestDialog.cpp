#include "QuestDialog.hpp"
#include <ui/UiScriptParser.hpp>
#include <ui/ColorPalette.hpp>
#include <ui/widgets/TabPanel.hpp>
#include <spdlog/spdlog.h>
#include <cstdio>
#include <algorithm>

struct QuestCategoryTab {
    std::string name;
    std::vector<QuestDef> quests;
    ListBox* list = nullptr;
};

static std::vector<QuestDef> GenerateQuestPool() {
    std::vector<QuestDef> pool;
    QuestDef q;
    
    // Main storyline quests
    q = {1, "Goblin Extermination", "Clear the goblin menace near the village.", "Kill 5 Goblins", 200, 100, "Copper Ring", 1, "Main"};
    pool.push_back(q);
    q = {2, "Wolf Hunter", "Wolves have been attacking livestock.", "Kill 10 Wolves", 500, 250, "Wolf Fang Necklace", 5, "Main"};
    pool.push_back(q);
    q = {3, "Bandit Camp", "Bandits have set up camp nearby.", "Kill 15 Bandits", 800, 400, "Steel Sword", 10, "Main"};
    pool.push_back(q);
    q = {4, "The Lost Heirloom", "Retrieve the family heirloom.", "Find the Heirloom", 1500, 800, "Golden Ring", 20, "Main"};
    pool.push_back(q);
    q = {5, "Troll Slayer", "Kill the troll terrorizing the pass.", "Kill the Mountain Troll", 2000, 1000, "Troll Hide Armor", 25, "Main"};
    pool.push_back(q);
    q = {6, "Dragon's Breath", "Retrieve dragon scales for the forge.", "Collect 10 Dragon Scales", 3000, 2000, "Dragon Scale Shield", 35, "Main"};
    pool.push_back(q);
    q = {7, "The Cursed Temple", "Explore the ancient temple ruins.", "Clear the Temple", 5000, 3000, "Ancient Amulet", 45, "Main"};
    pool.push_back(q);
    q = {8, "Shadow Lord", "Defeat the Shadow Lord threatening the realm.", "Kill the Shadow Lord", 10000, 5000, "Legendary Sword", 55, "Main"};
    pool.push_back(q);
    q = {9, "Hero's Triumph", "Prove yourself as a true hero.", "Complete 3 Main Quests", 15000, 10000, "Hero's Crown", 60, "Main"};
    pool.push_back(q);
    
    // Side quests
    q = {10, "Herb Collection", "The apothecary needs herbs.", "Collect 8 Herbs", 300, 150, "HP Potion x5", 3, "Side"};
    pool.push_back(q);
    q = {11, "Gatherer's Challenge", "Prove your gathering skills.", "Gather 20 items", 600, 350, "Gatherer's Gloves", 10, "Side"};
    pool.push_back(q);
    q = {12, "Fishing Master", "Catch rare fish for the chef.", "Catch 5 Rare Fish", 800, 500, "Golden Fishing Rod", 15, "Side"};
    pool.push_back(q);
    q = {13, "Message Delivery", "Deliver messages across town.", "Deliver 10 Messages", 400, 200, "Swift Boots", 8, "Side"};
    pool.push_back(q);
    q = {14, "Monster Research", "Help the scholar study monsters.", "Defeat 5 of each type", 1200, 700, "Scholar's Glasses", 20, "Side"};
    pool.push_back(q);
    q = {15, "Treasure Hunter", "Find hidden treasures.", "Find 5 Treasure Chests", 2000, 1500, "Treasure Map", 30, "Side"};
    pool.push_back(q);
    
    // Daily quests
    q = {20, "Daily: Monster Hunt", "Hunt monsters for rewards.", "Kill 20 Monsters", 400, 200, "Daily Chest", 1, "Daily"};
    pool.push_back(q);
    q = {21, "Daily: Mining", "Mine ore deposits.", "Mine 10 Ores", 300, 150, "Iron Ore x5", 1, "Daily"};
    pool.push_back(q);
    q = {22, "Daily: Fishing", "Catch fish for the market.", "Catch 10 Fish", 350, 180, "Fish Bundle", 1, "Daily"};
    pool.push_back(q);
    q = {23, "Daily: Dungeon Run", "Complete a dungeon floor.", "Clear 1 Dungeon Floor", 600, 400, "Dungeon Key", 1, "Daily"};
    pool.push_back(q);
    q = {24, "Daily: Crafting", "Create items at the forge.", "Craft 5 Items", 300, 200, "Crafting Materials", 1, "Daily"};
    pool.push_back(q);
    
    // Weekly quests
    q = {30, "Weekly: Boss Hunter", "Defeat world bosses.", "Kill 3 Boss Monsters", 3000, 2000, "Boss Chest", 30, "Weekly"};
    pool.push_back(q);
    q = {31, "Weekly: Dungeon Marathon", "Complete many dungeon floors.", "Clear 10 Dungeon Floors", 5000, 3000, "Rare Weapon Box", 35, "Weekly"};
    pool.push_back(q);
    q = {32, "Weekly: PvP Champion", "Win PvP battles.", "Win 5 PvP Matches", 4000, 2500, "PvP Trophy", 30, "Weekly"};
    pool.push_back(q);
    
    // Event quests
    q = {40, "Event: Spring Festival", "Celebrate the spring festival!", "Talk to 5 NPCs", 500, 300, "Festival Hat", 1, "Event"};
    pool.push_back(q);
    q = {41, "Event: Halloween", "Trick or treat!", "Collect 20 Candies", 800, 500, "Pumpkin Mask", 10, "Event"};
    pool.push_back(q);
    q = {42, "Event: Christmas", "Spread holiday cheer.", "Deliver 15 Gifts", 1000, 800, "Santa Hat", 15, "Event"};
    pool.push_back(q);
    
    return pool;
}

std::vector<QuestDef> QuestDialog::GetAvailableQuests(int level) {
    auto all = GenerateQuestPool();
    std::vector<QuestDef> available;
    for (auto& q : all) {
        if (level >= q.required_level) available.push_back(q);
    }
    return available;
}

bool QuestDialog::TryCompleteQuest(GameState* state, const QuestDef& quest) {
    // Check if quest is in the player's list
    auto it = std::find(state->quest_list.begin(), state->quest_list.end(), quest.name);
    if (it == state->quest_list.end()) return false;
    
    // Remove quest and give rewards
    state->quest_list.erase(it);
    state->exp = std::min(state->exp + quest.xp_reward, state->exp_next);
    state->gold += quest.gold_reward;
    
    char buf[256];
    snprintf(buf, sizeof(buf), "Quest Complete: %s! +%d XP, +%d Gold, %s",
        quest.name.c_str(), quest.xp_reward, quest.gold_reward, quest.item_reward.c_str());
    state->chat_messages.push_back(buf);
    return true;
}

void QuestDialog::Open(GameState* state, WindowManager* wm) {
    if (window_) return;

    if (wm) window_ = wm->LoadFromScript("assets/interface/Windows/Quest.bin.txt");

    if (!window_) {
        spdlog::warn("QuestDialog: failed to load UI script, using C++ fallback");
        window_ = new Window("", 600, 50, 522, 420);
        window_->SetClosable(true);
        window_->SetMovable(true);
        window_->SetTitleBarH(30);
    }
    window_->SetClosable(true);
    window_->SetMovable(true);
    window_->SetTitleBarH(30);

    window_->SetCustomBackground([this](UIRenderer& ui, float x, float y, float w, float h) {
        if (!bgfx::isValid(bg_tex_.handle)) {
            bg_tex_ = ui.LoadTexture("ui_atlas2", "b2.png");
        }
        if (bgfx::isValid(bg_tex_.handle)) {
            ui.DrawImageUV(x, y, w, h, bg_tex_.handle, 1.0f/1024.0f, 532.0f/1024.0f, 523.0f/1024.0f, 892.0f/1024.0f);
        } else {
            ui.DrawRect(x, y, w, h, {40,40,60,200});
            ui.DrawBorder(x, y, w, h, {100,100,150,200});
        }
    });

    // Tab panel
    auto* tabs = window_->AddWidget<TabPanel>(15, 40, 240, 300);

    // Available quests
    auto* avail_list = new ListBox(0, 0, 240, 260);
    current_quests_ = GetAvailableQuests(state->level);
    for (auto& q : current_quests_) {
        char buf[128];
        snprintf(buf, sizeof(buf), "[Lv.%d] %s", q.required_level, q.name.c_str());
        avail_list->AddItem(buf);
    }
    tabs->AddTab("Available", avail_list);

    // In Progress
    quest_list_ = new ListBox(0, 0, 240, 260);
    for (auto& q : state->quest_list) quest_list_->AddItem(q);
    tabs->AddTab("In Progress", quest_list_);

    // Completed
    auto* done_list = new ListBox(0, 0, 240, 260);
    tabs->AddTab("Completed", done_list);

    // Detail text area (right side)
    detail_label_ = window_->AddWidget<Label>("Select a quest to see details.", 270, 50, ColorPalette::TEXT_DARK);
    reward_label_ = window_->AddWidget<Label>("", 270, 240, ColorPalette::TEXT_BROWN);

    // Accept / Complete buttons
    auto* accept_btn = window_->AddWidget<Button>("Accept Quest", 270, 370, 110, 24);
    accept_btn->SetColors({40,80,40,220}, {80,130,80,220}, {30,50,30,220});
    accept_btn->OnEvent([this, state, tabs, avail_list](const UIEvent& e) {
        if (e.type == UIEvent::Click) {
            int idx = tabs->GetActive();
            if (idx == 0) {
                if (avail_list && avail_list->GetSelected() >= 0 && avail_list->GetSelected() < (int)current_quests_.size()) {
                    auto& q = current_quests_[avail_list->GetSelected()];
                    state->quest_list.push_back(q.name);
                    state->chat_messages.push_back("Quest accepted: " + q.name);
                    if (detail_label_) detail_label_->SetText("Quest accepted! Check 'In Progress' tab.");
                    // Update progress list
                    quest_list_->Clear();
                    for (auto& qn : state->quest_list) quest_list_->AddItem(qn);
                }
            }
        }
    });

    auto* complete_btn = window_->AddWidget<Button>("Complete", 390, 370, 100, 24);
    complete_btn->SetColors({80,80,40,220}, {130,130,80,220}, {50,50,30,220});
    complete_btn->OnEvent([this, state, tabs](const UIEvent& e) {
        if (e.type == UIEvent::Click) {
            int idx = tabs->GetActive();
            if (idx == 1) {
                if (quest_list_ && quest_list_->GetSelected() >= 0) {
                    // Find matching quest from pool
                    for (auto& q : current_quests_) {
                        if (q.name == quest_list_->GetItem(quest_list_->GetSelected())) {
                            if (TryCompleteQuest(state, q)) {
                                quest_list_->Clear();
                                for (auto& qn : state->quest_list) quest_list_->AddItem(qn);
                                if (detail_label_) detail_label_->SetText("Quest completed! Check rewards.");
                            }
                            break;
                        }
                    }
                }
            }
        }
    });

    // Click on available quests shows detail
    // (Since ListBox doesn't have a callback, we'll handle via tabs)
    (void)avail_list;
    (void)done_list;
}

void QuestDialog::ShowQuestDetail(GameState* state, int index) {
    if (index < 0 || index >= (int)current_quests_.size()) return;
    auto& q = current_quests_[index];
    (void)state;
    char buf[512];
    snprintf(buf, sizeof(buf), "%s (Lv.%d)\n\n%s\n\nObjective: %s\nCategory: %s",
        q.name.c_str(), q.required_level, q.description.c_str(), q.objective.c_str(), q.category.c_str());
    if (detail_label_) detail_label_->SetText(buf);
    
    char rbuf[256];
    snprintf(rbuf, sizeof(rbuf), "Rewards: %d XP | %d Gold | %s",
        q.xp_reward, q.gold_reward, q.item_reward.c_str());
    if (reward_label_) reward_label_->SetText(rbuf);
}

void QuestDialog::UpdateFromState(GameState* state) {
    if (!quest_list_) return;
    quest_list_->Clear();
    for (auto& q : state->quest_list) quest_list_->AddItem(q);
}
