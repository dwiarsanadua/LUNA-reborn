#pragma once
#include <string>
#include <vector>
#include <cstdint>
#include <ctime>

enum class FamilyRelation {
    None, Single, Engaged, Married, Divorced
};

struct FamilyMember {
    uint32_t character_id = 0;
    std::string name;
    FamilyRelation relation = FamilyRelation::None;
    uint32_t partner_id = 0;
    std::string partner_name;
    time_t married_date = 0;
    uint32_t family_id = 0;
    std::string family_name;
    bool online = false;
    
    // Family tree
    uint32_t parent1_id = 0;
    uint32_t parent2_id = 0;
    std::vector<uint32_t> children_ids;
};

class FamilySystem {
public:
    void Init();
    
    // Proposal / Marriage
    bool Propose(uint32_t from_id, const std::string& from_name, uint32_t to_id, const std::string& to_name);
    bool AcceptProposal(uint32_t character_id);
    bool RejectProposal(uint32_t character_id);
    bool Divorce(uint32_t character_id);
    
    // Family creation
    bool CreateFamily(uint32_t founder_id, const std::string& family_name);
    bool InviteToFamily(uint32_t inviter_id, uint32_t invitee_id);
    bool LeaveFamily(uint32_t character_id);
    bool DisbandFamily(uint32_t family_id);
    
    // Queries
    FamilyMember* GetMember(uint32_t character_id);
    std::vector<FamilyMember> GetFamilyMembers(uint32_t family_id) const;
    std::vector<FamilyMember> GetChildren(uint32_t parent_id) const;
    bool IsEngaged(uint32_t character_id) const;
    bool IsMarried(uint32_t character_id) const;
    bool IsInFamily(uint32_t character_id) const;
    
    int GetMemberCount() const { return (int)members_.size(); }
    int GetFamilyCount() const;
    
    std::string GetRelationName(FamilyRelation r) const;

private:
    std::vector<FamilyMember> members_;
    std::vector<uint32_t> pending_proposals_; // [proposer, proposee]
    uint32_t next_family_id_ = 1;
    
    FamilyMember* FindMember(uint32_t id);
};
