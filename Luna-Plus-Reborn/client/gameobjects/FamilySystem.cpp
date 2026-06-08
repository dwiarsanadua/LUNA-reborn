#include "FamilySystem.hpp"
#include <algorithm>
#include <spdlog/spdlog.h>

void FamilySystem::Init() {
    members_.clear();
    pending_proposals_.clear();
    next_family_id_ = 1;
    spdlog::info("FamilySystem: initialized");
}

bool FamilySystem::Propose(uint32_t from_id, const std::string& from_name,
                            uint32_t to_id, const std::string& to_name) {
    if (IsMarried(from_id) || IsMarried(to_id)) return false;
    if (IsEngaged(from_id) || IsEngaged(to_id)) return false;
    
    auto* from = FindMember(from_id);
    if (!from) { FamilyMember fm; fm.character_id = from_id; fm.name = from_name; fm.relation = FamilyRelation::Single; members_.push_back(fm); from = &members_.back(); }
    auto* to = FindMember(to_id);
    if (!to) { FamilyMember fm; fm.character_id = to_id; fm.name = to_name; fm.relation = FamilyRelation::Single; members_.push_back(fm); to = &members_.back(); }
    
    pending_proposals_.push_back(from_id);
    pending_proposals_.push_back(to_id);
    from->relation = FamilyRelation::Engaged;
    to->relation = FamilyRelation::Engaged;
    from->partner_id = to_id;
    to->partner_id = from_id;
    from->partner_name = to_name;
    to->partner_name = from_name;
    spdlog::info("Family: {} proposed to {}", from_name, to_name);
    return true;
}

bool FamilySystem::AcceptProposal(uint32_t character_id) {
    auto* member = FindMember(character_id);
    if (!member || member->relation != FamilyRelation::Engaged) return false;
    if (member->partner_id == 0) return false;
    
    auto* partner = FindMember(member->partner_id);
    if (!partner) return false;
    
    member->relation = FamilyRelation::Married;
    partner->relation = FamilyRelation::Married;
    member->married_date = time(nullptr);
    partner->married_date = time(nullptr);
    
    // Create family
    uint32_t fid = next_family_id_++;
    member->family_id = fid;
    partner->family_id = fid;
    member->family_name = member->name + " & " + partner->name;
    partner->family_name = member->family_name;
    
    spdlog::info("Family: {} and {} are now married!", member->name, partner->name);
    return true;
}

bool FamilySystem::RejectProposal(uint32_t character_id) {
    auto* member = FindMember(character_id);
    if (!member || member->relation != FamilyRelation::Engaged) return false;
    
    auto* partner = FindMember(member->partner_id);
    if (partner) { partner->relation = FamilyRelation::Single; partner->partner_id = 0; }
    member->relation = FamilyRelation::Single;
    member->partner_id = 0;
    return true;
}

bool FamilySystem::Divorce(uint32_t character_id) {
    auto* member = FindMember(character_id);
    if (!member || member->relation != FamilyRelation::Married) return false;
    auto* partner = FindMember(member->partner_id);
    if (partner) { partner->relation = FamilyRelation::Divorced; partner->partner_id = 0; }
    member->relation = FamilyRelation::Divorced;
    member->partner_id = 0;
    spdlog::info("Family: {} divorced", member->name);
    return true;
}

bool FamilySystem::CreateFamily(uint32_t founder_id, const std::string& family_name) {
    auto* member = FindMember(founder_id);
    if (!member) { FamilyMember fm; fm.character_id = founder_id; fm.relation = FamilyRelation::Single; members_.push_back(fm); member = &members_.back(); }
    if (member->family_id > 0) return false;
    
    member->family_id = next_family_id_++;
    member->family_name = family_name;
    spdlog::info("Family: {} created family {}", member->name, family_name);
    return true;
}

bool FamilySystem::InviteToFamily(uint32_t inviter_id, uint32_t invitee_id) {
    auto* inviter = FindMember(inviter_id);
    auto* invitee = FindMember(invitee_id);
    if (!inviter || !invitee) return false;
    if (inviter->family_id == 0) return false;
    if (invitee->family_id > 0) return false;
    
    invitee->family_id = inviter->family_id;
    invitee->family_name = inviter->family_name;
    return true;
}

bool FamilySystem::LeaveFamily(uint32_t character_id) {
    auto* member = FindMember(character_id);
    if (!member || member->family_id == 0) return false;
    member->family_id = 0;
    member->family_name.clear();
    return true;
}

bool FamilySystem::DisbandFamily(uint32_t family_id) {
    for (auto& m : members_) {
        if (m.family_id == family_id) {
            m.family_id = 0;
            m.family_name.clear();
        }
    }
    return true;
}

FamilyMember* FamilySystem::FindMember(uint32_t id) {
    for (auto& m : members_) if (m.character_id == id) return &m;
    return nullptr;
}

FamilyMember* FamilySystem::GetMember(uint32_t character_id) {
    return FindMember(character_id);
}

std::vector<FamilyMember> FamilySystem::GetFamilyMembers(uint32_t family_id) const {
    std::vector<FamilyMember> result;
    for (auto& m : members_) if (m.family_id == family_id) result.push_back(m);
    return result;
}

std::vector<FamilyMember> FamilySystem::GetChildren(uint32_t parent_id) const {
    std::vector<FamilyMember> result;
    for (auto& m : members_) {
        if (m.parent1_id == parent_id || m.parent2_id == parent_id)
            result.push_back(m);
    }
    return result;
}

bool FamilySystem::IsEngaged(uint32_t character_id) const {
    for (auto& m : members_) if (m.character_id == character_id) return m.relation == FamilyRelation::Engaged;
    return false;
}

bool FamilySystem::IsMarried(uint32_t character_id) const {
    for (auto& m : members_) if (m.character_id == character_id) return m.relation == FamilyRelation::Married;
    return false;
}

bool FamilySystem::IsInFamily(uint32_t character_id) const {
    for (auto& m : members_) if (m.character_id == character_id) return m.family_id > 0;
    return false;
}

int FamilySystem::GetFamilyCount() const {
    int count = 0;
    uint32_t last = 0;
    for (auto& m : members_) {
        if (m.family_id > 0 && m.family_id != last) { count++; last = m.family_id; }
    }
    return count;
}

std::string FamilySystem::GetRelationName(FamilyRelation r) const {
    switch (r) {
    case FamilyRelation::Single: return "Single";
    case FamilyRelation::Engaged: return "Engaged";
    case FamilyRelation::Married: return "Married";
    case FamilyRelation::Divorced: return "Divorced";
    default: return "Unknown";
    }
}

std::string FamilySystem::GetRelationName(uint8_t relation) {
    switch (relation) {
    case 2: return "Engaged";
    case 3: return "Married";
    case 4: return "Divorced";
    default: return "Single";
    }
}

void FamilySystem::SyncFromNetwork(uint32_t self_id, uint32_t family_id,
                                   const std::string& family_name, uint32_t master_id,
                                   const std::vector<NetworkFamilyMemberView>& net_members) {
    (void)self_id;
    members_.clear();
    pending_proposals_.clear();
    uint32_t max_fid = next_family_id_;
    for (const auto& nm : net_members) {
        FamilyMember m;
        m.character_id = nm.character_id;
        m.name = nm.name;
        switch (nm.relation) {
        case 2: m.relation = FamilyRelation::Engaged; break;
        case 3: m.relation = FamilyRelation::Married; break;
        case 4: m.relation = FamilyRelation::Divorced; break;
        default: m.relation = FamilyRelation::Single; break;
        }
        m.partner_id = nm.partner_id;
        m.partner_name = nm.partner_name;
        m.married_date = static_cast<time_t>(nm.married_date);
        m.family_id = family_id;
        m.family_name = family_name;
        if (nm.is_master) (void)master_id;
        if (family_id >= max_fid) max_fid = family_id + 1;
        members_.push_back(std::move(m));
    }
    if (family_id) next_family_id_ = std::max(next_family_id_, max_fid);
}
