#pragma once
#include "NavMeshSystem.hpp"
#include <string>
#include <glm/glm.hpp>
#include <cstdint>
#include <vector>
#include <functional>

class UIRenderer;
class PhysicsWorld;

enum class MonsterState {
    Idle, Patrol, Aggro, Chase, Attack, Hit, Flee, Die, Respawn, Return
};

enum class MonsterType {
    Normal, Elite, Boss
};

struct BossPhase {
    int hp_threshold_pct;    // e.g., 75, 50, 25 → triggers at that HP %
    std::string enrage_skill;
    float damage_mult = 1.5f;
    float speed_mult = 1.2f;
    std::string summon_monster;
    int summon_count = 0;
    std::string speech;      // Boss yell text
    bool enraged = false;
};

class Monster {
public:
    Monster() = default;
    Monster(uint32_t id, const std::string& name, float x, float z, int level,
            MonsterType type = MonsterType::Normal, const std::string& model_path = "");
    
    void Update(float dt, float player_x = 0, float player_z = 0);
    void RenderOverhead(UIRenderer& ui, const glm::mat4* view = nullptr,
                        const glm::mat4* proj = nullptr);
    
    // Getters
    uint32_t GetID() const { return id_; }
    const std::string& GetName() const { return name_; }
    float GetX() const { return x_; }
    float GetY() const { return y_; }
    float GetZ() const { return z_; }
    int GetHP() const { return hp_; }
    int GetMaxHP() const { return max_hp_; }
    int GetLevel() const { return level_; }
    bool IsAlive() const { return alive_; }
    bool IsBoss() const { return type_ == MonsterType::Boss; }
    MonsterType GetType() const { return type_; }
    MonsterState GetState() const { return state_; }
    bool JustAggroed() const { return just_aggroed_; }
    void ClearAggroFlag() { just_aggroed_ = false; }
    void ForceAggro(float px, float pz);
    float GetDistance(float px, float pz) const;
    float GetAggroRange() const { return aggro_range_; }
    float GetMoveSpeed() const { return move_speed_; }
    bool IsFleeing() const { return state_ == MonsterState::Flee; }
    
    // Actions
    void SetPosition(float x, float y, float z);
    void TakeDamage(int dmg);
    void SetTarget(uint32_t target_id) { target_id_ = target_id; }
    uint32_t GetTarget() const { return target_id_; }
    void MarkDead() { alive_ = false; }
    bool ShouldRespawn(float dt);
    void Respawn(float new_x, float new_z);
    void AddToThreatTable(uint32_t entity_id, int damage);
    uint32_t GetTopThreat() const;
    
    // Boss phases
    const std::vector<BossPhase>& GetPhases() const { return phases_; }
    void AddPhase(const BossPhase& phase);
    int GetCurrentPhase() const { return current_phase_; }
    bool IsEnraged() const { return is_enraged_; }
    
    // Flee
    void SetFleeHP(int pct) { flee_hp_pct_ = pct; }
    int GetFleeHP() const { return flee_hp_pct_; }
    float GetFleeRange() const { return flee_range_; }
    void SetFleeRange(float r) { flee_range_ = r; }

    // Physics integration
    void SetPhysicsWorld(PhysicsWorld* pw) { physics_world_ = pw; }
    void SetNavMesh(class NavMeshSystem* nav) { navmesh_ = nav; }

private:
    uint32_t id_ = 0;
    std::string name_;
    float x_ = 0, y_ = 0.5f, z_ = 0;
    int hp_ = 100, max_hp_ = 100;
    int level_ = 1;
    MonsterType type_ = MonsterType::Normal;
    bool alive_ = true;
    float respawn_timer_ = 0;
    uint32_t target_id_ = UINT32_MAX;
    MonsterState state_ = MonsterState::Idle;
    
    // AI parameters
    float aggro_range_ = 12.0f;
    float chase_range_ = 30.0f;     // De-aggro if player runs beyond this
    float attack_range_ = 2.5f;
    float move_speed_ = 3.0f;
    float attack_cooldown_ = 1.5f;
    float attack_timer_ = 0;
    int attack_damage_ = 10;
    float patrol_timer_ = 0;
    float patrol_x_ = 0, patrol_z_ = 0;
    uint32_t color_ = 0xff44cc44;
    std::string model_path_;
    
    // Flee behavior
    int flee_hp_pct_ = 15;          // Flee at 15% HP
    float flee_range_ = 20.0f;
    
    // Threat table
    struct ThreatEntry { uint32_t id; int damage; };
    std::vector<ThreatEntry> threat_table_;
    
    // Physics / pathfinding
    PhysicsWorld* physics_world_ = nullptr;
    class NavMeshSystem* navmesh_ = nullptr;
    NavPath chase_path_;
    float path_replan_timer_ = 0.0f;

    // Boss phases
    std::vector<BossPhase> phases_;
    int current_phase_ = 0;
    bool is_enraged_ = false;
    float aggro_scan_timer_ = 0.0f;
    float battle_attack_timer_ = 0.0f;
    bool just_aggroed_ = false;
    
    void UpdateAI(float dt, float px, float pz);
    void UpdateBossPhases();
    void ChaseTarget(float dt, float px, float pz);
    void FleeFromTarget(float dt, float px, float pz);
};
