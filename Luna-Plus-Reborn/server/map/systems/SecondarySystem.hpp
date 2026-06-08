#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <functional>
#include <unordered_map>

class NetworkLayer;
class Database;

struct MapPlayerContext {
    uint32_t character_id = 0;
    std::string name;
    uint32_t guild_id = 0;
    std::string guild_name;
    uint32_t* gold = nullptr;
    std::function<void(uint32_t item_id, uint16_t count)> grant_loot;
};

class SecondarySystem {
public:
    void Init(Database* db, int map_id);
    void Update(float dt);
    void HandlePacket(NetworkLayer* network, const MapPlayerContext& player,
                      uint16_t type, const uint8_t* payload, size_t len);

private:
    struct MemberState {
        uint32_t character_id = 0;
        std::string name;
        uint8_t relation = 1;
        uint32_t partner_id = 0;
        std::string partner_name;
        uint32_t family_id = 0;
        std::string family_name;
    };
    struct FamilyGroup {
        uint32_t family_id = 0;
        std::string name;
        uint32_t master_id = 0;
    };
    struct PetState {
        uint32_t pet_id = 1;
        uint32_t template_id = 1;
        std::string name = "Fluffy";
        uint16_t level = 1;
        uint16_t hp = 100;
        uint16_t max_hp = 100;
        uint16_t satiation = 100;
        bool summoned = false;
        bool initialized = false;
    };
    struct FarmPlotState {
        uint8_t plot_id = 0;
        uint32_t seed_id = 0;
        std::string plant_name;
        uint8_t growth_stage = 0;
        uint8_t max_stages = 4;
        float growth_timer = 0;
        float growth_time = 60.0f;
        bool watered = false;
        bool harvested = false;
    };
    struct SeedDef {
        uint32_t id = 0;
        std::string name;
        uint32_t harvest_item = 0;
        uint8_t min_yield = 1;
        uint8_t max_yield = 3;
        float growth_time = 60.0f;
        uint8_t max_stages = 4;
    };
    struct FishTableEntry {
        uint32_t item_id = 0;
        std::string name;
        uint8_t rarity = 0;
        float weight = 1.0f;
    };
    struct TerritoryState {
        uint32_t id = 0;
        std::string name;
        uint32_t owner_guild_id = 0;
        std::string owner_guild_name;
        uint16_t tax_rate = 10;
        uint64_t siege_time = 0;
    };
    struct TournamentState {
        uint32_t id = 0;
        std::string name;
        uint8_t state = 0;
        uint16_t registered = 0;
        uint16_t max_teams = 8;
        uint32_t prize_gold = 5000;
    };
    struct HouseState {
        uint32_t house_id = 0;
        uint32_t owner_id = 0;
        uint16_t map_id = 51;
        float pos_x = 0;
        float pos_y = 0;
        uint8_t house_type = 0;
        uint16_t furniture_count = 0;
    };
    struct ShopItem {
        uint32_t item_id = 0;
        std::string name;
        uint32_t price = 0;
        std::string category;
    };

    void SendFamilyState(NetworkLayer* network, const MapPlayerContext& player, uint8_t result,
                         const std::string& message = {});
    void SendPetState(NetworkLayer* network, const MapPlayerContext& player, uint8_t result,
                      const std::string& message = {});
    void SendFishingResult(NetworkLayer* network, const MapPlayerContext& player, uint8_t result,
                           uint32_t fish_id, const std::string& fish_name, uint8_t rarity,
                           const std::string& message = {});
    void SendSiegeInfo(NetworkLayer* network, const MapPlayerContext& player);
    void SendTournamentList(NetworkLayer* network, const MapPlayerContext& player);
    void SendHousingInfo(NetworkLayer* network, const MapPlayerContext& player);
    void SendCashShopList(NetworkLayer* network, const MapPlayerContext& player);
    void SendFarmState(NetworkLayer* network, const MapPlayerContext& player, uint8_t result,
                       uint32_t harvest_item = 0, uint16_t harvest_count = 0,
                       const std::string& message = {});

    MemberState* FindMember(uint32_t character_id);
    const MemberState* FindMember(uint32_t character_id) const;
    MemberState& EnsureMember(uint32_t character_id, const std::string& name);
    PetState& EnsurePet(uint32_t character_id);
    std::vector<FarmPlotState>& EnsureFarm(uint32_t character_id);

    void LoadMembersFromDb();
    void LoadMemberFromDb(uint32_t character_id, MemberState& out);
    void SaveMemberToDb(const MemberState& member);
    void LoadPetFromDb(uint32_t character_id, PetState& out);
    void SavePetToDb(uint32_t character_id, const PetState& pet);
    void LoadFarmFromDb(uint32_t character_id, std::vector<FarmPlotState>& plots);
    void SaveFarmPlotToDb(uint32_t character_id, const FarmPlotState& plot);
    void SaveFamilyGroupToDb(const FamilyGroup& group);
    void TickFarmGrowth(uint32_t character_id, float dt);
    const SeedDef* FindSeed(uint32_t seed_id) const;
    static std::vector<SeedDef> DefaultSeeds();

    Database* db_ = nullptr;
    int map_id_ = 0;
    std::vector<MemberState> members_;
    std::vector<FamilyGroup> families_;
    uint32_t next_family_id_ = 1;
    uint32_t proposal_from_ = 0;
    uint32_t proposal_to_ = 0;
    std::unordered_map<uint32_t, PetState> pets_;
    std::unordered_map<uint32_t, std::vector<FarmPlotState>> farms_;
    std::vector<FishTableEntry> fish_table_;
    std::vector<TerritoryState> territories_;
    std::vector<TournamentState> tournaments_;
    std::vector<HouseState> houses_;
    std::vector<ShopItem> shop_items_;
    std::vector<SeedDef> seeds_;
    float farm_tick_timer_ = 0.0f;
    static constexpr int kFarmPlotCount = 9;
};
