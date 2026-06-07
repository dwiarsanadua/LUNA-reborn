#include "PetSystem.hpp"
#include "../components/Transform.hpp"
#include "../components/CharacterStats.hpp"
#include "../components/Tag.hpp"
#include "../components/Inventory.hpp"
#include <spdlog/spdlog.h>
#include <random>

struct PetInstance {
    uint32_t pet_id;
    uint32_t owner_id;
    uint16_t level = 1;
    uint32_t exp = 0;
    uint32_t exp_next = 100;
    uint16_t hunger = 100;
    std::string name;
    glm::vec3 pos{0};
};

static std::unordered_map<uint32_t, PetInstance> g_pets;
static std::mt19937 pet_rng(std::random_device{}());

void PetSystem::SummonPet(entt::registry& reg, entt::entity player, uint32_t pet_id) {
    if (!reg.valid(player)) return;
    uint32_t pid = static_cast<uint32_t>(player);
    // Check if already summoned
    for (auto& [id, p] : g_pets)
        if (p.owner_id == pid) { spdlog::warn("Pet already active"); return; }
    PetInstance p;
    p.pet_id = pet_id;
    p.owner_id = pid;
    p.level = 1;
    p.exp = 0;
    p.exp_next = 100;
    p.name = "Pet_" + std::to_string(pet_id);
    if (reg.all_of<Transform>(player))
        p.pos = reg.get<Transform>(player).position;
    uint32_t hash = pid * 1000 + pet_id;
    g_pets[hash] = p;
    spdlog::info("Player {} summoned pet {} (Lv.{})", pid, pet_id, p.level);
}

void PetSystem::FeedPet(entt::registry& reg, entt::entity pet) {
    uint32_t pid = static_cast<uint32_t>(pet);
    for (auto& [id, p] : g_pets) {
        if (p.owner_id == pid) {
            p.hunger = std::min(100, p.hunger + 30);
            p.exp += 10;
            if (p.exp >= p.exp_next) {
                p.exp -= p.exp_next;
                p.level++;
                p.exp_next = p.level * 150;
                spdlog::info("Pet {} leveled up to Lv.{}!", p.name, p.level);
            }
            spdlog::info("Pet fed: hunger={} exp={}/{}", p.hunger, p.exp, p.exp_next);
            return;
        }
    }
}

void PetSystem::DespawnPet(entt::registry& reg, entt::entity player) {
    uint32_t pid = static_cast<uint32_t>(player);
    for (auto it = g_pets.begin(); it != g_pets.end(); ) {
        if (it->second.owner_id == pid) it = g_pets.erase(it);
        else ++it;
    }
}

void PetSystem::Update(entt::registry& reg, float dt) {
    (void)reg;
    for (auto& [id, p] : g_pets) {
        if (p.hunger > 0) p.hunger = std::max(0, p.hunger - static_cast<int>(dt * 0.5f));
        if (p.hunger == 0 && pet_rng() % 1000 == 0) {
            p.exp = std::max(0, static_cast<int>(p.exp) - 1);
        }
    }
}
