#pragma once
#include <string>
#include <glm/glm.hpp>
#include <cstdint>
#include <functional>

class UIRenderer;
struct GameState;
class AudioManager;
class PhysicsWorld;
class PKManagerDlg;

enum class HeroState {
    Idle, Walk, Run, Attack, Skill, Casting, Hit, Stun, Knockback, Die, Sit, Revive
};

class Hero {
public:
    Hero() = default;
    void Init(GameState* state, AudioManager* audio, PhysicsWorld* physics = nullptr);
    void Update(float dt);
    void Render(UIRenderer& ui);
    void SetRenderHud(bool v) { render_hud_ = v; }
    void UpdateEquipment();
    
    float GetX() const { return x_; }
    float GetZ() const { return z_; }
    float GetY() const { return y_; }
    HeroState GetState() const { return state_; }
    float GetStateTimer() const { return state_timer_; }
    
    void Move(float dx, float dz, float dt = 0.016f);
    void SetPosition(float x, float y, float z);
    void SetWaypoint(float x, float z);
    void ClearWaypoint();
    bool HasWaypoint() const { return has_waypoint_; }
    void TakeDamage(int dmg);
    void TakeDamage(int dmg, int stun_duration_ms);
    void Knockback(float from_x, float from_z, float distance);
    void Heal(int amount);
    void LevelUp();
    void Respawn();
    bool UseSkill(int skill_id);
    int GetCurrentSkillId() const { return current_skill_id_; }
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
    int GetAttack() const { return attack_; }
    int GetDefense() const { return defense_; }
    void AddGold(int g) { gold_ += g; }
    void AddEXP(int e) { exp_ += e; }
    void ApplyServerStats(int hp, int max_hp, int mp, int max_mp, int gold, uint64_t exp = 0);
    const std::string& GetName() const { return name_; }

    // Auto-attack
    void SetTarget(uint32_t entity_id) { target_entity_ = entity_id; }
    uint32_t GetTarget() const { return target_entity_; }
    void ExecuteAutoAttack();
    bool IsInAttackRange() const;

    void SetNavMesh(class NavMeshSystem* nav) { navmesh_ = nav; }

    // PK Integration
    void SetPKManager(PKManagerDlg* mgr) { pk_mgr_ = mgr; }
    bool IsPKFlagged() const { return pk_flagged_; }
    void SetPKFlagged(bool f) { pk_flagged_ = f; }

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

    // Auto-attack & cooldowns
    uint32_t target_entity_ = 0;
    float attack_cooldown_ = 0;
    float skill_cooldown_ = 0;
    float dash_cooldown_ = 0;
    float battle_delay_timer_ = 0.0f;

    bool render_hud_ = true;
    bool has_waypoint_ = false;
    float waypoint_x_ = 0, waypoint_z_ = 0;
    class NavMeshSystem* navmesh_ = nullptr;
    
    int hp_ = 500, max_hp_ = 500;
    int mp_ = 100, max_mp_ = 100;
    int exp_ = 0, exp_next_ = 500;
    int level_ = 10;
    int gold_ = 0;
    int attack_ = 50, defense_ = 20;
    std::string name_ = "Hero";
    
    GameState* game_state_ = nullptr;
    AudioManager* audio_ = nullptr;
    PhysicsWorld* physics_world_ = nullptr;
    int physics_char_id_ = -1;
    
    // PK state
    PKManagerDlg* pk_mgr_ = nullptr;
    bool pk_flagged_ = false;
    
    void ProcessStateTransitions(float dt);
};
