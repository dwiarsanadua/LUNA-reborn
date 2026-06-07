#pragma once
#include <string>
#include <glm/glm.hpp>
#include <cstdint>

class UIRenderer;

class Pet {
public:
    Pet() = default;
    void Init(const std::string& name, uint32_t id);
    void Update(float dt, float follow_x, float follow_y, float follow_z);
    void RenderOverhead(UIRenderer& ui);
    
    bool IsActive() const { return active_; }
    void Summon() { active_ = true; }
    void Dismiss() { active_ = false; }
    void Feed(int amount) { hunger_ = std::min(100, hunger_ + amount); }
    int GetLevel() const { return level_; }
    int GetHunger() const { return hunger_; }
    int GetEvolution() const { return evolution_; }
    const std::string& GetName() const { return name_; }

private:
    uint32_t id_ = 0;
    std::string name_ = "Pet";
    float x_ = 0, y_ = 1, z_ = 0;
    int level_ = 1;
    int hunger_ = 100;
    int evolution_ = 1;
    float exp_ = 0;
    float follow_dist_ = 2.0f;
    bool active_ = false;
    float timer_ = 0;
};
