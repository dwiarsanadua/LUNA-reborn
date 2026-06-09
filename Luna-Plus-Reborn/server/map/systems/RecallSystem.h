#pragma once
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>
#include <entt/entt.hpp>
#include <glm/glm.hpp>

struct RecallTarget {
    uint32_t target_char_id = 0;
    int target_map_id = 0;
    glm::vec3 target_position{0.0f};
    std::string target_name;
};

struct RecallRequest {
    uint32_t id;
    uint32_t character_id;
    uint32_t target_char_id;
    int target_map_id;
    glm::vec3 target_position;
    float timeout;
    uint32_t key;
    uint32_t skill_id;
};

class RecallSystem {
public:
    void RequestRecall(uint32_t character_id, const RecallTarget& target, uint32_t skill_id);
    bool AcceptRecall(uint32_t character_id, uint32_t key);
    void Process(entt::registry& registry, float dt);

private:
    std::unordered_map<uint32_t, RecallRequest> requests_;
    uint32_t next_request_id_ = 1;

    void ExecuteRecall(uint32_t request_id, entt::registry& registry);
    void NotifyRecall(uint32_t character_id, const RecallRequest& request);
};
