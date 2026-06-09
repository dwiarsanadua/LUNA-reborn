#include "RecallSystem.h"
#include <ecs/components/Tag.hpp>
#include <ecs/components/Transform.hpp>
#include <ecs/components/CharacterStats.hpp>
#include <spdlog/spdlog.h>
#include <random>
#include <algorithm>

static std::mt19937 s_rng(std::random_device{}());

void RecallSystem::RequestRecall(uint32_t character_id, const RecallTarget& target, uint32_t skill_id) {
    RecallRequest req;
    req.id = next_request_id_++;
    req.character_id = character_id;
    req.target_char_id = target.target_char_id;
    req.target_map_id = target.target_map_id;
    req.target_position = target.target_position;
    req.timeout = 30.0f;
    req.key = std::uniform_int_distribution<uint32_t>(100000, 999999)(s_rng);
    req.skill_id = skill_id;

    requests_[req.id] = req;

    spdlog::info("RecallSystem: recall {} requested by {} to target {} (key={})",
                  req.id, character_id, target.target_char_id, req.key);
}

void RecallSystem::Recall(uint32_t character_id, entt::registry& registry) {
    auto it = bind_points_.find(character_id);
    if (it == bind_points_.end()) {
        spdlog::warn("RecallSystem: no bind point for character {}", character_id);
        return;
    }

    auto& bp = it->second;
    auto view = registry.view<TagPlayer, CharacterStats>();
    for (auto entity : view) {
        auto* stats = registry.try_get<CharacterStats>(entity);
        if (stats && static_cast<uint32_t>(entity) == character_id) {
            auto* xform = registry.try_get<Transform>(entity);
            if (xform) {
                xform->position = bp.position;
                spdlog::info("RecallSystem: character {} recalled to bind point at ({:.1f},{:.1f},{:.1f})",
                              character_id, bp.position.x, bp.position.y, bp.position.z);
            }
            break;
        }
    }
}

void RecallSystem::RecallParty(uint32_t character_id, entt::registry& registry, uint32_t party_id) {
    auto view = registry.view<TagPlayer, CharacterStats>();
    for (auto entity : view) {
        auto* stats = registry.try_get<CharacterStats>(entity);
        if (!stats) continue;
        uint32_t id = static_cast<uint32_t>(entity);
        if (id == character_id) continue;
        // In full implementation, check if entity is in party
        // For now, recall all nearby party members
        auto* xform = registry.try_get<Transform>(entity);
        if (xform) {
            auto caller_it = bind_points_.find(character_id);
            if (caller_it != bind_points_.end()) {
                xform->position = caller_it->second.position;
                spdlog::info("RecallSystem: party member {} recalled to caller's position", id);
            }
        }
    }
    spdlog::info("RecallSystem: party recall initiated by character {}", character_id);
}

void RecallSystem::BindLocation(uint32_t character_id, int map_id, const glm::vec3& position) {
    BindPoint bp;
    bp.character_id = character_id;
    bp.map_id = map_id;
    bp.position = position;
    bind_points_[character_id] = bp;
    spdlog::info("RecallSystem: character {} bound to map {} at ({:.1f},{:.1f},{:.1f})",
                  character_id, map_id, position.x, position.y, position.z);
}

const BindPoint* RecallSystem::GetBindPoint(uint32_t character_id) const {
    auto it = bind_points_.find(character_id);
    if (it != bind_points_.end()) return &it->second;
    return nullptr;
}

bool RecallSystem::AcceptRecall(uint32_t character_id, uint32_t key) {
    for (auto& [id, req] : requests_) {
        if (req.target_char_id == character_id && req.key == key && req.timeout > 0) {
            NotifyRecall(character_id, req);
            return true;
        }
    }
    spdlog::warn("RecallSystem: invalid recall accept for {} with key {}", character_id, key);
    return false;
}

void RecallSystem::Process(entt::registry& registry, float dt) {
    for (auto it = requests_.begin(); it != requests_.end(); ) {
        it->second.timeout -= dt;
        if (it->second.timeout <= 0.0f) {
            spdlog::debug("RecallSystem: request {} timed out", it->second.id);
            it = requests_.erase(it);
        } else {
            ++it;
        }
    }
}

void RecallSystem::ExecuteRecall(uint32_t request_id, entt::registry& registry) {
    auto it = requests_.find(request_id);
    if (it == requests_.end()) return;

    auto& req = it->second;

    // Find the target entity
    auto view = registry.view<TagPlayer, CharacterStats>();
    for (auto entity : view) {
        auto* stats = registry.try_get<CharacterStats>(entity);
        if (stats && static_cast<uint32_t>(entity) == req.target_char_id) {
            auto* xform = registry.try_get<Transform>(entity);
            if (xform) {
                xform->position = req.target_position;
                spdlog::info("RecallSystem: entity {} recalled to ({:.1f},{:.1f},{:.1f})",
                              req.target_char_id, req.target_position.x,
                              req.target_position.y, req.target_position.z);
            }
            break;
        }
    }

    requests_.erase(it);
}

void RecallSystem::NotifyRecall(uint32_t character_id, const RecallRequest& request) {
    spdlog::info("RecallSystem: [NOTIFY] Character {} has accepted recall request {} (skill_id={})",
                  character_id, request.id, request.skill_id);
}
