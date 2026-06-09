#include "GuildSystem.h"
#include <spdlog/spdlog.h>
#include <algorithm>
#include <ctime>
#include <random>
#include <cstring>

GuildSystem* g_guild_system = nullptr;

GuildSystem::GuildSystem() {
    g_guild_system = this;
}

void GuildSystem::SetPacketSender(PacketSender sender) {
    send_packet_ = std::move(sender);
}

uint32_t GuildSystem::GetCharacterId(entt::entity entity) const {
    return static_cast<uint32_t>(entity);
}

bool GuildSystem::HasRankAccess(uint32_t guild_id, uint32_t character_id, uint8_t min_rank) const {
    auto git = guilds_.find(guild_id);
    if (git == guilds_.end()) return false;
    for (auto& m : git->second.members) {
        if (m.character_id == character_id && m.rank <= min_rank) return true;
    }
    return false;
}

bool GuildSystem::CreateGuild(entt::registry& reg, uint32_t founder_id, const std::string& founder_name, const std::string& name) {
    if (member_to_guild_.count(founder_id)) {
        spdlog::warn("GuildSystem: player {} already in a guild", founder_id);
        return false;
    }

    if (name.empty() || name.length() > 32) return false;
    for (auto& [id, g] : guilds_) {
        (void)id;
        if (g.name == name) {
            spdlog::warn("GuildSystem: guild name '{}' already exists", name);
            return false;
        }
    }

    uint32_t guild_id = next_guild_id_++;
    GuildData data;
    data.guild_id = guild_id;
    data.name = name;
    data.master_id = founder_id;
    data.level = 1;
    data.exp = 0;
    data.gold = 0;
    data.score = 0;

    GuildMember master;
    master.character_id = founder_id;
    master.name = founder_name;
    master.rank = GUILD_MASTER;
    master.is_online = true;
    master.joined_at = static_cast<uint32_t>(std::time(nullptr));
    master.contribution = 0;
    master.level = 1;
    data.members.push_back(master);

    guilds_[guild_id] = data;
    member_to_guild_[founder_id] = guild_id;
    warehouse_rank_access_[guild_id] = GUILD_MEMBER;

    spdlog::info("GuildSystem: guild '{}' (id={}) created by {}", name, guild_id, founder_name);
    return true;
}

bool GuildSystem::DisbandGuild(entt::registry& reg, uint32_t actor_id) {
    auto mit = member_to_guild_.find(actor_id);
    if (mit == member_to_guild_.end()) return false;
    uint32_t guild_id = mit->second;
    auto git = guilds_.find(guild_id);
    if (git == guilds_.end()) return false;

    auto& guild = git->second;
    auto master_it = std::find_if(guild.members.begin(), guild.members.end(),
        [&](const GuildMember& m) { return m.character_id == actor_id; });
    if (master_it == guild.members.end() || master_it->rank != GUILD_MASTER) return false;

    std::string guild_name = guild.name;
    BroadcastMessage(guild_id, "Guild '" + guild_name + "' has been disbanded.");

    for (auto& m : guild.members) {
        member_to_guild_.erase(m.character_id);
    }

    guilds_.erase(guild_id);
    guild_skills_.erase(guild_id);
    war_scores_.erase(guild_id);
    warehouse_rank_access_.erase(guild_id);

    spdlog::info("GuildSystem: guild '{}' (id={}) disbanded", guild_name, guild_id);
    return true;
}

bool GuildSystem::InviteMember(entt::registry& reg, uint32_t inviter_id, uint32_t invitee_id, const std::string& invitee_name) {
    auto mit = member_to_guild_.find(inviter_id);
    if (mit == member_to_guild_.end()) return false;
    uint32_t guild_id = mit->second;

    if (member_to_guild_.count(invitee_id)) return false;
    if (!HasRankAccess(guild_id, inviter_id, GUILD_SENIOR)) return false;

    auto& guild = guilds_[guild_id];
    if (guild.members.size() >= MAX_GUILD_MEMBERS) return false;

    for (auto& pi : pending_invites_) {
        if (pi.invitee_id == invitee_id) return false;
    }

    PendingInvite invite;
    invite.inviter_id = inviter_id;
    invite.invitee_id = invitee_id;
    invite.guild_id = guild_id;
    invite.timer = 0.0f;
    pending_invites_.push_back(invite);

    spdlog::info("GuildSystem: {} invited {} to guild '{}'", inviter_id, invitee_name, guild.name);
    return true;
}

bool GuildSystem::AcceptInvite(entt::registry& reg, uint32_t player_id) {
    for (auto it = pending_invites_.begin(); it != pending_invites_.end(); ++it) {
        if (it->invitee_id == player_id) {
            uint32_t guild_id = it->guild_id;
            auto git = guilds_.find(guild_id);
            if (git == guilds_.end()) { pending_invites_.erase(it); return false; }

            GuildMember member;
            member.character_id = player_id;
            member.name = "Player_" + std::to_string(player_id);
            member.rank = GUILD_MEMBER;
            member.is_online = true;
            member.joined_at = static_cast<uint32_t>(std::time(nullptr));
            member.contribution = 0;
            member.level = 1;
            git->second.members.push_back(member);
            member_to_guild_[player_id] = guild_id;

            BroadcastMessage(guild_id, "New member joined the guild!");
            BroadcastMemberUpdate(guild_id);
            pending_invites_.erase(it);

            spdlog::info("GuildSystem: player {} joined guild {}", player_id, guild_id);
            return true;
        }
    }
    return false;
}

void GuildSystem::DeclineInvite(uint32_t player_id) {
    pending_invites_.erase(
        std::remove_if(pending_invites_.begin(), pending_invites_.end(),
            [&](const PendingInvite& pi) { return pi.invitee_id == player_id; }),
        pending_invites_.end());
}

bool GuildSystem::KickMember(entt::registry& reg, uint32_t kicker_id, uint32_t target_id) {
    auto mit = member_to_guild_.find(kicker_id);
    if (mit == member_to_guild_.end()) return false;
    uint32_t guild_id = mit->second;

    auto git = guilds_.find(guild_id);
    if (git == guilds_.end()) return false;
    auto& guild = git->second;

    if (!HasRankAccess(guild_id, kicker_id, GUILD_VICEMASTER)) return false;
    if (kicker_id == target_id) return false;

    guild.members.erase(
        std::remove_if(guild.members.begin(), guild.members.end(),
            [&](const GuildMember& m) { return m.character_id == target_id; }),
        guild.members.end());
    member_to_guild_.erase(target_id);

    BroadcastMemberUpdate(guild_id);
    spdlog::info("GuildSystem: {} kicked from guild {}", target_id, guild_id);
    return true;
}

bool GuildSystem::LeaveGuild(entt::registry& reg, uint32_t player_id) {
    auto mit = member_to_guild_.find(player_id);
    if (mit == member_to_guild_.end()) return false;
    uint32_t guild_id = mit->second;

    auto git = guilds_.find(guild_id);
    if (git == guilds_.end()) return false;
    auto& guild = git->second;

    auto it = std::find_if(guild.members.begin(), guild.members.end(),
        [&](const GuildMember& m) { return m.character_id == player_id; });
    if (it == guild.members.end()) return false;
    if (it->rank == GUILD_MASTER) return false;

    guild.members.erase(it);
    member_to_guild_.erase(player_id);

    BroadcastMemberUpdate(guild_id);
    spdlog::info("GuildSystem: {} left guild {}", player_id, guild_id);
    return true;
}

bool GuildSystem::ChangeMaster(entt::registry& reg, uint32_t actor_id, uint32_t target_id) {
    auto mit = member_to_guild_.find(actor_id);
    if (mit == member_to_guild_.end()) return false;
    uint32_t guild_id = mit->second;

    auto git = guilds_.find(guild_id);
    if (git == guilds_.end()) return false;
    auto& guild = git->second;

    auto actor_it = std::find_if(guild.members.begin(), guild.members.end(),
        [&](const GuildMember& m) { return m.character_id == actor_id; });
    if (actor_it == guild.members.end() || actor_it->rank != GUILD_MASTER) return false;

    auto target_it = std::find_if(guild.members.begin(), guild.members.end(),
        [&](const GuildMember& m) { return m.character_id == target_id; });
    if (target_it == guild.members.end()) return false;

    actor_it->rank = GUILD_VICEMASTER;
    target_it->rank = GUILD_MASTER;
    guild.master_id = target_id;

    BroadcastMemberUpdate(guild_id);
    spdlog::info("GuildSystem: master changed to {} in guild {}", target_id, guild_id);
    return true;
}

bool GuildSystem::SetMemberRank(entt::registry& reg, uint32_t actor_id, uint32_t target_id, uint8_t new_rank) {
    auto mit = member_to_guild_.find(actor_id);
    if (mit == member_to_guild_.end()) return false;
    uint32_t guild_id = mit->second;

    auto git = guilds_.find(guild_id);
    if (git == guilds_.end()) return false;
    auto& guild = git->second;

    if (!HasRankAccess(guild_id, actor_id, GUILD_MASTER)) return false;

    auto target_it = std::find_if(guild.members.begin(), guild.members.end(),
        [&](const GuildMember& m) { return m.character_id == target_id; });
    if (target_it == guild.members.end()) return false;
    if (target_it->rank == GUILD_MASTER) return false;

    target_it->rank = new_rank;
    BroadcastMemberUpdate(guild_id);
    return true;
}

bool GuildSystem::SetNotice(entt::registry& reg, uint32_t actor_id, const std::string& notice) {
    auto mit = member_to_guild_.find(actor_id);
    if (mit == member_to_guild_.end()) return false;
    auto git = guilds_.find(mit->second);
    if (git == guilds_.end()) return false;
    if (!HasRankAccess(mit->second, actor_id, GUILD_VICEMASTER)) return false;
    git->second.notice = notice;
    return true;
}

bool GuildSystem::SetNickname(entt::registry& reg, uint32_t actor_id, uint32_t target_id, const std::string& nickname) {
    auto mit = member_to_guild_.find(actor_id);
    if (mit == member_to_guild_.end()) return false;
    auto git = guilds_.find(mit->second);
    if (git == guilds_.end()) return false;
    auto& guild = git->second;

    if (!HasRankAccess(guild.guild_id, actor_id, GUILD_SENIOR)) return false;

    auto target_it = std::find_if(guild.members.begin(), guild.members.end(),
        [&](const GuildMember& m) { return m.character_id == target_id; });
    if (target_it == guild.members.end()) return false;
    target_it->nickname = nickname;
    return true;
}

bool GuildSystem::SetMark(entt::registry& reg, uint32_t actor_id, const std::vector<uint8_t>& mark_data) {
    (void)reg;
    (void)actor_id;
    (void)mark_data;
    return true;
}

bool GuildSystem::DepositGold(entt::registry& reg, uint32_t player_id, uint32_t amount) {
    auto mit = member_to_guild_.find(player_id);
    if (mit == member_to_guild_.end()) return false;
    auto git = guilds_.find(mit->second);
    if (git == guilds_.end()) return false;
    git->second.gold += amount;
    return true;
}

bool GuildSystem::WithdrawGold(entt::registry& reg, uint32_t player_id, uint32_t amount) {
    auto mit = member_to_guild_.find(player_id);
    if (mit == member_to_guild_.end()) return false;
    auto git = guilds_.find(mit->second);
    if (git == guilds_.end()) return false;
    auto& guild = git->second;
    if (!HasRankAccess(guild.guild_id, player_id, GUILD_SENIOR)) return false;
    if (guild.gold < amount) return false;
    guild.gold -= amount;
    return true;
}

bool GuildSystem::LearnSkill(entt::registry& reg, uint32_t actor_id, uint32_t skill_id) {
    auto mit = member_to_guild_.find(actor_id);
    if (mit == member_to_guild_.end()) return false;
    uint32_t guild_id = mit->second;
    if (!HasRankAccess(guild_id, actor_id, GUILD_MASTER)) return false;

    auto& skills = guild_skills_[guild_id];
    for (auto& s : skills) {
        if (s.skill_id == skill_id) return false;
    }
    GuildSkillInfo skill;
    skill.skill_id = skill_id;
    skill.level = 1;
    skill.name = "Skill_" + std::to_string(skill_id);
    skills.push_back(skill);
    return true;
}

bool GuildSystem::UpgradeSkill(entt::registry& reg, uint32_t actor_id, uint32_t skill_id) {
    auto mit = member_to_guild_.find(actor_id);
    if (mit == member_to_guild_.end()) return false;
    uint32_t guild_id = mit->second;
    if (!HasRankAccess(guild_id, actor_id, GUILD_MASTER)) return false;

    auto& skills = guild_skills_[guild_id];
    for (auto& s : skills) {
        if (s.skill_id == skill_id) {
            if (s.level >= 5) return false;
            s.level++;
            return true;
        }
    }
    return false;
}

std::vector<GuildSkillInfo> GuildSystem::GetGuildSkills(uint32_t guild_id) const {
    auto it = guild_skills_.find(guild_id);
    return (it != guild_skills_.end()) ? it->second : std::vector<GuildSkillInfo>();
}

void GuildSystem::GuildChat(uint32_t actor_id, const std::string& message) {
    auto mit = member_to_guild_.find(actor_id);
    if (mit == member_to_guild_.end()) return;
    BroadcastMessage(mit->second, "[Guild] " + message);
}

void GuildSystem::BroadcastToGuild(uint32_t guild_id, uint16_t packet_type, const uint8_t* data, size_t len) {
    if (send_packet_) {
        auto git = guilds_.find(guild_id);
        if (git == guilds_.end()) return;
        for (auto& m : git->second.members) {
            if (m.is_online) {
                send_packet_(m.character_id, data, len);
            }
        }
    }
}

void GuildSystem::BroadcastMessage(uint32_t guild_id, const std::string& message) {
    spdlog::info("[Guild {}] {}", guild_id, message);
}

void GuildSystem::AddWarKillScore(uint32_t guild_id, uint32_t score) {
    war_scores_[guild_id].kill_score += score;
    war_scores_[guild_id].war_score += score;
}

void GuildSystem::AddWarDeathScore(uint32_t guild_id, uint32_t score) {
    war_scores_[guild_id].death_score += score;
    if (war_scores_[guild_id].war_score > score)
        war_scores_[guild_id].war_score -= score;
    else
        war_scores_[guild_id].war_score = 0;
}

uint32_t GuildSystem::GetWarScore(uint32_t guild_id) const {
    auto it = war_scores_.find(guild_id);
    return (it != war_scores_.end()) ? it->second.war_score : 0;
}

void GuildSystem::SetWarehouseRankAccess(uint32_t guild_id, uint8_t min_rank) {
    warehouse_rank_access_[guild_id] = min_rank;
}

bool GuildSystem::OpenWarehouse(entt::registry& reg, uint32_t player_id) {
    auto mit = member_to_guild_.find(player_id);
    if (mit == member_to_guild_.end()) return false;
    uint32_t guild_id = mit->second;

    uint8_t required = GUILD_MEMBER;
    auto rit = warehouse_rank_access_.find(guild_id);
    if (rit != warehouse_rank_access_.end()) required = rit->second;

    if (!HasRankAccess(guild_id, player_id, required)) return false;

    warehouse_viewers_[guild_id].push_back(player_id);
    return true;
}

void GuildSystem::CloseWarehouse(uint32_t player_id) {
    for (auto& [gid, viewers] : warehouse_viewers_) {
        (void)gid;
        viewers.erase(
            std::remove(viewers.begin(), viewers.end(), player_id),
            viewers.end());
    }
}

void GuildSystem::AddPlayer(entt::registry& reg, uint32_t character_id) {
    (void)reg;
    auto mit = member_to_guild_.find(character_id);
    if (mit == member_to_guild_.end()) return;
    auto git = guilds_.find(mit->second);
    if (git == guilds_.end()) return;
    for (auto& m : git->second.members) {
        if (m.character_id == character_id) {
            m.is_online = true;
            break;
        }
    }
}

void GuildSystem::RemovePlayer(entt::registry& reg, uint32_t character_id) {
    (void)reg;
    auto mit = member_to_guild_.find(character_id);
    if (mit == member_to_guild_.end()) return;
    auto git = guilds_.find(mit->second);
    if (git == guilds_.end()) return;
    for (auto& m : git->second.members) {
        if (m.character_id == character_id) {
            m.is_online = false;
            break;
        }
    }
    CloseWarehouse(character_id);
}

void GuildSystem::Update(entt::registry& reg, float dt) {
    (void)reg;
    for (auto it = pending_invites_.begin(); it != pending_invites_.end(); ) {
        it->timer += dt;
        if (it->timer >= it->timeout) {
            it = pending_invites_.erase(it);
        } else {
            ++it;
        }
    }

    static float score_timer = 0.0f;
    score_timer += dt;
    if (score_timer >= REGEN_SCORE_INTERVAL) {
        score_timer = 0.0f;
        for (auto& [gid, guild] : guilds_) {
            (void)gid;
            uint32_t gain = 1 + static_cast<uint32_t>(guild.members.size());
            guild.exp += gain;
        }
    }
}

uint32_t GuildSystem::GetGuildId(uint32_t character_id) const {
    auto it = member_to_guild_.find(character_id);
    return (it != member_to_guild_.end()) ? it->second : 0;
}

GuildData* GuildSystem::GetGuild(uint32_t guild_id) {
    auto it = guilds_.find(guild_id);
    return (it != guilds_.end()) ? &it->second : nullptr;
}

const GuildData* GuildSystem::GetGuildByMember(uint32_t character_id) const {
    auto mit = member_to_guild_.find(character_id);
    if (mit == member_to_guild_.end()) return nullptr;
    auto git = guilds_.find(mit->second);
    return (git != guilds_.end()) ? &git->second : nullptr;
}

std::string GuildSystem::GetGuildName(uint32_t guild_id) const {
    auto it = guilds_.find(guild_id);
    return (it != guilds_.end()) ? it->second.name : std::string();
}

void GuildSystem::BroadcastMemberUpdate(uint32_t guild_id) {
    auto git = guilds_.find(guild_id);
    if (git == guilds_.end()) return;
    spdlog::debug("GuildSystem: member update broadcast for guild {}", guild_id);
}
