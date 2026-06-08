#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <functional>
#include <unordered_map>
#include <unordered_set>
#include <flatbuffers/flatbuffers.h>

namespace luna::protocol {
struct FarmPlotInfo;
struct PetInfo;
struct FamilyInfo;
struct SiegeInfoResponse;
struct TournamentListResponse;
struct CashShopListResponse;
struct HousingInfoResponse;
}

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
        uint64_t married_date = 0;
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
        uint8_t evolution = 1;
        uint32_t exp = 0;
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
        uint32_t attacker_guild_id = 0;
        std::string attacker_guild_name;
        bool is_castle = false;
        uint8_t defense_bonus = 0;
        uint32_t tax_accumulated = 0;
    };
    struct TournamentState {
        uint32_t id = 0;
        std::string name;
        uint8_t state = 0;
        uint16_t registered = 0;
        uint16_t max_teams = 8;
        uint32_t prize_gold = 5000;
        uint64_t registration_end = 0;
        uint8_t current_round = 0;
        uint8_t min_team_size = 1;
        uint8_t max_team_size = 6;
        uint32_t winner_guild_id = 0;
        bool prize_claimed = false;
    };
    struct TournamentRegistration {
        uint32_t tournament_id = 0;
        uint32_t guild_id = 0;
        std::string guild_name;
        uint32_t team_leader_id = 0;
        uint16_t seed = 0;
        bool eliminated = false;
    };
    struct TournamentMatchState {
        uint32_t tournament_id = 0;
        uint8_t round = 0;
        uint8_t match_index = 0;
        uint32_t team1_guild_id = 0;
        uint32_t team2_guild_id = 0;
        uint32_t winner_guild_id = 0;
        bool completed = false;
    };
    struct HouseState {
        uint32_t house_id = 0;
        uint32_t owner_id = 0;
        std::string name;
        uint16_t map_id = 51;
        float pos_x = 0;
        float pos_y = 0;
        uint8_t house_type = 0;
        uint16_t furniture_count = 0;
        uint16_t max_furniture = 16;
    };
    struct FurnitureState {
        uint32_t furniture_id = 0;
        uint32_t house_id = 0;
        uint32_t item_id = 0;
        std::string name;
        float pos_x = 0;
        float pos_y = 0;
        float rot_y = 0;
    };
    struct HouseTemplate {
        uint8_t template_id = 0;
        std::string name;
        uint32_t price = 10000;
        uint16_t max_furniture = 16;
        uint16_t map_id = 51;
    };
    struct FurnitureCatalogEntry {
        uint32_t item_id = 0;
        std::string name;
        std::string category;
    };
    struct ShopItem {
        uint32_t item_id = 0;
        std::string name;
        std::string description;
        uint32_t price = 0;
        std::string category;
        uint8_t currency_type = 0;
        uint16_t stack_count = 1;
        uint16_t max_purchase = 99;
        bool on_sale = false;
        uint32_t sale_price = 0;
    };
    struct PlayerCashShopState {
        uint32_t luna_points = 500;
        uint16_t battle_pass_level = 1;
        uint32_t battle_pass_xp = 0;
        bool battle_pass_active = false;
    };

    void SendFamilyState(NetworkLayer* network, const MapPlayerContext& player, uint8_t result,
                         const std::string& message = {});
    flatbuffers::Offset<luna::protocol::FamilyInfo> MakeFamilyInfoOffset(
        flatbuffers::FlatBufferBuilder& fbb, const MapPlayerContext& player) const;
    static const char* RelationLabel(uint8_t relation);
    void SendPetState(NetworkLayer* network, const MapPlayerContext& player, uint8_t result,
                      const std::string& message = {});
    void SendFishingResult(NetworkLayer* network, const MapPlayerContext& player, uint8_t result,
                           uint32_t fish_id, const std::string& fish_name, uint8_t rarity,
                           const std::string& message = {});
    void SendSiegeInfo(NetworkLayer* network, const MapPlayerContext& player,
                       uint8_t result = 0, const std::string& message = {});
    flatbuffers::Offset<luna::protocol::SiegeInfoResponse> MakeSiegeInfoResponseOffset(
        flatbuffers::FlatBufferBuilder& fbb, const MapPlayerContext& player,
        uint8_t result, const std::string& message) const;
    TerritoryState* FindTerritory(uint32_t territory_id);
    const TerritoryState* FindTerritory(uint32_t territory_id) const;
    void SaveTerritoryToDb(const TerritoryState& territory);
    void TickSieges();
    void ResolveSiege(TerritoryState& territory);
    static uint32_t SecondsUntilSiege(uint64_t siege_time);
    void SendTournamentList(NetworkLayer* network, const MapPlayerContext& player,
                            uint8_t result = 0, const std::string& message = {});
    flatbuffers::Offset<luna::protocol::TournamentListResponse> MakeTournamentListResponseOffset(
        flatbuffers::FlatBufferBuilder& fbb, const MapPlayerContext& player,
        uint8_t result, const std::string& message) const;
    TournamentState* FindTournament(uint32_t tournament_id);
    void LoadTournamentsFromDb();
    void SaveTournamentToDb(const TournamentState& tournament);
    void SaveTournamentRegistrationToDb(const TournamentRegistration& reg);
    void DeleteTournamentRegistrationFromDb(uint32_t tournament_id, uint32_t guild_id);
    void SaveTournamentMatchToDb(const TournamentMatchState& match);
    void DeleteTournamentMatchesFromDb(uint32_t tournament_id);
    bool IsGuildRegistered(uint32_t tournament_id, uint32_t guild_id) const;
    void SyncTournamentRegisteredCount(TournamentState& tournament);
    void StartTournament(TournamentState& tournament);
    void GenerateTournamentBracket(TournamentState& tournament);
    void AdvanceTournamentRound(TournamentState& tournament);
    void CompleteTournament(TournamentState& tournament);
    void TickTournaments();
    static uint32_t SecondsUntilTournamentStart(uint64_t registration_end);
    void SendHousingInfo(NetworkLayer* network, const MapPlayerContext& player,
                         uint8_t result = 0, const std::string& message = {});
    flatbuffers::Offset<luna::protocol::HousingInfoResponse> MakeHousingInfoResponseOffset(
        flatbuffers::FlatBufferBuilder& fbb, const MapPlayerContext& player,
        uint8_t result, const std::string& message) const;
    HouseState* FindHouse(uint32_t house_id);
    const HouseState* FindHouse(uint32_t house_id) const;
    const HouseTemplate* FindHouseTemplate(uint8_t template_id) const;
    const FurnitureCatalogEntry* FindFurnitureCatalog(uint32_t item_id) const;
    bool PlayerOwnsHouse(uint32_t character_id) const;
    HouseState* FindOwnedHouse(uint32_t character_id);
    std::vector<const FurnitureState*> FurnitureForHouse(uint32_t house_id) const;
    void SaveHouseToDb(const HouseState& house);
    void SaveFurnitureToDb(const FurnitureState& furniture);
    void SyncHouseFurnitureCount(HouseState& house);
    static std::string HouseTypeName(uint8_t house_type);
    void SendCashShopList(NetworkLayer* network, const MapPlayerContext& player,
                          uint8_t result = 0, const std::string& message = {});
    flatbuffers::Offset<luna::protocol::CashShopListResponse> MakeCashShopListResponseOffset(
        flatbuffers::FlatBufferBuilder& fbb, const MapPlayerContext& player,
        uint8_t result, const std::string& message) const;
    PlayerCashShopState& EnsurePlayerCash(uint32_t character_id);
    void LoadPlayerCashFromDb(uint32_t character_id, PlayerCashShopState& out);
    void SavePlayerCashToDb(uint32_t character_id, const PlayerCashShopState& state);
    uint16_t GetPurchaseCount(uint32_t character_id, uint32_t item_id) const;
    void SetPurchaseCount(uint32_t character_id, uint32_t item_id, uint16_t count);
    static uint32_t EffectiveShopPrice(const ShopItem& item);
    const ShopItem* FindShopItem(uint32_t item_id) const;
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
    void TickPet(uint32_t character_id, PetState& pet, float dt);
    static uint32_t PetExpToNext(uint16_t level);
    static uint32_t PetFeedCost();
    static uint32_t PetEvolveCost(uint8_t evolution);
    static bool CanSummonPet(const PetState& pet);
    static void ApplyPetExpLevel(PetState& pet, uint32_t gained_exp);
    flatbuffers::Offset<luna::protocol::PetInfo> MakePetInfoOffset(
        flatbuffers::FlatBufferBuilder& fbb, const PetState& pet) const;
    void LoadFarmFromDb(uint32_t character_id, std::vector<FarmPlotState>& plots);
    void SaveFarmPlotToDb(uint32_t character_id, const FarmPlotState& plot);
    void SaveFamilyGroupToDb(const FamilyGroup& group);
    void TickFarmGrowth(uint32_t character_id, float dt);
    void TickFarmGrowthPlot(FarmPlotState& plot, float dt);
    static uint8_t CalcGrowthPct(const FarmPlotState& plot);
    static bool IsPlotReady(const FarmPlotState& plot);
    static bool IsPlotEmpty(const FarmPlotState& plot);
    const SeedDef* FindSeed(uint32_t seed_id) const;
    flatbuffers::Offset<luna::protocol::FarmPlotInfo> MakeFarmPlotOffset(
        flatbuffers::FlatBufferBuilder& fbb, const FarmPlotState& plot) const;
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
    std::vector<TournamentRegistration> tournament_regs_;
    std::vector<TournamentMatchState> tournament_matches_;
    std::vector<HouseState> houses_;
    std::vector<FurnitureState> furniture_;
    std::vector<HouseTemplate> house_templates_;
    std::vector<FurnitureCatalogEntry> furniture_catalog_;
    std::unordered_map<uint32_t, uint32_t> selected_house_;
    uint32_t next_house_id_ = 1;
    uint32_t next_furniture_id_ = 1;
    std::vector<ShopItem> shop_items_;
    std::unordered_map<uint32_t, PlayerCashShopState> player_cash_;
    std::unordered_map<uint32_t, std::unordered_map<uint32_t, uint16_t>> purchase_counts_;
    std::unordered_set<uint32_t> purchases_loaded_;
    void LoadPurchasesFromDb(uint32_t character_id);
    std::vector<SeedDef> seeds_;
    float farm_tick_timer_ = 0.0f;
    float pet_tick_timer_ = 0.0f;
    float siege_tick_timer_ = 0.0f;
    float tournament_tick_timer_ = 0.0f;
    static constexpr int kFarmPlotCount = 9;
};
