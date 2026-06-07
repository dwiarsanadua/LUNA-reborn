#pragma once
#include <string>
#include <glm/glm.hpp>
#include <cstdint>
#include <functional>

class UIRenderer;
struct GameState;
class AudioManager;

enum class HeroState {
    Idle, Walk, Run, Attack, Skill, Casting, Hit, Stun, Knockback, Die, Sit, Revive
};

class Hero {
public:
    Hero() = default;
    void Init(GameState* state, AudioManager* audio);
    void Update(float dt);
    void Render(UIRenderer& ui);
    void UpdateEquipment();
    
    float GetX() const { return x_; }
    float GetZ() const { return z_; }
    float GetY() const { return y_; }
    HeroState GetState() const { return state_; }
    float GetStateTimer() const { return state_timer_; }
    
    void Move(float dx, float dz, float dt = 0.016f);
    void SetPosition(float x, float y, float z);
    void TakeDamage(int dmg);
    void TakeDamage(int dmg, int stun_duration_ms);
    void Knockback(float from_x, float from_z, float distance);
    void Heal(int amount);
    void LevelUp();
    void Respawn();
    void UseSkill(int skill_id);
    void SetState(HeroState s, float duration = 0);
    
    bool IsAlive() const { return hp_ > 0; }
    bool IsMoving() const { return moving_; }
    bool IsStunned() const { return state_ == HeroState::Stun; }
    bool IsCasting() const { return state_ == HeroState::Casting; }
    bool CanAct() const { return IsAlive() && !IsStunned() && !IsCasting(); }
    
    int GetHP() const { return hp_; }
    int GetMaxHP() const { return max_hp_; }
    int GetMP() const { return mp_; }
    int GetMaxMP() const { return max_mp_; }
    int GetLevel() const { return level_; }
    int GetEXP() const { return exp_; }
    int GetExpNext() const { return exp_next_; }
    int GetGold() const { return gold_; }
    void AddGold(int g) { gold_ += g; }
    void AddEXP(int e) { exp_ += e; }
    const std::string& GetName() const { return name_; }

private:
    float x_ = 0, y_ = 3, z_ = 0;
    float prev_x_ = 0, prev_z_ = 0;
    bool moving_ = false;
    HeroState state_ = HeroState::Idle;
    HeroState prev_state_ = HeroState::Idle;
    float state_timer_ = 0;
    float state_duration_ = 0;
    
    // Knockback
    float kb_vx_ = 0, kb_vz_ = 0;
    float kb_distance_ = 0;
    float kb_progress_ = 0;
    
    // Skill casting
    int current_skill_id_ = 0;
    float cast_time_ = 0;
    
    int hp_ = 500, max_hp_ = 500;
    int mp_ = 100, max_mp_ = 100;
    int exp_ = 0, exp_next_ = 500;
    int level_ = 10;
    int gold_ = 0;
    int attack_ = 50, defense_ = 20;
    std::string name_ = "Hero";
    
    GameState* game_state_ = nullptr;
    AudioManager* audio_ = nullptr;
    
    void ProcessStateTransitions(float dt);
};
