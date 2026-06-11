#include "Hero.hpp"
#include "NavMeshSystem.hpp"
#include "ClassAdvancement.hpp"
#include <ui/GameState.hpp>
#include <rendering/UIRenderer.hpp>
#include <rendering/CharacterRenderer.hpp>
#include <audio/AudioManager.hpp>
#include <game/data/ItemModelTable.hpp>
#include <game/ecs/components/Equipment.hpp>
#include <engine/physics/PhysicsWorld.h>
#include <engine/EngineMap.hpp>
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>

void Hero::UpdateEquipment() {
    if (!game_state_) return;
    
    // Demo: force a weapon if slots are empty
    bool has_weapon = false;
    for (int i = 0; i < Equipment::COUNT; i++) {
        if (game_state_->equipment.items[i] != 0) {
            has_weapon = true;
            break;
        }
    }
    
    if (!has_weapon) {
        game_state_->equipment.items[Equipment::Weapon] = 1001; // Sword
    }

    // Update attachments
    for (int i = 0; i < Equipment::COUNT; i++) {
        uint32_t item_id = game_state_->equipment.items[i];
        if (item_id != 0) {
            auto info = Luna::GetItemModel(item_id);
            if (!info.model_path.empty()) {
                CharRenderer_Attach(0, info.model_path, info.bone_name);
            }
        }
    }
}

void Hero::Init(GameState* state, AudioManager* audio, PhysicsWorld* physics) {
    game_state_ = state;
    audio_ = audio;
    physics_world_ = physics;
    if (state) {
        name_ = state->name;
        level_ = state->level;
        hp_ = state->hp; max_hp_ = state->max_hp;
        mp_ = state->mp; max_mp_ = state->max_mp;
        exp_ = state->exp; exp_next_ = state->exp_next;
        gold_ = state->gold;
        attack_ = state->attack; defense_ = state->defense;
        x_ = state->player_x; y_ = state->player_y; z_ = state->player_z;
    }
    state_ = HeroState::Idle;
    prev_state_ = HeroState::Idle;
    state_timer_ = 0;
    state_duration_ = 0;
    target_entity_ = 0;
    attack_cooldown_ = 0;
    skill_cooldown_ = 0;
    dash_cooldown_ = 0;
    pk_flagged_ = false;
    bad_fame_ = 0;
    pk_protection_timer_ = 0.0f;

    // Create physics character
    if (physics_world_) {
        physics_char_id_ = physics_world_->CreateCharacter(glm::vec3(x_, y_, z_), 0.4f, 1.8f);
    }
    
    UpdateEquipment();
}

void Hero::SetWaypoint(float x, float z) {
    waypoint_x_ = x;
    waypoint_z_ = z;
    has_waypoint_ = true;
}

void Hero::ClearWaypoint() {
    has_waypoint_ = false;
}

void Hero::SetPosition(float x, float y, float z) {
    x_ = x; y_ = y; z_ = z;
    CharRenderer_Move(0, x_, y_, z_, moving_);
    if (physics_world_ && physics_char_id_ >= 0) {
        physics_world_->SetCharacterPosition(physics_char_id_, glm::vec3(x_, y_, z_));
    }
}

void Hero::Move(float dx, float dz, float dt) {
    if (!CanAct()) {
        if (state_ == HeroState::Knockback) { prev_x_ = x_; prev_z_ = z_; x_ += dx; z_ += dz; }
        return;
    }
    // Anti-speedhack: cap movement speed
    float max_speed = 150.0f; // units/sec
    if (state_ == HeroState::Dash) max_speed = 350.0f; // dash speed boost
    float dist = std::sqrt(dx * dx + dz * dz);
    float max_delta = max_speed * dt;
    if (dist > max_delta) {
        float scale = max_delta / dist;
        dx *= scale; dz *= scale;
        dist = max_delta;
    }
    prev_x_ = x_; prev_z_ = z_;
    x_ += dx; z_ += dz;
    state_ = (state_ == HeroState::Dash) ? HeroState::Dash :
             (dist > 4.5f * dt) ? HeroState::Run : HeroState::Walk;
}

void Hero::SetState(HeroState s, float duration) {
    if (s != state_) {
        prev_state_ = state_;
        state_ = s;
        state_timer_ = 0;
        state_duration_ = duration;
    }
}

bool Hero::UseSkill(int skill_id) {
    if (!CanAct() || !IsAlive() || skill_id <= 0) return false;
    if (skill_cooldown_ > 0.0f) return false;

    int mp_cost = 8;
    auto skills = ClassAdvancement::GetSkillsForClass(game_state_ ? game_state_->class_id : 0);
    for (const auto& sk : skills) {
        if (sk.skill_id == skill_id) {
            mp_cost = std::max(5, sk.required_level * 2);
            break;
        }
    }
    if (mp_ < mp_cost) return false;

    mp_ -= mp_cost;
    current_skill_id_ = skill_id;
    skill_cooldown_ = 2.5f;
    cast_time_ = 0.4f;
    SetState(HeroState::Casting, cast_time_);
    if (game_state_) game_state_->pending_skill_id = static_cast<uint32_t>(skill_id);
    if (audio_) audio_->PlaySFXByCategory(AudioManager::SFX_UI, "button_ok.wav");
    return true;
}

void Hero::ApplyServerStats(int hp, int max_hp, int mp, int max_mp, int gold, uint64_t exp) {
    int prev_hp = hp_;
    hp_ = hp;
    max_hp_ = max_hp;
    mp_ = mp;
    max_mp_ = max_mp;
    gold_ = gold;
    if (exp > 0) exp_ = static_cast<int>(exp);
    if (game_state_) {
        game_state_->hp = hp_;
        game_state_->max_hp = max_hp_;
        game_state_->mp = mp_;
        game_state_->max_mp = max_mp_;
        game_state_->gold = gold_;
        if (exp > 0) game_state_->exp = static_cast<int>(exp);
    }
    if (hp_ < prev_hp && hp_ > 0)
        SetState(HeroState::Hit, 0.2f);
    else if (hp_ <= 0)
        SetState(HeroState::Die);
}

void Hero::TakeDamage(int dmg) {
    if (!IsAlive()) return;
    hp_ = std::max(0, hp_ - dmg);
    SetState(HeroState::Hit, 0.2f);
    if (hp_ <= 0) {
        SetState(HeroState::Die);
        if (audio_) audio_->PlaySFXByCategory(AudioManager::SFX_Monster, "die.wav");
    }
}

void Hero::TakeDamage(int dmg, int stun_duration_ms) {
    if (!IsAlive()) return;
    hp_ = std::max(0, hp_ - dmg);
    if (stun_duration_ms > 0) {
        SetState(HeroState::Stun, stun_duration_ms / 1000.0f);
    } else {
        SetState(HeroState::Hit, 0.2f);
    }
    if (hp_ <= 0) {
        SetState(HeroState::Die);
    }
}

void Hero::Knockback(float from_x, float from_z, float distance) {
    if (!IsAlive()) return;
    // Calculate knockback direction
    float dx = x_ - from_x;
    float dz = z_ - from_z;
    float len = std::sqrt(dx * dx + dz * dz);
    if (len > 0.01f) {
        kb_vx_ = (dx / len) * distance / 0.3f; // velocity over 300ms
        kb_vz_ = (dz / len) * distance / 0.3f;
    } else {
        kb_vx_ = 0; kb_vz_ = 0;
    }
    kb_distance_ = distance;
    kb_progress_ = 0;
    SetState(HeroState::Knockback, 0.3f);
}

void Hero::Heal(int amount) {
    hp_ = std::min(max_hp_, hp_ + amount);
}

void Hero::Dash() {
    if (!CanDash()) return;
    dash_cooldown_ = 8.0f;
    SetState(HeroState::Dash, 0.4f);
    if (audio_) audio_->PlaySFXByCategory(AudioManager::SFX_Character, "dash.wav");
}

void Hero::LevelUp() {
    level_++;
    exp_next_ = level_ * 500;
    max_hp_ = 500 + level_ * 20;
    hp_ = max_hp_;
    if (audio_) audio_->PlaySFXByCategory(AudioManager::SFX_UI, "button_ok.wav");
}

void Hero::Respawn() {
    hp_ = max_hp_ / 2;
    state_timer_ = 0;
    state_duration_ = 0;
    SetState(HeroState::Revive, 1.0f);
}

void Hero::ProcessStateTransitions(float dt) {
    state_timer_ += dt;
    
    switch (state_) {
    case HeroState::Casting:
        if (state_timer_ >= state_duration_) {
            SetState(HeroState::Skill, 0.5f);
        }
        break;
        
    case HeroState::Skill:
        if (state_timer_ >= state_duration_) {
            SetState(HeroState::Idle);
        }
        break;
        
    case HeroState::Hit:
        if (state_timer_ >= state_duration_) {
            SetState(HeroState::Idle);
        }
        break;
        
    case HeroState::Stun:
        if (state_timer_ >= state_duration_) {
            SetState(prev_state_ != HeroState::Stun ? prev_state_ : HeroState::Idle);
        }
        break;
        
    case HeroState::Knockback:
        // Apply knockback movement
        kb_progress_ = std::min(1.0f, state_timer_ / state_duration_);
        x_ += kb_vx_ * dt;
        z_ += kb_vz_ * dt;
        if (state_timer_ >= state_duration_) {
            SetState(HeroState::Idle);
        }
        break;
        
    case HeroState::Die:
        // Stay dead until resurrected
        break;
        
    case HeroState::Revive:
        if (state_timer_ >= state_duration_) {
            SetState(HeroState::Idle);
        }
        break;
        
    case HeroState::Dash:
        if (state_timer_ >= state_duration_) {
            SetState(HeroState::Idle);
        }
        break;

    case HeroState::Sit:
        // Stay sitting until key press
        break;

    default:
        break;
    }
}

void Hero::ExecuteAutoAttack() {
    if (!game_state_) return;
    float atk = static_cast<float>(attack_);
    float def = static_cast<float>(defense_);
    float rnd = 0.9f + static_cast<float>(std::rand()) / RAND_MAX * 0.2f;
    float base_dmg = atk * rnd - def * 0.5f;
    int dmg = std::max(1, static_cast<int>(base_dmg));
    SetState(HeroState::Attack, 0.3f);
    if (audio_) audio_->PlaySFXByCategory(AudioManager::SFX_Monster, "hit.wav");
}

bool Hero::IsInAttackRange() const {
    if (!game_state_ || target_entity_ == 0) return false;
    if (battle_delay_timer_ > 0.0f) return false;
    for (const auto& e : game_state_->entities) {
        if (e.id == target_entity_) {
            float dx = e.x - x_, dz = e.z - z_;
            return std::sqrt(dx * dx + dz * dz) < 3.0f;
        }
    }
    return false;
}

void Hero::Update(float dt) {
    if (game_state_) {
        battle_delay_timer_ = game_state_->battle_delay_timer;
        if (game_state_->has_waypoint) {
            SetWaypoint(game_state_->waypoint_x, game_state_->waypoint_z);
            game_state_->has_waypoint = false;
        }
    }
    if (battle_delay_timer_ > 0.0f)
        battle_delay_timer_ = std::max(0.0f, battle_delay_timer_ - dt);
    if (skill_cooldown_ > 0.0f)
        skill_cooldown_ = std::max(0.0f, skill_cooldown_ - dt);
    if (dash_cooldown_ > 0.0f)
        dash_cooldown_ = std::max(0.0f, dash_cooldown_ - dt);
    if (pk_protection_timer_ > 0.0f)
        pk_protection_timer_ = std::max(0.0f, pk_protection_timer_ - dt);

    if (has_waypoint_ && CanAct()) {
        float dx = waypoint_x_ - x_, dz = waypoint_z_ - z_;
        float dist = std::sqrt(dx * dx + dz * dz);
        if (dist < 0.5f) {
            has_waypoint_ = false;
            SetState(HeroState::Idle);
        } else {
            // Old Luna run speed: a bit above monster chase speed (3.0-3.5),
            // so mobs can almost keep pace but the player can kite.
            float speed = 6.5f * dt;
            if (navmesh_) {
                auto seek = navmesh_->Seek({x_, z_}, {waypoint_x_, waypoint_z_}, speed / dt, dt);
                dx = seek.x - x_;
                dz = seek.y - z_;
            } else {
                dx = (dx / dist) * speed;
                dz = (dz / dist) * speed;
            }
            if (engine_map_ && engine_map_->IsLoaded()) {
                glm::vec3 from(x_, y_, z_);
                glm::vec3 to(x_ + dx, y_, z_ + dz);
                if (!engine_map_->TryMove(from, to, 0.5f, entt::null)) {
                    dx = 0.0f;
                    dz = 0.0f;
                }
            }
            Move(dx, dz, dt);
        }
    }

    // Auto-attack
    if (target_entity_ != 0 && IsAlive()) {
        attack_cooldown_ -= dt;
        if (attack_cooldown_ <= 0 && IsInAttackRange()) {
            ExecuteAutoAttack();
            attack_cooldown_ = 1.2f;
        }
    }

    ProcessStateTransitions(dt);
    
    moving_ = (fabs(x_ - prev_x_) > 0.01f || fabs(z_ - prev_z_) > 0.01f)
              && (state_ == HeroState::Walk || state_ == HeroState::Run);
    prev_x_ = x_; prev_z_ = z_;

    // Apply gravity: stick to terrain height
    if (physics_world_) {
        float terrain_y = physics_world_->GetTerrainHeight(x_, z_);
        y_ = terrain_y + 1.0f;
        if (physics_char_id_ >= 0) {
            physics_world_->SetCharacterPosition(physics_char_id_, glm::vec3(x_, y_, z_));
        }
    }

    CharAnim anim = CharAnim::Idle;
    if (state_ == HeroState::Attack || state_ == HeroState::Skill) anim = CharAnim::Attack;
    else if (state_ == HeroState::Die) anim = CharAnim::Die;
    else if (state_ == HeroState::Dash) anim = CharAnim::Walk;
    else if (moving_ || state_ == HeroState::Walk || state_ == HeroState::Run) anim = CharAnim::Walk;
    CharRenderer_Move(0, x_, y_, z_, moving_ || state_ == HeroState::Walk || state_ == HeroState::Run, anim);
    
    if (game_state_) {
        game_state_->player_x = x_; game_state_->player_y = y_; game_state_->player_z = z_;
        game_state_->battle_delay_timer = battle_delay_timer_;
        game_state_->hp = hp_; game_state_->max_hp = max_hp_;
        game_state_->mp = mp_; game_state_->max_mp = max_mp_;
        game_state_->exp = exp_; game_state_->exp_next = exp_next_;
        game_state_->level = level_; game_state_->gold = gold_;
        game_state_->attack = attack_; game_state_->defense = defense_;
        game_state_->name = name_;
    }
}

void Hero::Render(UIRenderer& ui) {
    if (!render_hud_) return;
    float hx = 10, hy = 10;
    TextureInfo atlas4 = ui.LoadTexture("ui_atlas4", "b4.png");
    
    if (bgfx::isValid(atlas4.handle)) {
        ui.DrawImageUV(hx, hy, 218, 71, atlas4.handle, 223.0f/1024.0f, 469.0f/1024.0f, 441.0f/1024.0f, 540.0f/1024.0f);
        ui.DrawImageUV(hx + 8, hy + 5, 58, 60, atlas4.handle, 226.0f/1024.0f, 542.0f/1024.0f, 284.0f/1024.0f, 602.0f/1024.0f);
        ui.DrawText(hx + 85, hy + 6, 0xffffffff, "%s", name_.c_str());
        ui.DrawText(hx + 65, hy + 6, 0xff00ffff, "%d", level_);
        
        // State indicator (show current state)
        const char* state_names[] = {
            "Idle", "Walk", "Run", "Attack", "Skill", "Casting",
            "Hit", "Stun", "Knockback", "Die", "Sit", "Revive", "Dash"
        };
        ui.DrawText(hx + 140, hy + 6, 0xff88ff88, "[%s]", state_names[(int)state_]);

        // PK Flag indicator
        if (pk_flagged_) {
            ui.DrawText(hx + 85, hy + 75, 0xffff4444, "PK FLAGGED");
        }
        
        float hp_pct = (float)hp_ / std::max(1, max_hp_);
        float mp_pct = (float)mp_ / std::max(1, max_mp_);
        float xp_pct = (float)exp_ / std::max(1, exp_next_);
        
        float hp_w = 60.0f * hp_pct;
        if (hp_w > 0) {
            ui.DrawImageUV(hx + 80, hy + 22, hp_w, 11, atlas4.handle, 674.0f/1024.0f, 948.0f/1024.0f, (674.0f + hp_w)/1024.0f, 959.0f/1024.0f, {255,100,100,255});
        }
        float mp_w = 60.0f * mp_pct;
        if (mp_w > 0) {
            ui.DrawImageUV(hx + 80, hy + 35, mp_w, 11, atlas4.handle, 674.0f/1024.0f, 948.0f/1024.0f, (674.0f + mp_w)/1024.0f, 959.0f/1024.0f, {100,100,255,255});
        }
        float xp_w = 114.0f * xp_pct;
        if (xp_w > 0) {
            ui.DrawImageUV(hx + 80, hy + 50, xp_w, 8, atlas4.handle, 674.0f/1024.0f, 948.0f/1024.0f, (674.0f + xp_w)/1024.0f, 959.0f/1024.0f, {255,200,50,255});
        }
        
        // Stun indicator overlay
        if (state_ == HeroState::Stun) {
            ui.DrawText(hx + 85, hy + 65, 0xffff4444, "STUNNED (%.1fs)", state_duration_ - state_timer_);
        } else if (state_ == HeroState::Casting) {
            ui.DrawText(hx + 85, hy + 65, 0xff88ff88, "CASTING...");
        } else if (state_ == HeroState::Knockback) {
            ui.DrawText(hx + 85, hy + 65, 0xffff8800, "KNOCKBACK!");
        } else if (state_ == HeroState::Dash) {
            ui.DrawText(hx + 85, hy + 65, 0xff44ff44, "DASH!");
        }
    } else {
        ui.DrawText(hx + 1, hy + 1, 0x88000000, "%s  Lv.%d", name_.c_str(), level_);
        ui.DrawText(hx, hy, 0xffffffff, "%s  Lv.%d", name_.c_str(), level_);
        
        const char* state_names[] = {
            "Idle", "Walk", "Run", "Attack", "Skill", "Casting",
            "Hit", "Stun", "Knockback", "Die", "Sit", "Revive", "Dash"
        };
        ui.DrawText(hx + 120, hy, 0xff88ff88, "[%s]", state_names[(int)state_]);

        if (pk_flagged_) {
            ui.DrawText(hx + 170, hy, 0xffff4444, "[PK]");
        }
        
        float hp_pct = (float)hp_ / std::max(1, max_hp_);
        float mp_pct = (float)mp_ / std::max(1, max_mp_);
        float xp_pct = (float)exp_ / std::max(1, exp_next_);
        
        ui.DrawBar(hx, hy+20, 220, 18, hp_pct, {255, 60, 60, 255}, {60, 0, 0, 180});
        ui.DrawText(hx + 4, hy + 21, 0xffffffff, "HP %d/%d", hp_, max_hp_);
        ui.DrawBar(hx, hy+42, 220, 18, mp_pct, {60, 60, 255, 255}, {0, 0, 60, 180});
        ui.DrawText(hx + 4, hy + 43, 0xffffffff, "MP %d/%d", mp_, max_mp_);
        ui.DrawBar(hx, hy+64, 220, 14, xp_pct, {255, 220, 60, 255}, {60, 40, 0, 180});
        ui.DrawText(hx + 4, hy + 64, 0xffffffff, "EXP %d/%d", exp_, exp_next_);
        
        // State indicators
        if (state_ == HeroState::Stun) {
            ui.DrawText(hx, hy+82, 0xffff4444, "STUNNED (%.1fs)", state_duration_ - state_timer_);
        } else if (state_ == HeroState::Casting) {
            ui.DrawText(hx, hy+82, 0xff88ff88, "CASTING...");
        } else if (state_ == HeroState::Knockback) {
            ui.DrawText(hx, hy+82, 0xffff8800, "KNOCKBACK!");
        } else if (state_ == HeroState::Dash) {
            ui.DrawText(hx, hy+82, 0xff44ff44, "DASH!");
        }
        
        ui.DrawText(hx, hy+96, 0xffcccccc, "Gold: %d", gold_);
        if (bad_fame_ > 0) {
            ui.DrawText(hx, hy+110, 0xffff6644, "BadFame: %d", bad_fame_);
        }
    }
}
