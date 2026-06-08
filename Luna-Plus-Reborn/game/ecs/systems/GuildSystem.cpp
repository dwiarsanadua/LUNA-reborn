#include "GuildSystem.hpp"
#include "../components/Guild.hpp"
#include "../components/Inventory.hpp"
#include "../components/Tag.hpp"
#include <spdlog/spdlog.h>
#include <algorithm>
#include <ctime>
#include <random>

GuildSystem::GuildSystem() {
    InitLevelTable();
}

void GuildSystem::InitLevelTable() {
    level_table_ = {
        {1, 0, 30},
        {2, 1000, 35},
        {3, 3000, 40},
        {4, 6000, 45},
        {5, 10000, 50},
        {6, 15000, 55},
        {7, 21000, 60},
        {8, 28000, 65},
        {9, 36000, 70},
        {10, 50000, 80}
    };
}

uint32_t GuildSystem::GetRequiredExp(uint32_t level) const {
    for (auto& entry : level_table_) {
        if (entry.level == level) return entry.exp_required;
    }
    return level * 5000;
}

bool GuildSystem::AddExp(entt::registry& reg, entt::entity guild_entity, uint32_t amount) {
    if (!reg.valid(guild_entity) || !reg.all_of<Guild>(guild_entity)) return false;
    auto& guild = reg.get<Guild>(guild_entity);
    guild.exp += amount;

    uint32_t required = GetRequiredExp(guild.level);
    while (guild.exp >= required && guild.level < 10) {
        guild.exp -= required;
        guild.level++;
        required = GetRequiredExp(guild.level);
        spdlog::info("Guild '{}' reached level {}", guild.name, guild.level);
    }
    return true;
}

bool GuildSystem::LevelUp(entt::registry& reg, entt::entity guild_entity) {
    if (!reg.valid(guild_entity) || !reg.all_of<Guild>(guild_entity)) return false;
    auto& guild = reg.get<Guild>(guild_entity);
    uint32_t required = GetRequiredExp(guild.level);
    if (guild.exp < required) return false;
    guild.exp -= required;
    guild.level++;
    spdlog::info("Guild '{}' manually leveled up to {}", guild.name, guild.level);
    return true;
}

bool GuildSystem::LearnGuildSkill(entt::registry& reg, entt::entity actor,
                                  uint32_t guild_entity_id, uint32_t skill_id) {
    auto guild_entity = static_cast<entt::entity>(guild_entity_id);
    if (!reg.valid(guild_entity) || !reg.all_of<Guild>(guild_entity)) return false;

    auto& skills = guild_skills_[guild_entity_id];
    for (auto& s : skills) {
        if (s.skill_id == skill_id) return false;
    }

    GuildSkill gs;
    gs.skill_id = skill_id;
    gs.level = 1;
    gs.name = "Skill_" + std::to_string(skill_id);
    skills.push_back(gs);

    spdlog::info("Guild learned skill {} at level 1", skill_id);
    return true;
}

bool GuildSystem::UpgradeGuildSkill(entt::registry& reg, entt::entity actor,
                                    uint32_t guild_entity_id, uint32_t skill_id) {
    auto guild_entity = static_cast<entt::entity>(guild_entity_id);
    if (!reg.valid(guild_entity) || !reg.all_of<Guild>(guild_entity)) return false;

    auto& skills = guild_skills_[guild_entity_id];
    for (auto& s : skills) {
        if (s.skill_id == skill_id) {
            if (s.level >= 5) return false;
            s.level++;
            spdlog::info("Guild skill {} upgraded to level {}", skill_id, s.level);
            return true;
        }
    }
    return false;
}

std::vector<GuildSkill> GuildSystem::GetGuildSkills(entt::registry& reg,
                                                     entt::entity guild_entity) const {
    if (!reg.valid(guild_entity)) return {};
    auto it = guild_skills_.find(static_cast<uint32_t>(guild_entity));
    return it != guild_skills_.end() ? it->second : std::vector<GuildSkill>();
}

bool GuildSystem::CreateGuild(entt::registry& reg, entt::entity founder, const std::string& name) {
    if (!reg.valid(founder) || reg.all_of<Guild>(founder)) return false;
    auto& guild = reg.emplace<Guild>(founder);
    guild.guild_id = next_guild_id_++;
    guild.name = name;
    guild.master_id = static_cast<uint32_t>(founder);
    GuildMember gm;
    gm.character_id = static_cast<uint32_t>(founder);
    gm.name = "Master";
    gm.rank = 1;
    gm.is_online = true;
    gm.joined_at = static_cast<uint32_t>(std::time(nullptr));
    guild.members.push_back(gm);

    guild_warehouses_[guild.guild_id] = std::vector<ItemSlot>(60);

    spdlog::info("Guild created: {} (id={})", name, guild.guild_id);
    return true;
}

bool GuildSystem::InviteToGuild(entt::registry& reg, entt::entity inviter, entt::entity invitee) {
    if (!reg.valid(inviter) || !reg.valid(invitee)) return false;
    if (!reg.all_of<Guild>(inviter)) return false;
    auto& guild = reg.get<Guild>(inviter);

    int max_members = 30;
    for (auto& entry : level_table_) {
        if (entry.level == guild.level) {
            max_members = entry.max_members;
            break;
        }
    }

    if (guild.members.size() >= static_cast<size_t>(max_members)) return false;
    invites_[invitee] = inviter;
    return true;
}

bool GuildSystem::AcceptInvite(entt::registry& reg, entt::entity player) {
    auto it = invites_.find(player);
    if (it == invites_.end() || !reg.valid(it->second)) return false;
    if (!reg.all_of<Guild>(it->second)) return false;
    auto& guild = reg.get<Guild>(it->second);
    GuildMember gm;
    gm.character_id = static_cast<uint32_t>(player);
    gm.rank = 5;
    gm.is_online = true;
    gm.joined_at = static_cast<uint32_t>(std::time(nullptr));
    gm.contribution = 0;
    guild.members.push_back(gm);
    reg.emplace<Guild>(player, guild);
    invites_.erase(it);
    spdlog::info("Player {} joined guild {}", static_cast<uint32_t>(player), guild.name);
    return true;
}

void GuildSystem::LeaveGuild(entt::registry& reg, entt::entity player) {
    if (!reg.valid(player) || !reg.all_of<Guild>(player)) return;
    auto& guild = reg.get<Guild>(player);
    guild.members.erase(
        std::remove_if(guild.members.begin(), guild.members.end(),
            [&](const GuildMember& m) { return m.character_id == static_cast<uint32_t>(player); }),
        guild.members.end());
    reg.remove<Guild>(player);
    spdlog::info("Player {} left guild {}", static_cast<uint32_t>(player), guild.name);
}

void GuildSystem::KickMember(entt::registry& reg, entt::entity kicker, uint32_t target_id) {
    if (!reg.valid(kicker) || !reg.all_of<Guild>(kicker)) return;
    auto& guild = reg.get<Guild>(kicker);
    auto& gm = guild.members;
    auto it = std::find_if(gm.begin(), gm.end(),
        [&](const GuildMember& m) { return m.character_id == static_cast<uint32_t>(kicker); });
    if (it == gm.end() || it->rank > 2) return;
    gm.erase(std::remove_if(gm.begin(), gm.end(),
        [&](const GuildMember& m) { return m.character_id == target_id; }), gm.end());
    spdlog::info("Player {} kicked from guild {}", target_id, guild.name);
}

bool GuildSystem::SetRank(entt::registry& reg, entt::entity actor, uint32_t target_id, uint8_t rank) {
    if (!reg.valid(actor) || !reg.all_of<Guild>(actor)) return false;
    auto& guild = reg.get<Guild>(actor);
    auto it = std::find_if(guild.members.begin(), guild.members.end(),
        [&](const GuildMember& m) { return m.character_id == target_id; });
    if (it == guild.members.end()) return false;
    it->rank = rank;
    return true;
}

void GuildSystem::SetNotice(entt::registry& reg, entt::entity actor, const std::string& notice) {
    if (!reg.valid(actor) || !reg.all_of<Guild>(actor)) return;
    auto& guild = reg.get<Guild>(actor);
    guild.notice = notice;
}

bool GuildSystem::DepositGold(entt::registry& reg, entt::entity player, uint32_t amount) {
    if (!reg.valid(player) || !reg.all_of<Guild>(player)) return false;
    auto& guild = reg.get<Guild>(player);

    if (reg.all_of<Inventory>(player)) {
        auto& inv = reg.get<Inventory>(player);
        if (inv.gold < amount) return false;
        inv.gold -= amount;
    }

    guild.gold += amount;
    AddAuditEntry(guild.guild_id, static_cast<uint32_t>(player), "deposit_gold", amount, guild.gold);
    return true;
}

bool GuildSystem::WithdrawGold(entt::registry& reg, entt::entity player, uint32_t amount) {
    if (!reg.valid(player) || !reg.all_of<Guild>(player)) return false;
    auto& guild = reg.get<Guild>(player);

    auto it = std::find_if(guild.members.begin(), guild.members.end(),
        [&](const GuildMember& m) { return m.character_id == static_cast<uint32_t>(player); });
    if (it == guild.members.end() || it->rank > 3) return false;

    if (guild.gold < amount) return false;
    guild.gold -= amount;

    if (reg.all_of<Inventory>(player)) {
        reg.get<Inventory>(player).gold += amount;
    }

    AddAuditEntry(guild.guild_id, static_cast<uint32_t>(player), "withdraw_gold",
                  -static_cast<int32_t>(amount), guild.gold);
    return true;
}

bool GuildSystem::DepositItem(entt::registry& reg, entt::entity player, uint32_t guild_entity_id,
                              uint32_t item_id, uint16_t count) {
    auto guild_entity = static_cast<entt::entity>(guild_entity_id);
    if (!reg.valid(player) || !reg.valid(guild_entity)) return false;
    if (!reg.all_of<Inventory>(player) || !reg.all_of<Guild>(guild_entity)) return false;

    auto& inv = reg.get<Inventory>(player);
    int32_t slot = inv.FindItem(item_id);
    if (slot < 0) return false;
    if (!inv.RemoveItem(static_cast<size_t>(slot), count)) return false;

    auto& warehouse = guild_warehouses_[guild_entity_id];
    for (auto& ws : warehouse) {
        if (ws.item_id == 0) {
            ws.item_id = item_id;
            ws.count = count;
            break;
        }
    }

    auto& guild = reg.get<Guild>(guild_entity);
    AddAuditEntry(guild.guild_id, static_cast<uint32_t>(player),
                  "deposit_item", static_cast<int32_t>(item_id), count);
    return true;
}

bool GuildSystem::WithdrawItem(entt::registry& reg, entt::entity player, uint32_t guild_entity_id,
                               uint32_t item_id, uint16_t count) {
    auto guild_entity = static_cast<entt::entity>(guild_entity_id);
    if (!reg.valid(player) || !reg.valid(guild_entity)) return false;
    if (!reg.all_of<Inventory>(player) || !reg.all_of<Guild>(guild_entity)) return false;

    auto& warehouse = guild_warehouses_[guild_entity_id];
    for (auto& ws : warehouse) {
        if (ws.item_id == item_id && ws.count >= count) {
            ws.count -= count;
            if (ws.count == 0) ws = ItemSlot{};
            reg.get<Inventory>(player).AddItem(item_id, count);

            auto& guild = reg.get<Guild>(guild_entity);
            AddAuditEntry(guild.guild_id, static_cast<uint32_t>(player),
                          "withdraw_item", -static_cast<int32_t>(item_id), count);
            return true;
        }
    }
    return false;
}

std::vector<AuditEntry> GuildSystem::GetAuditLog(entt::registry& reg,
                                                  entt::entity guild_entity,
                                                  uint32_t limit) const {
    if (!reg.valid(guild_entity)) return {};
    auto it = guild_audit_logs_.find(static_cast<uint32_t>(guild_entity));
    if (it == guild_audit_logs_.end()) return {};

    if (it->second.size() <= limit) return it->second;
    return std::vector<AuditEntry>(it->second.end() - limit, it->second.end());
}

void GuildSystem::AddAuditEntry(uint32_t guild_id, uint32_t character_id,
                                const std::string& action, int32_t amount, uint32_t balance) {
    AuditEntry entry;
    entry.timestamp = static_cast<uint32_t>(std::time(nullptr));
    entry.character_id = character_id;
    entry.action = action;
    entry.amount = amount;
    entry.balance_after = balance;
    guild_audit_logs_[guild_id].push_back(entry);

    if (guild_audit_logs_[guild_id].size() > 200) {
        guild_audit_logs_[guild_id].erase(guild_audit_logs_[guild_id].begin());
    }
}

void GuildSystem::Update(entt::registry& reg, float dt) {
    (void)dt;
    auto view = reg.view<Guild>();
    for (auto entity : view) {
        auto& guild = view.get<Guild>(entity);
        uint32_t gain = 1 + guild.members.size();
        AddExp(reg, entity, gain);
    }
}
