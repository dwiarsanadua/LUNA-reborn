#include "TradingSystem.h"
#include <ecs/components/CharacterStats.hpp>
#include <ecs/components/Tag.hpp>
#include <ecs/components/Transform.hpp>
#include <spdlog/spdlog.h>
#include <algorithm>

TradingSystem::TradingSystem() {}

bool TradingSystem::CanApplyExchange(entt::entity player, entt::entity target, entt::registry& registry) {
    if (!registry.valid(player) || !registry.valid(target)) return false;
    if (player == target) return false;

    // Both must be players
    if (!registry.all_of<TagPlayer>(player) || !registry.all_of<TagPlayer>(target)) return false;

    // Check no existing exchange for either player
    for (const auto& [id, room] : rooms_) {
        if (room.player1 == player || room.player2 == player) return false;
        if (room.player1 == target || room.player2 == target) return false;
    }

    // Check range (within 10 units)
    auto* p1_xform = registry.try_get<Transform>(player);
    auto* p2_xform = registry.try_get<Transform>(target);
    if (!p1_xform || !p2_xform) return false;

    float dist = glm::distance(p1_xform->position, p2_xform->position);
    if (dist > 10.0f) return false;

    return true;
}

uint32_t TradingSystem::CreateExchangeRoom(entt::entity player1, entt::entity player2) {
    ExchangeRoom room;
    room.id = next_room_id_++;
    room.player1 = player1;
    room.player2 = player2;
    room.timeout = 30.0f;
    rooms_[room.id] = room;
    spdlog::info("TradingSystem: exchange room {} created between {} and {}",
                  room.id, static_cast<uint32_t>(player1), static_cast<uint32_t>(player2));
    return room.id;
}

bool TradingSystem::AddItem(entt::entity player, uint32_t room_id, const ExchangeItem& item) {
    auto it = rooms_.find(room_id);
    if (it == rooms_.end()) return false;

    auto& room = it->second;
    if (player == room.player1 && !room.locked1) {
        room.items1.push_back(item);
        return true;
    } else if (player == room.player2 && !room.locked2) {
        room.items2.push_back(item);
        return true;
    }
    return false;
}

bool TradingSystem::Lock(entt::entity player, uint32_t room_id) {
    auto it = rooms_.find(room_id);
    if (it == rooms_.end()) return false;

    auto& room = it->second;
    if (player == room.player1) {
        room.locked1 = true;
        return true;
    } else if (player == room.player2) {
        room.locked2 = true;
        return true;
    }
    return false;
}

bool TradingSystem::InputMoney(entt::entity player, uint32_t room_id, uint32_t gold) {
    auto it = rooms_.find(room_id);
    if (it == rooms_.end()) return false;

    auto& room = it->second;
    if (player == room.player1 && !room.locked1) {
        room.gold1 = gold;
        return true;
    } else if (player == room.player2 && !room.locked2) {
        room.gold2 = gold;
        return true;
    }
    return false;
}

bool TradingSystem::Confirm(entt::entity player, uint32_t room_id, entt::registry& registry) {
    auto it = rooms_.find(room_id);
    if (it == rooms_.end()) return false;

    auto& room = it->second;
    if (player == room.player1) {
        room.confirmed1 = true;
    } else if (player == room.player2) {
        room.confirmed2 = true;
    } else {
        return false;
    }

    if (room.confirmed1 && room.confirmed2) {
        FinalizeExchange(room, registry);
        return true;
    }
    return true;
}

bool TradingSystem::CancelExchange(entt::entity player, uint32_t room_id, entt::registry& registry) {
    auto it = rooms_.find(room_id);
    if (it == rooms_.end()) return false;

    auto& room = it->second;
    if (room.player1 != player && room.player2 != player) return false;

    ReturnItems(room, registry);
    rooms_.erase(it);
    spdlog::info("TradingSystem: exchange room {} cancelled by {}", room_id, static_cast<uint32_t>(player));
    return true;
}

void TradingSystem::UserLogOut(entt::entity player, entt::registry& registry) {
    auto* room = FindRoomByPlayer(player);
    if (!room) return;
    ReturnItems(*room, registry);
    rooms_.erase(room->id);
    spdlog::info("TradingSystem: exchange room {} cleaned up on logout of {}",
                  room->id, static_cast<uint32_t>(player));
}

void TradingSystem::Process(entt::registry& registry, float dt) {
    for (auto it = rooms_.begin(); it != rooms_.end(); ) {
        it->second.timeout -= dt;
        if (it->second.timeout <= 0.0f) {
            ReturnItems(it->second, registry);
            spdlog::debug("TradingSystem: room {} timed out", it->second.id);
            it = rooms_.erase(it);
        } else {
            ++it;
        }
    }
}

void TradingSystem::ReturnItems(ExchangeRoom& room, entt::registry& registry) {
    auto returnToPlayer = [&](entt::entity player, std::vector<ExchangeItem>& items) {
        auto* inv = registry.try_get<Inventory>(player);
        if (!inv) return;
        for (auto& ei : items) {
            ItemSlot slot;
            slot.item_id = ei.item_id;
            slot.item_uid = ei.item_uid;
            slot.count = ei.count;
            slot.enchant = ei.enchant;
            for (size_t i = 0; i < inv->slots.size(); i++) {
                if (inv->slots[i].item_id == 0) {
                    inv->slots[i] = slot;
                    break;
                }
            }
        }
    };
    returnToPlayer(room.player1, room.items1);
    returnToPlayer(room.player2, room.items2);

    // Return gold
    auto* inv1 = registry.try_get<Inventory>(room.player1);
    if (inv1) inv1->gold += room.gold1;
    auto* inv2 = registry.try_get<Inventory>(room.player2);
    if (inv2) inv2->gold += room.gold2;
}

void TradingSystem::FinalizeExchange(ExchangeRoom& room, entt::registry& registry) {
    auto transferItems = [&](entt::entity from, entt::entity to, std::vector<ExchangeItem>& from_items) {
        auto* to_inv = registry.try_get<Inventory>(to);
        if (!to_inv) return;
        for (auto& ei : from_items) {
            ItemSlot slot;
            slot.item_id = ei.item_id;
            slot.item_uid = ei.item_uid;
            slot.count = ei.count;
            slot.enchant = ei.enchant;
            for (size_t i = 0; i < to_inv->slots.size(); i++) {
                if (to_inv->slots[i].item_id == 0) {
                    to_inv->slots[i] = slot;
                    break;
                }
            }
        }
    };

    // Remove items from original owners
    auto removeFromPlayer = [&](entt::entity player, std::vector<ExchangeItem>& items) {
        auto* inv = registry.try_get<Inventory>(player);
        if (!inv) return;
        for (auto& ei : items) {
            for (size_t i = 0; i < inv->slots.size(); i++) {
                if (inv->slots[i].item_uid == ei.item_uid) {
                    inv->slots[i] = ItemSlot{};
                    break;
                }
            }
        }
    };

    removeFromPlayer(room.player1, room.items1);
    removeFromPlayer(room.player2, room.items2);

    // Transfer items
    transferItems(room.player1, room.player2, room.items1);
    transferItems(room.player2, room.player1, room.items2);

    // Transfer gold
    auto* inv1 = registry.try_get<Inventory>(room.player1);
    auto* inv2 = registry.try_get<Inventory>(room.player2);
    if (inv1 && inv2) {
        inv1->gold -= room.gold1;
        inv1->gold += room.gold2;
        inv2->gold -= room.gold2;
        inv2->gold += room.gold1;
    }

    spdlog::info("TradingSystem: exchange room {} finalized", room.id);
}

ExchangeRoom* TradingSystem::FindRoomByPlayer(entt::entity player) {
    for (auto& [id, room] : rooms_) {
        if (room.player1 == player || room.player2 == player) return &room;
    }
    return nullptr;
}

ExchangeRoom* TradingSystem::FindRoom(uint32_t room_id) {
    auto it = rooms_.find(room_id);
    return it != rooms_.end() ? &it->second : nullptr;
}
