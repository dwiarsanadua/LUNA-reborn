#include "Hero.hpp"
#include <ui/GameState.hpp>
#include <rendering/UIRenderer.hpp>
#include <rendering/CharacterRenderer.hpp>
#include <audio/AudioManager.hpp>
#include <game/data/ItemModelTable.hpp>
#include <game/ecs/components/Equipment.hpp>
#include <algorithm>
#include <cmath>
#include <cstdio>

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

void Hero::Init(GameState* state, AudioManager* audio) {
    game_state_ = state;
    audio_ = audio;
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
    
    UpdateEquipment(); // Force visual update for demo
}

void Hero::SetPosition(float x, float y, float z) {
    x_ = x; y_ = y; z_ = z;
    CharRenderer_Move(0, x_, y_, z_, moving_);
}

void Hero::Move(float dx, float dz, float dt) {
    if (!CanAct()) {
        if (state_ == HeroState::Knockback) { prev_x_ = x_; prev_z_ = z_; x_ += dx; z_ += dz; }
        return;
    }
    // Anti-speedhack: cap movement speed
    float max_speed = 150.0f; // units/sec
    float dist = std::sqrt(dx * dx + dz * dz);
    float max_delta = max_speed * dt;
    if (dist > max_delta) {
        float scale = max_delta / dist;
        dx *= scale; dz *= scale;
        dist = max_delta;
    }
    prev_x_ = x_; prev_z_ = z_;
    x_ += dx; z_ += dz;
    state_ = (dist > 50.0f * dt) ? HeroState::Run : HeroState::Walk;
}

void Hero::SetState(HeroState s, float duration) {
    if (s != state_) {
        prev_state_ = state_;
        state_ = s;
        state_timer_ = 0;
        state_duration_ = duration;
    }
}

void Hero::UseSkill(int skill_id) {
    if (!CanAct() || !IsAlive()) return;
    current_skill_id_ = skill_id;
    cast_time_ = 0.3f; // 300ms cast time
    SetState(HeroState::Casting, cast_time_);
    if (audio_) audio_->PlaySFXByCategory(AudioManager::SFX_UI, "button_ok.wav");
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
        
    case HeroState::Sit:
        // Stay sitting until key press
        break;
        
    default:
        break;
    }
}

void Hero::Update(float dt) {
    ProcessStateTransitions(dt);
    
    moving_ = (fabs(x_ - prev_x_) > 0.01f || fabs(z_ - prev_z_) > 0.01f) 
              && state_ == HeroState::Walk;
    prev_x_ = x_; prev_z_ = z_;
    CharRenderer_Move(0, x_, y_, z_, moving_);
    
    if (game_state_) {
        game_state_->player_x = x_; game_state_->player_y = y_; game_state_->player_z = z_;
        game_state_->hp = hp_; game_state_->max_hp = max_hp_;
        game_state_->mp = mp_; game_state_->max_mp = max_mp_;
        game_state_->exp = exp_; game_state_->exp_next = exp_next_;
        game_state_->level = level_; game_state_->gold = gold_;
        game_state_->attack = attack_; game_state_->defense = defense_;
        game_state_->name = name_;
    }
}

void Hero::Render(UIRenderer& ui) {
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
            "Hit", "Stun", "Knockback", "Die", "Sit", "Revive"
        };
        ui.DrawText(hx + 140, hy + 6, 0xff88ff88, "[%s]", state_names[(int)state_]);
        
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
        }
    } else {
        ui.DrawText(hx + 1, hy + 1, 0x88000000, "%s  Lv.%d", name_.c_str(), level_);
        ui.DrawText(hx, hy, 0xffffffff, "%s  Lv.%d", name_.c_str(), level_);
        
        const char* state_names[] = {
            "Idle", "Walk", "Run", "Attack", "Skill", "Casting",
            "Hit", "Stun", "Knockback", "Die", "Sit", "Revive"
        };
        ui.DrawText(hx + 120, hy, 0xff88ff88, "[%s]", state_names[(int)state_]);
        
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
        }
        
        ui.DrawText(hx, hy+96, 0xffcccccc, "Gold: %d", gold_);
    }
}
