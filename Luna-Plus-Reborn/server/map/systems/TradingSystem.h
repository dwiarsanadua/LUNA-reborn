#pragma once
#include <cstdint>
#include <vector>
#include <unordered_map>
#include <entt/entt.hpp>
#include <ecs/components/Inventory.hpp>

struct ExchangeItem {
    uint32_t item_id = 0;
    uint32_t item_uid = 0;
    uint16_t count = 0;
    uint8_t enchant = 0;
    uint8_t slot = 0;
};

struct ExchangeRoom {
    uint32_t id;
    entt::entity player1;
    entt::entity player2;
    std::vector<ExchangeItem> items1;
    std::vector<ExchangeItem> items2;
    uint32_t gold1 = 0;
    uint32_t gold2 = 0;
    bool locked1 = false;
    bool locked2 = false;
    bool confirmed1 = false;
    bool confirmed2 = false;
    float timeout = 30.0f;
};

class TradingSystem {
public:
    TradingSystem();

    bool CanApplyExchange(entt::entity player, entt::entity target, entt::registry& registry);
    uint32_t CreateExchangeRoom(entt::entity player1, entt::entity player2);
    bool AddItem(entt::entity player, uint32_t room_id, const ExchangeItem& item);
    bool Lock(entt::entity player, uint32_t room_id);
    bool InputMoney(entt::entity player, uint32_t room_id, uint32_t gold);
    bool Confirm(entt::entity player, uint32_t room_id, entt::registry& registry);
    bool CancelExchange(entt::entity player, uint32_t room_id, entt::registry& registry);
    void UserLogOut(entt::entity player, entt::registry& registry);
    void Process(entt::registry& registry, float dt);

    ExchangeRoom* FindRoomByPlayer(entt::entity player);
    ExchangeRoom* FindRoom(uint32_t room_id);

private:
    std::unordered_map<uint32_t, ExchangeRoom> rooms_;
    uint32_t next_room_id_ = 1;

    void ReturnItems(ExchangeRoom& room, entt::registry& registry);
    void FinalizeExchange(ExchangeRoom& room, entt::registry& registry);
};
