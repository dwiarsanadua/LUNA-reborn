#include "PartySystem.hpp"
#include "../components/Party.hpp"
#include "../components/CharacterStats.hpp"
#include "../components/Inventory.hpp"
#include "../components/Tag.hpp"
#include <algorithm>
#include <random>
#include <spdlog/spdlog.h>

PartySystem::PartySystem() : rng_(std::random_device{}()) {}

bool PartySystem::CreateParty(entt::registry& reg, entt::entity leader) {
    if (!reg.valid(leader) || reg.all_of<Party>(leader)) return false;
    auto& party = reg.emplace<Party>(leader);
    party.party_id = next_party_id_++;
    party.leader_id = static_cast<uint32_t>(leader);
    party.is_distribution_free = false;
    PartyMember pm;
    pm.entity_id = static_cast<uint32_t>(leader);
    pm.character_id = static_cast<uint32_t>(leader);
    pm.name = "Leader";
    pm.is_online = true;
    if (reg.all_of<CharacterStats>(leader)) {
        auto& s = reg.get<CharacterStats>(leader);
        pm.level = s.level;
        pm.hp = s.hp; pm.max_hp = s.max_hp;
        pm.mp = s.mp; pm.max_mp = s.max_mp;
    }
    party.members.push_back(pm);
    spdlog::info("PartySystem: party created by entity {}", static_cast<uint32_t>(leader));
    return true;
}

bool PartySystem::InviteToParty(entt::registry& reg, entt::entity inviter, entt::entity invitee) {
    if (!reg.valid(inviter) || !reg.valid(invitee)) return false;
    if (!reg.all_of<Party>(inviter)) return false;
    auto& party = reg.get<Party>(inviter);
    if (party.members.size() >= 8) return false;

    for (auto it = invite_timers_.begin(); it != invite_timers_.end(); ) {
        if (it->invitee == invitee) {
            it = invite_timers_.erase(it);
        } else {
            ++it;
        }
    }

    InviteEntry entry;
    entry.inviter = inviter;
    entry.invitee = invitee;
    entry.timer = 0.0f;
    entry.timeout = 30.0f;
    entry.expired = false;
    invite_timers_.push_back(entry);

    invites_[invitee] = inviter;
    spdlog::info("PartySystem: {} invited {} to party",
                 static_cast<uint32_t>(inviter), static_cast<uint32_t>(invitee));
    return true;
}

bool PartySystem::AcceptInvite(entt::registry& reg, entt::entity player) {
    auto it = invites_.find(player);
    if (it == invites_.end() || !reg.valid(it->second)) return false;
    if (!reg.all_of<Party>(it->second)) return false;
    auto& party = reg.get<Party>(it->second);

    for (auto& entry : invite_timers_) {
        if (entry.invitee == player && entry.expired) return false;
    }

    PartyMember pm;
    pm.entity_id = static_cast<uint32_t>(player);
    pm.character_id = static_cast<uint32_t>(player);
    pm.is_online = true;
    if (reg.all_of<CharacterStats>(player)) {
        auto& s = reg.get<CharacterStats>(player);
        pm.level = s.level;
        pm.name = "Member";
    }
    party.members.push_back(pm);
    reg.emplace<Party>(player, party);
    invites_.erase(it);

    invite_timers_.erase(
        std::remove_if(invite_timers_.begin(), invite_timers_.end(),
            [&](const InviteEntry& e) { return e.invitee == player; }),
        invite_timers_.end());

    spdlog::info("PartySystem: player {} joined party {}", static_cast<uint32_t>(player), party.party_id);
    return true;
}

void PartySystem::DeclineInvite(entt::registry& reg, entt::entity player) {
    invites_.erase(player);
    invite_timers_.erase(
        std::remove_if(invite_timers_.begin(), invite_timers_.end(),
            [&](const InviteEntry& e) { return e.invitee == player; }),
        invite_timers_.end());
}

void PartySystem::LeaveParty(entt::registry& reg, entt::entity player) {
    if (!reg.valid(player) || !reg.all_of<Party>(player)) return;
    auto& party = reg.get<Party>(player);
    party.members.erase(
        std::remove_if(party.members.begin(), party.members.end(),
            [&](const PartyMember& m) { return m.entity_id == static_cast<uint32_t>(player); }),
        party.members.end());
    reg.remove<Party>(player);

    if (party.members.empty()) {
        spdlog::info("PartySystem: party {} disbanded (all members left)", party.party_id);
    } else if (party.leader_id == static_cast<uint32_t>(player)) {
        party.leader_id = party.members[0].entity_id;
        spdlog::info("PartySystem: party {} leadership transferred to {}",
                     party.party_id, party.members[0].entity_id);
    }
}

void PartySystem::KickMember(entt::registry& reg, entt::entity leader, uint32_t target_id) {
    if (!reg.valid(leader) || !reg.all_of<Party>(leader)) return;
    auto& party = reg.get<Party>(leader);
    if (party.leader_id != static_cast<uint32_t>(leader)) return;
    party.members.erase(
        std::remove_if(party.members.begin(), party.members.end(),
            [&](const PartyMember& m) { return m.entity_id == target_id; }),
        party.members.end());
    spdlog::info("PartySystem: {} kicked from party {}", target_id, party.party_id);
}

uint32_t PartySystem::DistributeXP(entt::registry& reg, entt::entity party_entity, uint32_t total_xp) {
    if (!reg.valid(party_entity)) return 0;
    auto& party = reg.get<Party>(party_entity);
    int online = 0;
    int total_level = 0;
    for (auto& m : party.members) {
        if (m.is_online) { online++; total_level += m.level; }
    }
    if (online == 0) return 0;
    float bonus = 1.0f + (online - 1) * 0.2f;
    uint32_t per_member = static_cast<uint32_t>(total_xp * bonus / online);
    for (auto& m : party.members) {
        if (m.is_online) {
            entt::entity e = static_cast<entt::entity>(m.entity_id);
            if (reg.valid(e) && reg.all_of<CharacterStats>(e)) {
                reg.get<CharacterStats>(e).exp += per_member;
            }
        }
    }
    party.total_xp_earned += total_xp;
    return per_member;
}

bool PartySystem::DistributeLoot(entt::registry& reg, entt::entity party_entity, uint32_t item_id) {
    if (!reg.valid(party_entity)) return false;
    auto& party = reg.get<Party>(party_entity);

    LootMode mode = static_cast<LootMode>(party.is_distribution_free ? 0 : 1);

    std::vector<uint32_t> online;
    for (auto& m : party.members)
        if (m.is_online) online.push_back(m.entity_id);
    if (online.empty()) return false;

    uint32_t target = 0;

    switch (mode) {
        case LootMode::FREE_FOR_ALL:
            return true;

        case LootMode::ROUND_ROBIN:
            target = online[round_robin_index_ % online.size()];
            round_robin_index_ = (round_robin_index_ + 1) % online.size();
            break;

        case LootMode::MASTER_LOOT:
            target = party.leader_id;
            break;

        case LootMode::NEED_GREED:
        default:
            target = online[rng_() % online.size()];
            break;
    }

    entt::entity e = static_cast<entt::entity>(target);
    if (reg.valid(e) && reg.all_of<Inventory>(e)) {
        reg.get<Inventory>(e).AddItem(item_id, 1);
        spdlog::info("PartySystem: loot {} distributed to entity {}", item_id, target);
        return true;
    }
    return false;
}

void PartySystem::Update(entt::registry& reg, float dt) {
    for (auto it = invite_timers_.begin(); it != invite_timers_.end(); ) {
        it->timer += dt;
        if (it->timer >= it->timeout) {
            it->expired = true;
            invites_.erase(it->invitee);
            it = invite_timers_.erase(it);
        } else {
            ++it;
        }
    }

    auto view = reg.view<Party>();
    for (auto entity : view) {
        auto& party = view.get<Party>(entity);
        if (party.leader_id == static_cast<uint32_t>(entity)) {
            leader_activity_timer_ = 0.0f;
        }
    }

    leader_activity_timer_ += dt;
    if (leader_activity_timer_ >= leader_timeout_) {
        auto pview = reg.view<Party>();
        for (auto entity : pview) {
            auto& party = pview.get<Party>(entity);
            if (party.members.size() <= 1) {
                reg.remove<Party>(entity);
                spdlog::info("PartySystem: party {} auto-disbanded (leader timeout)", party.party_id);
            } else if (party.leader_id == static_cast<uint32_t>(entity)) {
                party.leader_id = party.members[0].entity_id;
                spdlog::info("PartySystem: party {} leader transferred due to inactivity",
                             party.party_id);
            }
        }
        leader_activity_timer_ = 0.0f;
    }
}

void PartySystem::SetLootMode(entt::registry& reg, entt::entity leader, LootMode mode) {
    if (!reg.valid(leader) || !reg.all_of<Party>(leader)) return;
    auto& party = reg.get<Party>(leader);
    if (party.leader_id != static_cast<uint32_t>(leader)) return;
    party.is_distribution_free = (mode == LootMode::FREE_FOR_ALL);
    spdlog::info("PartySystem: loot mode set to {} for party {}",
                 static_cast<int>(mode), party.party_id);
}

LootMode PartySystem::GetLootMode(entt::registry& reg, entt::entity party_entity) const {
    if (!reg.valid(party_entity) || !reg.all_of<Party>(party_entity)) {
        return LootMode::FREE_FOR_ALL;
    }
    auto& party = reg.get<Party>(party_entity);
    return party.is_distribution_free ? LootMode::FREE_FOR_ALL : LootMode::ROUND_ROBIN;
}

bool PartySystem::TransferLeadership(entt::registry& reg, entt::entity leader, uint32_t target_id) {
    if (!reg.valid(leader) || !reg.all_of<Party>(leader)) return false;
    auto& party = reg.get<Party>(leader);
    if (party.leader_id != static_cast<uint32_t>(leader)) return false;

    auto it = std::find_if(party.members.begin(), party.members.end(),
        [&](const PartyMember& m) { return m.entity_id == target_id; });
    if (it == party.members.end()) return false;

    party.leader_id = target_id;
    spdlog::info("PartySystem: leadership transferred to {} in party {}",
                 target_id, party.party_id);
    return true;
}

void PartySystem::SetPartyProperty(entt::registry& reg, entt::entity leader,
                                   const std::string& key, int32_t value) {
    if (!reg.valid(leader) || !reg.all_of<Party>(leader)) return;
    auto& party = reg.get<Party>(leader);
    if (party.leader_id != static_cast<uint32_t>(leader)) return;
    (void)key;
    (void)value;
}
