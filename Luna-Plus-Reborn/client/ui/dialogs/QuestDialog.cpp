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

void QuestDialog::SetNetworkCallbacks(std::function<void(uint32_t)> start_fn,
                                      std::function<void(uint32_t)> complete_fn,
                                      std::function<void()> refresh_fn) {
    on_start_quest_ = std::move(start_fn);
    on_complete_quest_ = std::move(complete_fn);
    on_refresh_ = std::move(refresh_fn);
}

void QuestDialog::RefreshLists(GameState* state) {
    if (!avail_list_ || !quest_list_ || !done_list_) return;

    avail_list_->Clear();
    quest_list_->Clear();
    done_list_->Clear();

    current_quests_ = GetAvailableQuests(state->level);
    for (auto& q : current_quests_) {
        bool active = false;
        for (const auto& nq : state->network_quests) {
            if (nq.quest_id == q.id) { active = true; break; }
        }
        if (active) continue;
        bool done = std::find(state->completed_quest_ids.begin(),
                              state->completed_quest_ids.end(), q.id) != state->completed_quest_ids.end();
        if (done) continue;
        char buf[128];
        snprintf(buf, sizeof(buf), "[Lv.%d] %s", q.required_level, q.name.c_str());
        avail_list_->AddItem(buf);
    }

    if (!state->network_quests.empty()) {
        for (const auto& nq : state->network_quests) {
            char buf[128];
            snprintf(buf, sizeof(buf), "%s%s", nq.name.c_str(), nq.is_completed ? " (Ready)" : "");
            quest_list_->AddItem(buf);
        }
    } else {
        for (auto& q : state->quest_list) quest_list_->AddItem(q);
    }

    for (uint32_t cid : state->completed_quest_ids) {
        for (auto& q : current_quests_) {
            if (q.id == cid) {
                done_list_->AddItem(q.name);
                break;
            }
        }
    }
}

void QuestDialog::ShowNetworkQuestDetail(const GameState::NetworkQuestEntry& q) {
    if (!detail_label_) return;
    std::string obj_text;
    for (size_t i = 0; i < q.objectives.size(); ++i) {
        const auto& o = q.objectives[i];
        char ob[128];
        snprintf(ob, sizeof(ob), "\n  Obj %zu: %u/%u", i + 1, o.current, o.required);
        obj_text += ob;
    }
    char buf[512];
    snprintf(buf, sizeof(buf), "%s\n\nStatus: %s%s",
        q.name.c_str(),
        q.is_completed ? "Complete — turn in for reward" : "In progress",
        obj_text.c_str());
    detail_label_->SetText(buf);
    if (reward_label_) reward_label_->SetText("Server-authoritative quest");
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

    avail_list_ = new ListBox(0, 0, 240, 260);
    quest_list_ = new ListBox(0, 0, 240, 260);
    done_list_ = new ListBox(0, 0, 240, 260);
    tabs->AddTab("Available", avail_list_);
    tabs->AddTab("In Progress", quest_list_);
    tabs->AddTab("Completed", done_list_);
    RefreshLists(state);
    if (on_refresh_) on_refresh_();

    // Detail text area (right side)
    detail_label_ = window_->AddWidget<Label>("Select a quest to see details.", 270, 50, ColorPalette::TEXT_DARK);
    reward_label_ = window_->AddWidget<Label>("", 270, 240, ColorPalette::TEXT_BROWN);

    // Accept / Complete buttons
    auto* accept_btn = window_->AddWidget<Button>("Accept Quest", 270, 370, 110, 24);
    accept_btn->SetColors({40,80,40,220}, {80,130,80,220}, {30,50,30,220});
    accept_btn->OnEvent([this, state, tabs](const UIEvent& e) {
        if (e.type == UIEvent::Click) {
            if (tabs->GetActive() != 0 || !avail_list_) return;
            int sel = avail_list_->GetSelected();
            if (sel < 0) return;
            int shown = 0;
            for (auto& q : current_quests_) {
                bool active = false;
                for (const auto& nq : state->network_quests) {
                    if (nq.quest_id == q.id) { active = true; break; }
                }
                if (active) continue;
                bool done = std::find(state->completed_quest_ids.begin(),
                                      state->completed_quest_ids.end(), q.id) != state->completed_quest_ids.end();
                if (done) continue;
                if (shown++ != sel) continue;
                if (on_start_quest_) {
                    on_start_quest_(q.id);
                    return;
                }
                state->quest_list.push_back(q.name);
                state->chat_messages.push_back("Quest accepted: " + q.name);
                RefreshLists(state);
                return;
            }
        }
    });

    auto* complete_btn = window_->AddWidget<Button>("Complete", 390, 370, 100, 24);
    complete_btn->SetColors({80,80,40,220}, {130,130,80,220}, {50,50,30,220});
    complete_btn->OnEvent([this, state, tabs](const UIEvent& e) {
        if (e.type == UIEvent::Click) {
            if (tabs->GetActive() != 1 || !quest_list_) return;
            int sel = quest_list_->GetSelected();
            if (sel < 0) return;
            if (!state->network_quests.empty()) {
                if (sel < (int)state->network_quests.size()) {
                    const auto& nq = state->network_quests[sel];
                    if (on_complete_quest_ && nq.is_completed)
                        on_complete_quest_(nq.quest_id);
                }
                return;
            }
            std::string picked = quest_list_->GetItem(sel);
            for (auto& q : current_quests_) {
                if (q.name == picked && TryCompleteQuest(state, q)) {
                    RefreshLists(state);
                    if (detail_label_) detail_label_->SetText("Quest completed! Check rewards.");
                    break;
                }
            }
        }
    });
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
    RefreshLists(state);
}
