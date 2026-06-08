#include "Monster.hpp"
#include <rendering/UIRenderer.hpp>
#include <rendering/CharacterRenderer.hpp>
#include <engine/physics/PhysicsWorld.h>
#include <cstdlib>
#include <cmath>
#include <cstdio>
#include <algorithm>

Monster::Monster(uint32_t id, const std::string& name, float x, float z, int level, MonsterType type)
    : id_(id), name_(name), x_(x), z_(z), level_(level), type_(type) {
    
    switch (type_) {
    case MonsterType::Normal:
        max_hp_ = 50 + level * 10;
        aggro_range_ = 12.0f;
        move_speed_ = 3.0f;
        attack_damage_ = 5 + level;
        break;
    case MonsterType::Elite:
        max_hp_ = 200 + level * 25;
        aggro_range_ = 16.0f;
        move_speed_ = 3.5f;
        attack_damage_ = 10 + level * 2;
        color_ = 0xffcc8844;
        break;
    case MonsterType::Boss:
        max_hp_ = 5000;
        aggro_range_ = 25.0f;
        chase_range_ = 50.0f;
        move_speed_ = 2.5f;
        attack_damage_ = 30 + level * 3;
        attack_cooldown_ = 2.0f;
        color_ = 0xffff4444;
        // Default boss phases
        AddPhase({50, "Enrage", 1.8f, 1.3f, "", 0, "You will not defeat me!", false});
        AddPhase({25, "Berserk", 2.5f, 1.5f, "monster_placeholder", 2, "Minions, arise!", false});
        break;
    }
    
    hp_ = max_hp_;
    patrol_x_ = x;
    patrol_z_ = z;
    
    uint32_t colors[] = {0xff44cc44, 0xffcc4444, 0xffcccc44, 0xff44cccc, 0xffcc44cc};
    if (type_ == MonsterType::Normal) color_ = colors[id % 5];
    
    CharRenderer_Spawn(id_, "assets/models/monster_placeholder.glb", x_, 0, z_, color_);
}

float Monster::GetDistance(float px, float pz) const {
    float dx = px - x_, dz = pz - z_;
    return sqrtf(dx * dx + dz * dz);
}

void Monster::SetPosition(float x, float y, float z) {
    x_ = x; y_ = y; z_ = z;
    CharRenderer_Move(id_, x_, y_, z_, state_ == MonsterState::Chase || state_ == MonsterState::Attack,
                      (state_ == MonsterState::Chase || state_ == MonsterState::Attack) ? CHAR_WALK : CHAR_IDLE);
}

void Monster::TakeDamage(int dmg) {
    hp_ = std::max(0, hp_ - dmg);
    state_ = MonsterState::Hit;
    if (hp_ <= 0) { alive_ = false; state_ = MonsterState::Die; }
}

void Monster::AddToThreatTable(uint32_t entity_id, int damage) {
    for (auto& e : threat_table_) {
        if (e.id == entity_id) { e.damage += damage; return; }
    }
    threat_table_.push_back({entity_id, damage});
}

uint32_t Monster::GetTopThreat() const {
    if (threat_table_.empty()) return UINT32_MAX;
    uint32_t top_id = threat_table_[0].id;
    int top_dmg = threat_table_[0].damage;
    for (auto& e : threat_table_) {
        if (e.damage > top_dmg) { top_dmg = e.damage; top_id = e.id; }
    }
    return top_id;
}

void Monster::AddPhase(const BossPhase& phase) {
    phases_.push_back(phase);
}

void Monster::Update(float dt, float player_x, float player_z) {
    if (!alive_) { respawn_timer_ -= dt; return; }
    UpdateAI(dt, player_x, player_z);
    UpdateBossPhases();
}

static bool HasLineOfSight(PhysicsWorld* pw, float from_x, float from_y, float from_z,
                            float to_x, float to_y, float to_z) {
    if (!pw) return true;
    glm::vec3 from(from_x, from_y + 1.0f, from_z);
    glm::vec3 to(to_x, to_y + 1.0f, to_z);
    glm::vec3 hit;
    return !pw->RayCast(from, to, hit);
}

void Monster::UpdateAI(float dt, float px, float pz) {
    float dist = GetDistance(px, pz);
    
    switch (state_) {
    case MonsterState::Idle:
        // Check aggro (with line-of-sight)
        if (dist < aggro_range_ && HasLineOfSight(physics_world_, x_, y_, z_, px, 0, pz)) {
            state_ = MonsterState::Aggro;
            target_id_ = 0; // Player
        }
        // Random patrol
        patrol_timer_ -= dt;
        if (patrol_timer_ <= 0) {
            patrol_timer_ = 4.0f + (rand() % 4);
            patrol_x_ = patrol_x_ + (rand() % 10 - 5);
            patrol_z_ = patrol_z_ + (rand() % 10 - 5);
            state_ = MonsterState::Patrol;
        }
        break;
        
    case MonsterState::Patrol:
        // Move toward patrol point
        { float dx = patrol_x_ - x_, dz = patrol_z_ - z_;
        float pdist = sqrtf(dx * dx + dz * dz);
        if (pdist > 1.0f) {
            x_ += (dx / pdist) * move_speed_ * dt;
            z_ += (dz / pdist) * move_speed_ * dt;
        } else {
            state_ = MonsterState::Idle;
        }}
        // Check aggro during patrol (with line-of-sight)
        if (dist < aggro_range_ && HasLineOfSight(physics_world_, x_, y_, z_, px, 0, pz)) {
            state_ = MonsterState::Aggro;
            target_id_ = 0;
        }
        break;
        
    case MonsterState::Aggro:
        state_ = MonsterState::Chase;
        break;
        
    case MonsterState::Chase:
        if (dist > chase_range_) { state_ = MonsterState::Return; break; }
        if (dist < attack_range_) { state_ = MonsterState::Attack; attack_timer_ = attack_cooldown_; break; }
        ChaseTarget(dt, px, pz);
        break;
        
    case MonsterState::Attack:
        attack_timer_ -= dt;
        if (dist > attack_range_ + 1.0f) { state_ = MonsterState::Chase; break; }
        if (dist > chase_range_) { state_ = MonsterState::Return; break; }
        if (!HasLineOfSight(physics_world_, x_, y_, z_, px, 0, pz)) {
            state_ = MonsterState::Chase;
            break;
        }
        if (attack_timer_ <= 0) {
            attack_timer_ = attack_cooldown_;
            // Deal damage to target (handled by GameScreen combat)
        }
        break;
        
    case MonsterState::Hit:
        state_ = MonsterState::Chase;
        break;
        
    case MonsterState::Flee:
        FleeFromTarget(dt, px, pz);
        if (dist > flee_range_) state_ = MonsterState::Return;
        break;
        
    case MonsterState::Return:
        { float dx = patrol_x_ - x_, dz = patrol_z_ - z_;
        float rdist = sqrtf(dx * dx + dz * dz);
        if (rdist > 1.0f) {
            x_ += (dx / rdist) * move_speed_ * dt;
            z_ += (dz / rdist) * move_speed_ * dt;
        } else {
            hp_ = max_hp_; // Full heal on return
            state_ = MonsterState::Idle;
            target_id_ = UINT32_MAX;
        }}
        break;
        
    default: break;
    }
    
    // Flee check (at low HP)
    if (hp_ > 0 && hp_ < max_hp_ * flee_hp_pct_ / 100 && state_ != MonsterState::Flee && state_ != MonsterState::Return) {
        state_ = MonsterState::Flee;
    }
    
    CharRenderer_Move(id_, x_, y_, z_, 
                      state_ == MonsterState::Chase || state_ == MonsterState::Attack || state_ == MonsterState::Patrol,
                      (state_ == MonsterState::Chase || state_ == MonsterState::Attack) ? CHAR_WALK : CHAR_IDLE);
}

void Monster::ChaseTarget(float dt, float px, float pz) {
    float dx = px - x_, dz = pz - z_;
    float dist = sqrtf(dx * dx + dz * dz);
    if (dist < 0.1f) return;
    float speed = is_enraged_ ? move_speed_ * 1.5f : move_speed_;
    x_ += (dx / dist) * speed * dt;
    z_ += (dz / dist) * speed * dt;
}

void Monster::FleeFromTarget(float dt, float px, float pz) {
    float dx = x_ - px, dz = z_ - pz;
    float dist = sqrtf(dx * dx + dz * dz);
    if (dist < 0.1f) { x_ += 5; z_ += 5; return; }
    x_ += (dx / dist) * move_speed_ * 1.5f * dt;
    z_ += (dz / dist) * move_speed_ * 1.5f * dt;
}

void Monster::UpdateBossPhases() {
    if (type_ != MonsterType::Boss || phases_.empty()) return;
    int hp_pct = (hp_ * 100) / std::max(1, max_hp_);
    for (size_t i = 0; i < phases_.size(); i++) {
        if (hp_pct <= phases_[i].hp_threshold_pct && !phases_[i].enraged) {
            phases_[i].enraged = true;
            is_enraged_ = true;
            current_phase_ = (int)i + 1;
            // Phase effects applied by GameScreen
        }
    }
}

void Monster::RenderOverhead(UIRenderer& ui) {
    if (!alive_) return;
    float sx = (x_ * 12.0f + 640.0f) - 25;
    float sy = (z_ * 12.0f + 360.0f) - 50;
    if (sx < -50 || sx > 1330 || sy < -50 || sy > 770) return;
    
    float hp_pct = (float)hp_ / std::max(1, max_hp_);
    UIColor bar_color = {60, 200, 60, 200};
    if (hp_pct < 0.5f) bar_color = {255, 200, 60, 200};
    if (hp_pct < 0.25f) bar_color = {255, 60, 60, 200};
    if (is_enraged_) bar_color = {255, 0, 0, 255};
    
    // Type prefix
    const char* type_str = "";
    if (type_ == MonsterType::Elite) type_str = "[Elite] ";
    if (type_ == MonsterType::Boss) type_str = "[Boss] ";
    
    char buf[128];
    snprintf(buf, sizeof(buf), "%s%s Lv.%d", type_str, name_.c_str(), level_);
    uint32_t name_color = (type_ == MonsterType::Boss) ? 0xffff4444 : 0xffffffff;
    ui.DrawText(sx, sy, name_color, "%s", buf);
    ui.DrawBar(sx, sy + 14, 50, 6, hp_pct, bar_color, {40, 40, 40, 180});
    
    // Boss phase indicator
    if (type_ == MonsterType::Boss && current_phase_ > 0) {
        char phase_buf[32]; snprintf(phase_buf, sizeof(phase_buf), "Phase %d/%d", current_phase_, (int)phases_.size());
        ui.DrawText(sx, sy + 22, 0xffff4444, "%s", phase_buf);
    }
}

bool Monster::ShouldRespawn(float dt) {
    if (!alive_) { respawn_timer_ -= dt; return respawn_timer_ <= 0; }
    return false;
}

void Monster::Respawn(float new_x, float new_z) {
    x_ = new_x; z_ = new_z;
    hp_ = max_hp_;
    alive_ = true;
    state_ = MonsterState::Idle;
    target_id_ = UINT32_MAX;
    respawn_timer_ = 0;
    threat_table_.clear();
    for (auto& p : phases_) p.enraged = false;
    current_phase_ = 0;
    is_enraged_ = false;
    CharRenderer_Spawn(id_, "assets/models/monster_placeholder.glb", x_, 0, z_, color_);
}
