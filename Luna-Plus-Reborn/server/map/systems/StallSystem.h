// AGENT Nexus — DO NOT MODIFY WITHOUT COORDINATION
#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <entt/entt.hpp>

struct StallItem {
    uint32_t item_id;
    uint16_t count;
    uint32_t price_per_unit;
    uint8_t slot_index;
};

struct Stall {
    uint32_t stall_id;
    uint32_t owner_id;
    std::string title;
    std::vector<StallItem> items;
    bool is_open = false;
    float pos_x, pos_y, pos_z;
    uint32_t map_id;
    float tax_rate = 0.05f;
};

class StallSystem {
public:
    StallSystem();

    bool OpenStall(entt::registry& reg, entt::entity owner,
                   const std::string& title, float x, float y, float z);
    bool CloseStall(entt::registry& reg, entt::entity owner);
    bool AddItem(entt::registry& reg, entt::entity owner,
                 uint32_t item_id, uint16_t count, uint32_t price);
    bool RemoveItem(entt::registry& reg, entt::entity owner, uint8_t slot_index);

    std::vector<StallItem> BrowseStall(entt::registry& reg, uint32_t stall_id);
    bool BuyItem(entt::registry& reg, entt::entity buyer,
                 uint32_t stall_id, uint8_t slot_index, uint16_t count);

    Stall* GetStallByOwner(entt::entity owner);
    Stall* GetStallById(uint32_t stall_id);

private:
    uint32_t next_stall_id_ = 1;
    std::unordered_map<uint32_t, Stall> stalls_;
    std::unordered_map<uint32_t, uint32_t> owner_to_stall_;
};
