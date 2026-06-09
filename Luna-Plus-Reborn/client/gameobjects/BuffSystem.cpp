#include "BuffSystem.hpp"
#include <algorithm>
#include <cmath>

void BuffSystem::Update(float dt, CharacterStats& stats) {
    for (auto it = active_buffs_.begin(); it != active_buffs_.end(); ) {
        auto& buff = *it;

        // Clean up manually expired buffs (via Remove/RemoveAll etc.)
        if (buff.expired) {
            ApplyMods(buff, stats, false);
            if (buff.on_expire) buff.on_expire(stats);
            it = active_buffs_.erase(it);
            continue;
        }

        if (buff.permanent) { ++it; continue; }

        buff.elapsed += dt;

        if (buff.tick_interval > 0) {
            buff.tick_timer += dt;
            while (buff.tick_timer >= buff.tick_interval) {
                buff.tick_timer -= buff.tick_interval;
                if (buff.on_tick) buff.on_tick(stats, dt);
                if (buff.type == BuffType::DamageOverTime) {
                    int dmg = buff.tick_damage * buff.current_stacks;
                    stats.hp -= dmg;
                }
                if (buff.type == BuffType::HealOverTime) {
                    int heal = buff.tick_heal * buff.current_stacks;
                    stats.hp = std::min(stats.max_hp, stats.hp + heal);
                }
            }
        }

        if (!buff.permanent && buff.elapsed >= buff.duration) {
            if (buff.stack_rule == BuffStackRule::Intensity && buff.current_stacks > 1) {
                buff.current_stacks--;
                buff.elapsed = 0;
                RecalculateMods(buff, stats);
                if (buff.on_expire) buff.on_expire(stats);
            } else {
                ApplyMods(buff, stats, false);
                if (buff.on_expire) buff.on_expire(stats);
                it = active_buffs_.erase(it);
                continue;
            }
        }

        ++it;
    }
}

int BuffSystem::Apply(const BuffInstance& buff, CharacterStats& stats) {
    // Exclusive: only one buff per BuffType allowed
    if (buff.stack_rule == BuffStackRule::Exclusive) {
        for (auto& b : active_buffs_) {
            if (b.type == buff.type) {
                b.elapsed = 0;
                return b.id;
            }
        }
    }

    // None: same name refreshes duration, no stacking
    if (buff.stack_rule == BuffStackRule::None) {
        for (auto& b : active_buffs_) {
            if (b.name == buff.name) {
                b.elapsed = 0;
                return b.id;
            }
        }
    }

    // Intensity: increment stacks up to max_stacks
    if (buff.stack_rule == BuffStackRule::Intensity) {
        for (auto& b : active_buffs_) {
            if (b.name == buff.name) {
                if (b.current_stacks < b.max_stacks) {
                    BuffInstance one_stack = b;
                    one_stack.current_stacks = 1;
                    b.current_stacks++;
                    b.elapsed = 0;
                    ApplyMods(one_stack, stats, true);
                } else {
                    b.elapsed = 0;
                }
                if (b.on_apply) b.on_apply(stats);
                return b.id;
            }
        }
    }

    // Full: always add a new independent instance (no early-out)

    BuffInstance new_buff = buff;
    new_buff.id = next_id_++;
    ApplyMods(new_buff, stats, true);
    if (new_buff.on_apply) new_buff.on_apply(stats);
    active_buffs_.push_back(new_buff);
    return new_buff.id;
}

void BuffSystem::Remove(int buff_id) {
    for (auto& b : active_buffs_) {
        if (b.id == buff_id) {
            b.expired = true;
            break;
        }
    }
}

void BuffSystem::RemoveByType(BuffType type) {
    for (auto& b : active_buffs_) {
        if (b.type == type) b.expired = true;
    }
}

void BuffSystem::RemoveByName(const std::string& name) {
    for (auto& b : active_buffs_) {
        if (b.name == name) b.expired = true;
    }
}

void BuffSystem::RemoveAll() {
    for (auto& b : active_buffs_) b.expired = true;
}

void BuffSystem::ClearExpired() {
    active_buffs_.erase(std::remove_if(active_buffs_.begin(), active_buffs_.end(),
        [](auto& b) { return b.expired; }), active_buffs_.end());
}

bool BuffSystem::HasBuff(int buff_id) const {
    for (auto& b : active_buffs_) if (b.id == buff_id && !b.expired) return true;
    return false;
}

bool BuffSystem::HasBuffType(BuffType type) const {
    for (auto& b : active_buffs_) if (b.type == type && !b.expired) return true;
    return false;
}

bool BuffSystem::HasBuffName(const std::string& name) const {
    for (auto& b : active_buffs_) if (b.name == name && !b.expired) return true;
    return false;
}

int BuffSystem::GetStackCount(int buff_id) const {
    for (auto& b : active_buffs_) if (b.id == buff_id && !b.expired) return b.current_stacks;
    return 0;
}

void BuffSystem::ApplyMods(const BuffInstance& buff, CharacterStats& stats, bool apply) {
    float mult = apply ? 1.0f : -1.0f;
    float stacks = (float)buff.current_stacks;
    for (auto& mod : buff.mods) {
        float value = mod.value * stacks * mult;
        if (mod.stat_name == "str") stats.str += (int)value;
        else if (mod.stat_name == "dex") stats.dex += (int)value;
        else if (mod.stat_name == "vit") stats.vit += (int)value;
        else if (mod.stat_name == "int") stats.intel += (int)value;
        else if (mod.stat_name == "attack") stats.attack += mod.percent ? (int)(stats.attack * value / 100.0f) : (int)value;
        else if (mod.stat_name == "defense") stats.defense += mod.percent ? (int)(stats.defense * value / 100.0f) : (int)value;
        else if (mod.stat_name == "magic_attack") stats.magic_attack += mod.percent ? (int)(stats.magic_attack * value / 100.0f) : (int)value;
        else if (mod.stat_name == "magic_defense") stats.magic_defense += mod.percent ? (int)(stats.magic_defense * value / 100.0f) : (int)value;
        else if (mod.stat_name == "move_speed") stats.move_speed += value;
        else if (mod.stat_name == "attack_speed") stats.attack_speed += value;
        else if (mod.stat_name == "crit_rate") stats.crit_rate += value;
        else if (mod.stat_name == "crit_damage") stats.crit_damage += value;
        else if (mod.stat_name == "dodge_rate") stats.dodge_rate += value;
        else if (mod.stat_name == "block_rate") stats.block_rate += value;
        else if (mod.stat_name == "hp_regen") stats.hp_regen += (int)value;
        else if (mod.stat_name == "mp_regen") stats.mp_regen += (int)value;
    }
}

void BuffSystem::RecalculateMods(BuffInstance& buff, CharacterStats& stats) {
    BuffInstance one_stack = buff;
    one_stack.current_stacks = 1;
    ApplyMods(one_stack, stats, false);
}

BuffInstance BuffSystem::MakeStatBuff(const std::string& name, float duration, const std::string& stat, float value, bool percent) {
    BuffInstance b; b.name = name; b.type = BuffType::StatModifier; b.duration = duration;
    b.mods.push_back({stat, value, percent}); b.stack_rule = BuffStackRule::Exclusive; return b;
}

BuffInstance BuffSystem::MakeDOT(const std::string& name, float duration, int tick_dmg, float interval) {
    BuffInstance b; b.name = name; b.type = BuffType::DamageOverTime; b.duration = duration;
    b.tick_damage = tick_dmg; b.tick_interval = interval; return b;
}

BuffInstance BuffSystem::MakeHOT(const std::string& name, float duration, int tick_heal, float interval) {
    BuffInstance b; b.name = name; b.type = BuffType::HealOverTime; b.duration = duration;
    b.tick_heal = tick_heal; b.tick_interval = interval; return b;
}

BuffInstance BuffSystem::MakeStun(float duration) {
    BuffInstance b; b.name = "Stun"; b.type = BuffType::Stun; b.duration = duration;
    b.stack_rule = BuffStackRule::Exclusive; return b;
}

BuffInstance BuffSystem::MakeSilence(float duration) {
    BuffInstance b; b.name = "Silence"; b.type = BuffType::Silence; b.duration = duration;
    b.stack_rule = BuffStackRule::Exclusive; return b;
}
