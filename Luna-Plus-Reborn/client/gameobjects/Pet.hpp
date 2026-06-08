#pragma once
#include <string>
#include <glm/glm.hpp>
#include <cstdint>

class UIRenderer;

class Pet {
public:
    Pet() = default;
    void Init(const std::string& name, uint32_t id);
    void ApplyNetworkState(uint16_t level, uint16_t hp, uint16_t max_hp, uint16_t satiation,
                           uint8_t evolution, uint32_t exp, uint32_t exp_to_next);
    void Update(float dt, float follow_x, float follow_y, float follow_z);
    void RenderOverhead(UIRenderer& ui);

    bool IsActive() const { return active_; }
    void Summon() { active_ = true; }
    void Dismiss() { active_ = false; }
    void Feed(int amount) { hunger_ = std::min(100, hunger_ + amount); }
    int GetLevel() const { return level_; }
    int GetHunger() const { return hunger_; }
    int GetSatiation() const { return hunger_; }
    int GetMaxHp() const { return max_hp_; }
    int GetEvolution() const { return evolution_; }
    uint32_t GetExp() const { return exp_; }
    uint32_t GetExpNext() const { return exp_next_; }
    const std::string& GetName() const { return name_; }

private:
    uint32_t id_ = 0;
    std::string name_ = "Pet";
    float x_ = 0, y_ = 1, z_ = 0;
    int level_ = 1;
    int max_hp_ = 100;
    int hunger_ = 100;
    int evolution_ = 1;
    uint32_t exp_ = 0;
    uint32_t exp_next_ = 50;
    float follow_dist_ = 2.0f;
    bool active_ = false;
    float timer_ = 0;
};
