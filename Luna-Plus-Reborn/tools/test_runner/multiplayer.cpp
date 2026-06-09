#include "test_harness.hpp"
#include <string>
#include <vector>
#include <unordered_map>
#include <cstdint>
#include <algorithm>

struct PartyMember {
    uint32_t entity_id;
    std::string name;
    int level;
    int hp, max_hp;
    bool is_leader;
};

class Party {
public:
    uint32_t Create(uint32_t leader_id, const std::string& name) {
        id_ = next_id_++;
        leader_ = leader_id;
        name_ = name;
        members_.push_back({leader_id, name, 1, 100, 100, true});
        return id_;
    }

    bool Invite(uint32_t inviter, uint32_t target) {
        if (inviter != leader_) return false;
        if (members_.size() >= 8) return false;
        for (auto& m : members_) if (m.entity_id == target) return false;
        pending_.push_back(target);
        return true;
    }

    bool Accept(uint32_t entity_id) {
        auto it = std::find(pending_.begin(), pending_.end(), entity_id);
        if (it == pending_.end()) return false;
        pending_.erase(it);
        members_.push_back({entity_id, "Member" + std::to_string(entity_id), 1, 100, 100, false});
        return true;
    }

    bool Leave(uint32_t entity_id) {
        auto it = std::find_if(members_.begin(), members_.end(),
            [entity_id](const PartyMember& m) { return m.entity_id == entity_id; });
        if (it == members_.end()) return false;
        members_.erase(it);
        if (members_.empty()) { id_ = 0; leader_ = 0; return true; }
        if (entity_id == leader_) {
            leader_ = members_[0].entity_id;
            members_[0].is_leader = true;
        }
        return true;
    }

    uint32_t DistributeXP(uint32_t total_xp) {
        if (members_.empty()) return 0;
        uint32_t share = total_xp / static_cast<uint32_t>(members_.size());
        return share;
    }

    uint32_t GetId() const { return id_; }
    uint32_t GetLeader() const { return leader_; }
    size_t GetMemberCount() const { return members_.size(); }
    bool IsMember(uint32_t e) const {
        for (auto& m : members_) if (m.entity_id == e) return true;
        return false;
    }

private:
    uint32_t id_ = 0;
    uint32_t leader_ = 0;
    std::string name_;
    std::vector<PartyMember> members_;
    std::vector<uint32_t> pending_;
    static uint32_t next_id_;
};

uint32_t Party::next_id_ = 1;

struct GuildMember {
    uint32_t entity_id;
    std::string name;
    int level;
    int rank;
};

class Guild {
public:
    uint32_t Create(uint32_t founder, const std::string& name) {
        id_ = next_id_++;
        leader_ = founder;
        name_ = name;
        members_.push_back({founder, name, 1, 1});
        gp_ = 0;
        level_ = 1;
        return id_;
    }

    bool Invite(uint32_t inviter, uint32_t target) {
        if (inviter != leader_) return false;
        if (members_.size() >= 50) return false;
        pending_.push_back(target);
        return true;
    }

    bool Accept(uint32_t entity_id) {
        auto it = std::find(pending_.begin(), pending_.end(), entity_id);
        if (it == pending_.end()) return false;
        pending_.erase(it);
        members_.push_back({entity_id, "GMember" + std::to_string(entity_id), 1, 2});
        return true;
    }

    bool Disband(uint32_t requester) {
        if (requester != leader_) return false;
        members_.clear();
        pending_.clear();
        id_ = 0;
        leader_ = 0;
        return true;
    }

    void DepositGP(uint32_t amount) { gp_ += amount; }
    uint32_t GetGP() const { return gp_; }
    size_t GetMemberCount() const { return members_.size(); }

private:
    uint32_t id_ = 0;
    uint32_t leader_ = 0;
    std::string name_;
    uint32_t gp_ = 0;
    int level_ = 1;
    std::vector<GuildMember> members_;
    std::vector<uint32_t> pending_;
    static uint32_t next_id_;
};

uint32_t Guild::next_id_ = 1;

void RunMultiplayerTests() {
    TEST_SUITE("MULTIPLAYER: PARTY + GUILD");

    TEST_STEP("Party — create, invite, accept, member count");
    {
        Party party;
        uint32_t id = party.Create(1, "TestParty");
        TEST("Party created with ID", id > 0);

        bool inv = party.Invite(1, 2);
        TEST("Invite sent", inv);

        bool acc = party.Accept(2);
        TEST("Invite accepted", acc);
        TEST("Party has 2 members", party.GetMemberCount() == 2);

        uint32_t xp = party.DistributeXP(1000);
        TEST("XP distributed", xp > 0);
        spdlog::info("  Party ID={}, members={}, XP share={}", id, party.GetMemberCount(), xp);
    }

    TEST_STEP("Party — leave and new leader assignment");
    {
        Party party;
        party.Create(1, "Party2");
        party.Invite(1, 2);
        party.Accept(2);
        party.Invite(1, 3);
        party.Accept(3);

        bool left = party.Leave(1);
        TEST("Leader left", left);
        TEST("Party still has 2 members", party.GetMemberCount() == 2);
        TEST("New leader != 1", party.GetLeader() != 1);
        spdlog::info("  New leader ID={}, members={}", party.GetLeader(), party.GetMemberCount());
    }

    TEST_STEP("Party — max 8 members");
    {
        Party party;
        party.Create(1, "MaxParty");
        for (uint32_t i = 2; i <= 8; i++) {
            party.Invite(1, i);
            party.Accept(i);
        }
        TEST("Party has 8 members", party.GetMemberCount() == 8);
        bool extra = party.Invite(1, 9);
        TEST("9th invite rejected", !extra);
    }

    TEST_STEP("Guild — create, invite, accept, member count");
    {
        Guild guild;
        uint32_t id = guild.Create(1, "TestGuild");
        TEST("Guild created", id > 0);
        TEST("Guild has 1 member", guild.GetMemberCount() == 1);

        bool inv = guild.Invite(1, 2);
        TEST("Guild invite sent", inv);

        bool acc = guild.Accept(2);
        TEST("Guild invite accepted", acc);
        TEST("Guild has 2 members", guild.GetMemberCount() == 2);

        guild.DepositGP(5000);
        TEST("GP deposited", guild.GetGP() == 5000);
        spdlog::info("  Guild ID={}, members={}, GP={}", id, guild.GetMemberCount(), guild.GetGP());
    }

    TEST_STEP("Guild — disband");
    {
        Guild guild;
        guild.Create(1, "TempGuild");
        guild.Invite(1, 2);
        guild.Accept(2);
        bool disbanded = guild.Disband(1);
        TEST("Guild disbanded by leader", disbanded);
        TEST("Guild has 0 members after disband", guild.GetMemberCount() == 0);
    }

    TEST_STEP("Guild — non-leader cannot disband");
    {
        Guild guild;
        guild.Create(1, "SecureGuild");
        guild.Invite(1, 2);
        guild.Accept(2);
        bool unauthorized = guild.Disband(2);
        TEST("Non-leader cannot disband", !unauthorized);
        TEST("Guild intact after failed disband", guild.GetMemberCount() == 2);
    }
}
