#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <functional>

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

    MemberState* FindMember(uint32_t character_id);
    const MemberState* FindMember(uint32_t character_id) const;

    Database* db_ = nullptr;
    int map_id_ = 0;
    std::vector<MemberState> members_;
    std::vector<FamilyGroup> families_;
    uint32_t next_family_id_ = 1;
    uint32_t proposal_from_ = 0;
    uint32_t proposal_to_ = 0;
    PetState pet_;
    std::vector<FishTableEntry> fish_table_;
    std::vector<TerritoryState> territories_;
    std::vector<TournamentState> tournaments_;
    std::vector<HouseState> houses_;
    std::vector<ShopItem> shop_items_;
};
