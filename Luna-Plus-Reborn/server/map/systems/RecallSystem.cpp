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
