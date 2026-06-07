#include "GuildSystem.hpp"
#include "../components/Guild.hpp"
#include "../components/Tag.hpp"
#include <spdlog/spdlog.h>
#include <algorithm>

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
    guild.members.push_back(gm);
    spdlog::info("Guild created: {} (id={})", name, guild.guild_id);
    return true;
}

bool GuildSystem::InviteToGuild(entt::registry& reg, entt::entity inviter, entt::entity invitee) {
    if (!reg.valid(inviter) || !reg.valid(invitee)) return false;
    if (!reg.all_of<Guild>(inviter)) return false;
    auto& guild = reg.get<Guild>(inviter);
    if (guild.members.size() >= 30) return false;
    invites_[invitee] = inviter;
    return true;
}

bool GuildSystem::AcceptInvite(entt::registry& reg, entt::entity player) {
    auto it = invites_.find(player);
    if (it == invites_.end() || !reg.valid(it->second)) return false;
    auto& guild = reg.get<Guild>(it->second);
    GuildMember gm;
    gm.character_id = static_cast<uint32_t>(player);
    gm.rank = 5;
    gm.is_online = true;
    guild.members.push_back(gm);
    reg.emplace<Guild>(player, guild);
    invites_.erase(it);
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
    if (!reg.valid(player)) return false;
    auto& guild = reg.get<Guild>(player);
    guild.gold += amount;
    return true;
}

bool GuildSystem::WithdrawGold(entt::registry& reg, entt::entity player, uint32_t amount) {
    if (!reg.valid(player)) return false;
    auto& guild = reg.get<Guild>(player);
    if (guild.gold < amount) return false;
    guild.gold -= amount;
    return true;
}

void GuildSystem::Update(entt::registry& reg, float dt) {
    (void)dt;
    auto view = reg.view<Guild>();
    for (auto entity : view) {
        auto& guild = view.get<Guild>(entity);
        guild.exp += 1;
        if (guild.exp >= guild.level * 1000) {
            guild.exp = 0;
            guild.level++;
        }
    }
}
