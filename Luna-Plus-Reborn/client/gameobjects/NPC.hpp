#pragma once
#include <string>
#include <glm/glm.hpp>
#include <cstdint>
#include <functional>

class UIRenderer;

enum class NPCType { Shop, QuestGiver, Blacksmith, Storage, GuildManager, None };

class NPC {
public:
    NPC() = default;
    NPC(uint32_t id, const std::string& name, float x, float z, NPCType type);
    
    void Update(float dt, float player_x, float player_z);
    void RenderOverhead(UIRenderer& ui);
    bool IsInRange(float px, float pz) const;
    void Interact();
    
    uint32_t GetID() const { return id_; }
    const std::string& GetName() const { return name_; }
    NPCType GetType() const { return type_; }
    float GetX() const { return x_; }
    float GetZ() const { return z_; }
    bool HasQuest() const { return has_quest_; }
    void SetHasQuest(bool q) { has_quest_ = q; }

private:
    uint32_t id_ = 0;
    std::string name_;
    NPCType type_ = NPCType::None;
    float x_ = 0, z_ = 0;
    float interact_range_ = 5.0f;
    bool has_quest_ = false;
    std::string greeting_;
    std::function<void()> on_interact_;
};
