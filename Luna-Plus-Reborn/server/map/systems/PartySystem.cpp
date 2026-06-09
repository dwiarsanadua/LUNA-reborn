#include "PartySystem.h"
#include <ecs/components/Inventory.hpp>
#include <ecs/components/CharacterStats.hpp>
#include <spdlog/spdlog.h>
#include <algorithm>
#include <random>

PartySystem::PartySystem() {}
static std::mt19937 s_rng(std::random_device{}());

void PartySystem::SetPacketSender(PacketSender sender) {
    send_packet_ = sender;
}

uint32_t PartySystem::GetCharacterId(entt::entity entity) const {
    // In a full implementation, fetch from CharacterStats or similar component
    return static_cast<uint32_t>(entity);
}

bool PartySystem::CreateParty(entt::registry& reg, entt::entity leader, uint32_t leader_char_id, const std::string& leader_name) {
    if (!reg.valid(leader)) return false;

    // Check not already in a party
    if (member_to_party_.count(leader_char_id)) {
        spdlog::warn("PartySystem: player {} already in a party", leader_char_id);
        return false;
    }

    uint32_t party_id = next_party_id_++;
    PartyData data;
    data.party_id = party_id;
    data.leader_entity = leader;
    data.leader_id = leader_char_id;
    data.loot_mode = LootMode::FREE_FOR_ALL;
    data.share_exp = true;
    data.share_loot = false;
    data.formed_at = 0.0f;

    PartyMemberData leader_member;
    leader_member.entity = leader;
    leader_member.character_id = leader_char_id;
    leader_member.name = leader_name;
    leader_member.level = 1;
    leader_member.is_leader = true;
    leader_member.online = true;
    data.members.push_back(leader_member);

    parties_[party_id] = data;
    member_to_party_[leader_char_id] = party_id;

    spdlog::info("PartySystem: party {} created by {}", party_id, leader_name);
    return true;
}

bool PartySystem::InviteToParty(entt::registry& reg, entt::entity inviter, entt::entity invitee, uint32_t invitee_char_id) {
    if (!reg.valid(inviter) || !reg.valid(invitee)) return false;

    // Check inviter is in a party
    auto inviter_id = GetCharacterId(inviter);
    auto it = member_to_party_.find(inviter_id);
    if (it == member_to_party_.end()) return false;

    // Check invitee not already in party
    if (member_to_party_.count(invitee_char_id)) return false;

    // Check not already invited
    for (auto& pi : pending_invites_) {
        if (pi.invitee_id == invitee_char_id && pi.party_id == it->second) {
            return false; // Already invited
        }
    }

    PendingInvite invite;
    invite.inviter_id = inviter_id;
    invite.invitee_id = invitee_char_id;
    invite.party_id = it->second;
    invite.timer = 0.0f;
    pending_invites_.push_back(invite);

    spdlog::info("PartySystem: {} invited to party {}", invitee_char_id, it->second);
    return true;
}

bool PartySystem::AcceptInvite(entt::registry& reg, entt::entity player) {
    if (!reg.valid(player)) return false;

    auto player_id = GetCharacterId(player);
    for (auto it = pending_invites_.begin(); it != pending_invites_.end(); ++it) {
        if (it->invitee_id == player_id) {
            uint32_t party_id = it->party_id;
            auto p_it = parties_.find(party_id);
            if (p_it == parties_.end()) {
                pending_invites_.erase(it);
                return false;
            }

            auto& party = p_it->second;
            PartyMemberData member;
            member.entity = player;
            member.character_id = player_id;
            member.is_leader = false;
            member.online = true;
            party.members.push_back(member);
            member_to_party_[player_id] = party_id;

            pending_invites_.erase(it);
            BroadcastPartyUpdate(party_id);
            spdlog::info("PartySystem: {} joined party {}", player_id, party_id);
            return true;
        }
    }
    return false;
}

void PartySystem::DeclineInvite(entt::registry& reg, entt::entity player) {
    if (!reg.valid(player)) return;

    auto player_id = GetCharacterId(player);
    for (auto it = pending_invites_.begin(); it != pending_invites_.end(); ++it) {
        if (it->invitee_id == player_id) {
            pending_invites_.erase(it);
            spdlog::debug("PartySystem: {} declined invite", player_id);
            return;
        }
    }
}

void PartySystem::LeaveParty(entt::registry& reg, entt::entity player) {
    if (!reg.valid(player)) return;

    auto player_id = GetCharacterId(player);
    auto it = member_to_party_.find(player_id);
    if (it == member_to_party_.end()) return;

    uint32_t party_id = it->second;
    RemoveMember(party_id, player_id);
    spdlog::info("PartySystem: {} left party {}", player_id, party_id);
}

bool PartySystem::KickMember(entt::registry& reg, entt::entity leader, uint32_t target_id) {
    if (!reg.valid(leader)) return false;

    auto leader_id = GetCharacterId(leader);
    auto it = member_to_party_.find(leader_id);
    if (it == member_to_party_.end()) return false;

    uint32_t party_id = it->second;
    auto p_it = parties_.find(party_id);
    if (p_it == parties_.end()) return false;

    auto& party = p_it->second;
    if (party.leader_id != leader_id) return false; // Only leader can kick
    if (leader_id == target_id) return false; // Cannot kick self

    RemoveMember(party_id, target_id);
    spdlog::info("PartySystem: {} kicked from party {} by leader {}", target_id, party_id, leader_id);
    return true;
}

uint32_t PartySystem::GetPartyId(entt::entity player) const {
    auto player_id = static_cast<uint32_t>(player);
    auto it = member_to_party_.find(player_id);
    if (it == member_to_party_.end()) return 0;
    return it->second;
}

PartyData* PartySystem::GetParty(uint32_t party_id) {
    auto it = parties_.find(party_id);
    if (it == parties_.end()) return nullptr;
    return &it->second;
}

const PartyData* PartySystem::GetPartyByMember(entt::entity player) const {
    auto player_id = static_cast<uint32_t>(player);
    auto it = member_to_party_.find(player_id);
    if (it == member_to_party_.end()) return nullptr;
    auto p_it = parties_.find(it->second);
    if (p_it == parties_.end()) return nullptr;
    return &p_it->second;
}

uint32_t PartySystem::DistributeXP(entt::registry& reg, uint32_t party_id, uint32_t total_xp, entt::entity killer) {
    auto it = parties_.find(party_id);
    if (it == parties_.end()) return total_xp; // No party, return full XP to killer

    auto& party = it->second;
    if (!party.share_exp) return total_xp;

    // Count online members in same map
    std::vector<entt::entity> online_members;
    for (auto& m : party.members) {
        if (m.online) online_members.push_back(m.entity);
    }

    if (online_members.empty()) return total_xp;

    // Party bonus: +20% total XP
    uint32_t total_with_bonus = static_cast<uint32_t>(total_xp * 1.2f);
    uint32_t per_member = total_with_bonus / static_cast<uint32_t>(online_members.size());

    spdlog::debug("PartySystem: distributing {} XP across {} members ({} each)",
                  total_with_bonus, online_members.size(), per_member);
    return per_member;
}

bool PartySystem::DistributeLoot(entt::registry& reg, uint32_t party_id, uint32_t item_id) {
    auto it = parties_.find(party_id);
    if (it == parties_.end()) return false;

    auto& party = it->second;

    switch (party.loot_mode) {
        case LootMode::FREE_FOR_ALL:
            return true; // Anyone can pick up

        case LootMode::ROUND_ROBIN: {
            if (party.members.empty()) return false;
            round_robin_idx_ = (round_robin_idx_ + 1) % party.members.size();
            auto& member = party.members[round_robin_idx_];
            spdlog::debug("PartySystem: round-robin loot to {}", member.character_id);
            return true;
        }

        case LootMode::MASTER_LOOT:
            spdlog::debug("PartySystem: master loot, waiting for leader {}", party.leader_id);
            return false; // Leader must assign

        case LootMode::NEED_GREED:
            spdlog::debug("PartySystem: need/greed roll for item {}", item_id);
            return true; // Roll handled by client

        default:
            return true;
    }
}

void PartySystem::DistributeGold(entt::registry& reg, uint32_t party_id, uint32_t total_gold, entt::entity killer) {
    auto it = parties_.find(party_id);
    if (it == parties_.end()) {
        if (reg.valid(killer)) {
            auto* inv = reg.try_get<Inventory>(killer);
            if (inv) inv->gold += total_gold;
        }
        return;
    }

    auto& party = it->second;
    std::vector<entt::entity> online_members;
    for (auto& m : party.members) {
        if (m.online) online_members.push_back(m.entity);
    }

    if (online_members.empty()) {
        if (reg.valid(killer)) {
            auto* inv = reg.try_get<Inventory>(killer);
            if (inv) inv->gold += total_gold;
        }
        return;
    }

    uint32_t share = total_gold / static_cast<uint32_t>(online_members.size());
    for (auto& entity : online_members) {
        if (reg.valid(entity)) {
            auto* inv = reg.try_get<Inventory>(entity);
            if (inv) inv->gold += share;
        }
    }
    spdlog::debug("PartySystem: distributed {} gold across {} members ({} each)",
                  total_gold, online_members.size(), share);
}

void PartySystem::SetLootMode(entt::registry& reg, entt::entity leader, LootMode mode) {
    if (!reg.valid(leader)) return;
    auto leader_id = GetCharacterId(leader);

    auto it = member_to_party_.find(leader_id);
    if (it == member_to_party_.end()) return;

    auto p_it = parties_.find(it->second);
    if (p_it == parties_.end()) return;

    if (p_it->second.leader_id != leader_id) return; // Only leader

    p_it->second.loot_mode = mode;
    BroadcastPartyUpdate(p_it->first);
    spdlog::info("PartySystem: party {} loot mode set to {}", p_it->first, static_cast<int>(mode));
}

bool PartySystem::TransferLeadership(entt::registry& reg, entt::entity new_leader) {
    if (!reg.valid(new_leader)) return false;

    auto new_leader_id = GetCharacterId(new_leader);
    auto it = member_to_party_.find(new_leader_id);
    if (it == member_to_party_.end()) return false;

    auto p_it = parties_.find(it->second);
    if (p_it == parties_.end()) return false;

    auto& party = p_it->second;
    for (auto& m : party.members) {
        if (m.character_id == new_leader_id) {
            // Demote current leader
            for (auto& lm : party.members) {
                if (lm.character_id == party.leader_id) {
                    lm.is_leader = false;
                    break;
                }
            }
            m.is_leader = true;
            party.leader_entity = new_leader;
            party.leader_id = new_leader_id;
            BroadcastPartyUpdate(p_it->first);
            spdlog::info("PartySystem: leadership transferred to {} in party {}", new_leader_id, p_it->first);
            return true;
        }
    }
    return false;
}

void PartySystem::BroadcastToParty(uint32_t party_id, uint16_t packet_type, const uint8_t* data, size_t len) {
    if (!send_packet_) return;
    auto it = parties_.find(party_id);
    if (it == parties_.end()) return;

    for (auto& m : it->second.members) {
        // In production, look up network session for each player
        send_packet_(m.character_id, data, len);
    }
}

void PartySystem::UpdatePartyMember(entt::registry& reg, entt::entity player) {
    if (!reg.valid(player)) return;
    auto player_id = GetCharacterId(player);

    auto it = member_to_party_.find(player_id);
    if (it == member_to_party_.end()) return;

    auto p_it = parties_.find(it->second);
    if (p_it == parties_.end()) return;

    for (auto& m : p_it->second.members) {
        if (m.character_id == player_id) {
            // Update member stats from ECS
            // In full implementation, read CharacterStats component
            m.entity = player;
            m.online = true;
            break;
        }
    }
    BroadcastPartyUpdate(p_it->first);
}

void PartySystem::RemoveMember(uint32_t party_id, uint32_t character_id) {
    auto it = parties_.find(party_id);
    if (it == parties_.end()) return;

    auto& party = it->second;
    party.members.erase(
        std::remove_if(party.members.begin(), party.members.end(),
            [character_id](const PartyMemberData& m) { return m.character_id == character_id; }),
        party.members.end());

    member_to_party_.erase(character_id);

    if (party.members.empty()) {
        // Party dissolved
        parties_.erase(party_id);
        spdlog::info("PartySystem: party {} dissolved (no members)", party_id);
        return;
    }

    // Transfer leadership if leader left
    if (party.leader_id == character_id) {
        auto& new_leader = party.members.front();
        new_leader.is_leader = true;
        party.leader_entity = new_leader.entity;
        party.leader_id = new_leader.character_id;
        spdlog::info("PartySystem: leadership transferred to {} in party {}", new_leader.character_id, party_id);
    }

    BroadcastPartyUpdate(party_id);
}

void PartySystem::BroadcastPartyUpdate(uint32_t party_id) {
    auto it = parties_.find(party_id);
    if (it == parties_.end()) return;

    auto& party = it->second;
    spdlog::debug("PartySystem: broadcasting update for party {} ({} members)", party_id, party.members.size());
}

void PartySystem::Update(entt::registry& reg, float dt) {
    // Process pending invites
    for (auto it = pending_invites_.begin(); it != pending_invites_.end(); ) {
        it->timer += dt;
        if (it->timer >= it->timeout) {
            spdlog::debug("PartySystem: invite to {} expired", it->invitee_id);
            it = pending_invites_.erase(it);
        } else {
            ++it;
        }
    }

    // Check for dissolved parties (leader offline for too long)
    for (auto it = parties_.begin(); it != parties_.end(); ) {
        auto& party = it->second;
        bool leader_online = false;
        for (auto& m : party.members) {
            if (m.is_leader && m.online) {
                leader_online = true;
                break;
            }
        }

        if (!leader_online && party.members.size() > 1) {
            // Auto-transfer leadership to next online member
            for (auto& m : party.members) {
                if (m.online) {
                    m.is_leader = true;
                    party.leader_entity = m.entity;
                    party.leader_id = m.character_id;
                    spdlog::info("PartySystem: auto-transferred leadership to {} in party {}", m.character_id, party.party_id);
                    BroadcastPartyUpdate(party.party_id);
                    break;
                }
            }
        }

        if (party.members.size() <= 1) {
            spdlog::info("PartySystem: party {} dissolved", it->first);
            for (auto& m : party.members) {
                member_to_party_.erase(m.character_id);
            }
            it = parties_.erase(it);
        } else {
            ++it;
        }
    }
}
