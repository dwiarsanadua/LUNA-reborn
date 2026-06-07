#include "PartySystem.hpp"
#include "../components/Party.hpp"
#include "../components/CharacterStats.hpp"
#include "../components/Inventory.hpp"
#include "../components/Tag.hpp"
#include <algorithm>
#include <random>

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
    return true;
}

bool PartySystem::InviteToParty(entt::registry& reg, entt::entity inviter, entt::entity invitee) {
    if (!reg.valid(inviter) || !reg.valid(invitee)) return false;
    if (!reg.all_of<Party>(inviter)) return false;
    auto& party = reg.get<Party>(inviter);
    if (party.members.size() >= 8) return false;
    invites_[invitee] = inviter;
    return true;
}

bool PartySystem::AcceptInvite(entt::registry& reg, entt::entity player) {
    auto it = invites_.find(player);
    if (it == invites_.end() || !reg.valid(it->second)) return false;
    auto& party = reg.get<Party>(it->second);
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
    return true;
}

void PartySystem::LeaveParty(entt::registry& reg, entt::entity player) {
    if (!reg.valid(player) || !reg.all_of<Party>(player)) return;
    auto& party = reg.get<Party>(player);
    party.members.erase(
        std::remove_if(party.members.begin(), party.members.end(),
            [&](const PartyMember& m) { return m.entity_id == static_cast<uint32_t>(player); }),
        party.members.end());
    reg.remove<Party>(player);
}

void PartySystem::KickMember(entt::registry& reg, entt::entity leader, uint32_t target_id) {
    if (!reg.valid(leader) || !reg.all_of<Party>(leader)) return;
    auto& party = reg.get<Party>(leader);
    if (party.leader_id != static_cast<uint32_t>(leader)) return;
    party.members.erase(
        std::remove_if(party.members.begin(), party.members.end(),
            [&](const PartyMember& m) { return m.entity_id == target_id; }),
        party.members.end());
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
    return per_member;
}

bool PartySystem::DistributeLoot(entt::registry& reg, entt::entity party_entity, uint32_t item_id) {
    if (!reg.valid(party_entity)) return false;
    auto& party = reg.get<Party>(party_entity);
    if (party.is_distribution_free) return true;
    // Round robin: give to a random online member
    std::vector<uint32_t> online;
    for (auto& m : party.members)
        if (m.is_online) online.push_back(m.entity_id);
    if (online.empty()) return false;
    uint32_t target = online[rng_() % online.size()];
    entt::entity e = static_cast<entt::entity>(target);
    if (reg.valid(e) && reg.all_of<Inventory>(e)) {
        reg.get<Inventory>(e).AddItem(item_id, 1);
        return true;
    }
    return false;
}
