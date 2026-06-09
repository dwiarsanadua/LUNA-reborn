#pragma once
#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <vector>
#include <cstdint>

struct WayPoint {
    glm::vec3 position;
    bool is_teleport = false;
};

class MovementSystem {
public:
    void Update(entt::registry& registry, float dt);
    void HandleMoveRequest(entt::registry& registry, entt::entity entity,
                           const glm::vec3& destination, float speed);
    void HandleStopMove(entt::registry& registry, entt::entity entity);
    bool IsWalkable(const glm::vec3& pos) const;

    // WayPoint pathfinding (A* via simple grid; Old: Hero::Move_UsePath)
    std::vector<WayPoint> FindPath(const glm::vec3& from, const glm::vec3& to);
    bool HasLineOfSight(const glm::vec3& from, const glm::vec3& to) const;
    void MoveAlongPath(entt::registry& registry, entt::entity entity,
                       const std::vector<WayPoint>& path);

    // KyungGong dash (Old: KyungGongIdx in MP_MOVE_ONETARGET)
    void HandleKyungGong(entt::registry& registry, entt::entity entity,
                         uint16_t skill_idx, const glm::vec3& direction);

    // Knockback / forced movement (Old: MP_MOVE_EFFECTMOVE / HeroEffectMove)
    void ApplyKnockback(entt::registry& registry, entt::entity entity,
                        const glm::vec3& direction, float distance);

    // Fade move (teleport; Old: MP_FADEMOVE_SYN)
    void HandleFadeMove(entt::registry& registry, entt::entity entity,
                        const glm::vec3& target);
};
