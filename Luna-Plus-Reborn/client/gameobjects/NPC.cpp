#include "NPC.hpp"
#include <rendering/UIRenderer.hpp>
#include <cstdio>

NPC::NPC(uint32_t id, const std::string& name, float x, float z, NPCType type)
    : id_(id), name_(name), type_(type), x_(x), z_(z) {
    switch (type) {
    case NPCType::Shop: greeting_ = "\"Welcome! Browse my wares.\""; break;
    case NPCType::QuestGiver: greeting_ = "\"Need a task?\""; has_quest_ = true; break;
    case NPCType::Blacksmith: greeting_ = "\"Need something? I'm the Blacksmith.\""; break;
    case NPCType::Storage: greeting_ = "\"Store your items here.\""; break;
    case NPCType::GuildManager: greeting_ = "\"Looking for a guild?\""; break;
    default: greeting_ = "\"Hello.\""; break;
    }
}

bool NPC::IsInRange(float px, float pz) const {
    float dx = px - x_, dz = pz - z_;
    return (dx * dx + dz * dz) < (interact_range_ * interact_range_);
}

void NPC::Interact() {
    if (on_interact_) on_interact_();
}

void NPC::Update(float dt, float player_x, float player_z) {
    (void)dt;
    // NPCs don't move; check player proximity for interaction highlighting
    (void)player_x; (void)player_z;
}

void NPC::RenderOverhead(UIRenderer& ui) {
    float sx = (x_ * 12.0f + 640.0f) - 20;
    float sy = (z_ * 12.0f + 360.0f) - 40;
    if (sx < -50 || sx > 1330 || sy < -50 || sy > 770) return;
    
    // NPC name
    uint32_t name_color = 0xffffcc88;
    const char* type_str = "";
    switch (type_) {
    case NPCType::Shop: type_str = "[Shop]"; name_color = 0xffffcc88; break;
    case NPCType::QuestGiver: type_str = "[Quest]"; name_color = 0xff44ff44; break;
    case NPCType::Blacksmith: type_str = "[Blacksmith]"; name_color = 0xffff8844; break;
    case NPCType::Storage: type_str = "[Storage]"; name_color = 0x8888ffff; break;
    case NPCType::GuildManager: type_str = "[Guild]"; name_color = 0xffff44ff; break;
    default: break;
    }
    
    char buf[128]; snprintf(buf, sizeof(buf), "%s %s", name_.c_str(), type_str);
    ui.DrawText(sx, sy, name_color, "%s", buf);
    
    // Quest indicator
    if (has_quest_) {
        ui.DrawText(sx + 60, sy, 0xff44ff44, "!");
    }
}
