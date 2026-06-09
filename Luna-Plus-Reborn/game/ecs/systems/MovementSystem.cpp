#include "MovementSystem.hpp"
#include "../components/Transform.hpp"
#include "../components/Movement.hpp"
#include "../components/CombatState.hpp"
#include <spdlog/spdlog.h>
#include <queue>
#include <unordered_set>
#include <algorithm>
#include <cmath>

static void MoveToNextWaypoint(entt::registry& registry, entt::entity entity, float dt);

// ─── Core Update (Old: NextMove loop) ───

void MovementSystem::Update(entt::registry& registry, float dt) {
    auto view = registry.view<Transform, Movement>();
    for (auto entity : view) {
        if (auto* combat = registry.try_get<CombatState>(entity)) {
            if (combat->is_casting || combat->is_animation_locked) {
                continue;
            }
        }

        auto& xform = view.get<Transform>(entity);
        auto& move = view.get<Movement>(entity);

        // Waypoint path following
        if (move.is_moving && move.use_pathfinding) {
            MoveToNextWaypoint(registry, entity, dt);
            continue;
        }

        if (!move.is_moving) continue;

        // Direct movement (Old: Move_Simple)
        glm::vec3 diff = move.destination - xform.position;
        float dist = glm::length(diff);

        if (dist < 0.1f) {
            xform.position = move.destination;
            move.is_moving = false;
            move.velocity = {0.0f, 0.0f, 0.0f};
            continue;
        }

        glm::vec3 dir = diff / dist;
        float step = move.current_speed * dt;
        if (step > dist) step = dist;

        xform.position += dir * step;
        move.velocity = dir * move.current_speed;
    }
}

// ─── Move request (Old: Move_UsePath entry) ───

void MovementSystem::HandleMoveRequest(entt::registry& registry, entt::entity entity,
                                        const glm::vec3& destination, float speed) {
    if (!registry.valid(entity)) return;
    auto& xform = registry.get<Transform>(entity);
    auto& move = registry.get<Movement>(entity);

    // Old: Move_UsePath — try line-of-sight first, else pathfind
    if (HasLineOfSight(xform.position, destination)) {
        // Direct move (Old: Move_Simple)
        move.destination = destination;
        move.speed = speed;
        move.current_speed = speed;
        move.is_moving = true;
        move.use_pathfinding = false;
        move.path.clear();
        move.path_index = 0;
    } else {
        // Pathfind around obstacles (Old: MAP->PathFind A*)
        auto path = FindPath(xform.position, destination);
        if (!path.empty()) {
            MoveAlongPath(registry, entity, path);
            move.speed = speed;
            move.current_speed = speed;
        } else {
            // Fallback: direct move even if blocked (Old behavior)
            move.destination = destination;
            move.speed = speed;
            move.current_speed = speed;
            move.is_moving = true;
            move.use_pathfinding = false;
        }
    }
}

void MovementSystem::HandleStopMove(entt::registry& registry, entt::entity entity) {
    if (!registry.valid(entity)) return;
    auto& move = registry.get<Movement>(entity);
    move.is_moving = false;
    move.velocity = {0.0f, 0.0f, 0.0f};
    move.path.clear();
    move.path_index = 0;
    move.use_pathfinding = false;
}

// ─── WayPoint pathfinding (A* grid; Old: Hero::Move_UsePath + CWayPointManager) ───

static constexpr float GRID_SIZE = 2.0f;
static constexpr float AGENT_RADIUS = 0.5f;

struct AStarNode {
    glm::vec3 pos;
    float g, h;
    glm::vec3 parent_pos;
};

float Heuristic(const glm::vec3& a, const glm::vec3& b) {
    return glm::abs(a.x - b.x) + glm::abs(a.z - b.z); // Manhattan
}

bool MovementSystem::IsWalkable(const glm::vec3& pos) const {
    if (pos.x < -50.0f || pos.x > 50.0f || pos.z < -50.0f || pos.z > 50.0f)
        return false;
    return pos.y >= -10.0f;
}

bool MovementSystem::HasLineOfSight(const glm::vec3& from, const glm::vec3& to) const {
    // Old: MAP->CollisionCheck_OneLine_New
    // Sample points along line and check if each is walkable
    glm::vec3 dir = to - from;
    float dist = glm::length(dir);
    if (dist < 0.1f) return true;
    dir /= dist;
    int steps = static_cast<int>(dist / 1.0f) + 1;
    for (int i = 1; i < steps; ++i) {
        glm::vec3 sample = from + dir * static_cast<float>(i) * 1.0f;
        if (!IsWalkable(sample)) return false;
    }
    return true;
}

std::vector<WayPoint> MovementSystem::FindPath(const glm::vec3& from, const glm::vec3& to) {
    std::vector<WayPoint> result;
    if (!IsWalkable(to)) return result;

    glm::vec3 start_grid = glm::vec3(
        std::round(from.x / GRID_SIZE) * GRID_SIZE, 0.0f,
        std::round(from.z / GRID_SIZE) * GRID_SIZE);
    glm::vec3 end_grid = glm::vec3(
        std::round(to.x / GRID_SIZE) * GRID_SIZE, 0.0f,
        std::round(to.z / GRID_SIZE) * GRID_SIZE);

    // A* search — pakai parent_pos (glm::vec3) bukan index agar swap-pop aman
    std::vector<AStarNode> open;
    std::unordered_set<uint64_t> closed;
    open.push_back({start_grid, 0.0f, Heuristic(start_grid, end_grid), glm::vec3{-1}});

    const int dx[] = {1, -1, 0, 0, 1, 1, -1, -1};
    const int dz[] = {0, 0, 1, -1, 1, -1, 1, -1};

    constexpr float GRID_EPSILON = 1.0f;

    while (!open.empty()) {
        size_t best = 0;
        for (size_t i = 1; i < open.size(); ++i)
            if (open[i].g + open[i].h < open[best].g + open[best].h)
                best = i;

        AStarNode cur = open[best];
        open.erase(open.begin() + static_cast<ptrdiff_t>(best)); // stable remove (no swap)

        uint64_t key = (static_cast<uint64_t>(static_cast<int>(cur.pos.x)) << 32) |
                        static_cast<uint64_t>(static_cast<int>(cur.pos.z));
        if (closed.count(key)) continue;
        closed.insert(key);

        if (glm::distance(cur.pos, end_grid) < GRID_SIZE) {
            // Reconstruct path via parent_pos
            std::vector<glm::vec3> rev;
            rev.push_back(end_grid);
            glm::vec3 p = cur.pos;
            rev.push_back(p);
            while (std::abs(p.x + 1.0f) > 0.1f || std::abs(p.z + 1.0f) > 0.1f) {
                // cari parent di open atau closed (closed tidak simpan parent, jadi cari di open)
                bool found = false;
                for (auto& node : open) {
                    if (glm::distance(node.pos, p) < GRID_EPSILON && node.parent_pos.x >= -0.5f) {
                        p = node.parent_pos;
                        rev.push_back(p);
                        found = true;
                        break;
                    }
                }
                if (!found) break;
            }
            std::reverse(rev.begin(), rev.end());
            for (auto& wp : rev) {
                wp.y = 0.0f;
                result.push_back({wp, false});
            }
            result.push_back({to, false});
            return result;
        }

        for (int i = 0; i < 8; ++i) {
            glm::vec3 next(cur.pos.x + dx[i] * GRID_SIZE, 0.0f, cur.pos.z + dz[i] * GRID_SIZE);
            uint64_t nk = (static_cast<uint64_t>(static_cast<int>(next.x)) << 32) |
                           static_cast<uint64_t>(static_cast<int>(next.z));
            if (closed.count(nk)) continue;
            if (!IsWalkable(next)) continue;

            float ng = cur.g + (dx[i] != 0 && dz[i] != 0 ? 1.414f : 1.0f) * GRID_SIZE;
            open.push_back({next, ng, Heuristic(next, end_grid), cur.pos});
        }
    }
    return result;
}

void MovementSystem::MoveAlongPath(entt::registry& registry, entt::entity entity,
                                    const std::vector<WayPoint>& path) {
    auto& move = registry.get<Movement>(entity);
    move.path.clear();
    for (auto& wp : path) move.path.push_back(wp.position);
    move.path_index = 0;
    move.use_pathfinding = true;
    move.is_moving = true;

    if (!move.path.empty()) {
        move.destination = move.path[0];
    }
}

void MoveToNextWaypoint(entt::registry& registry, entt::entity entity, float dt) {
    auto& xform = registry.get<Transform>(entity);
    auto& move = registry.get<Movement>(entity);

    if (move.path_index >= move.path.size()) {
        move.is_moving = false;
        move.use_pathfinding = false;
        move.velocity = {0.0f, 0.0f, 0.0f};
        return;
    }

    glm::vec3 target = move.path[move.path_index];
    glm::vec3 diff = target - xform.position;
    float dist = glm::length(diff);

    if (dist < 0.3f) {
        xform.position = target;
        move.path_index++;
        if (move.path_index < move.path.size()) {
            move.destination = move.path[move.path_index];
        } else {
            move.is_moving = false;
            move.use_pathfinding = false;
            move.velocity = {0.0f, 0.0f, 0.0f};
        }
        return;
    }

    glm::vec3 dir = diff / dist;
    float step = move.current_speed * dt;
    if (step > dist) step = dist;

    xform.position += dir * step;
    move.velocity = dir * move.current_speed;
    move.destination = target;
}

// ─── KyungGong dash (Old: KyungGongIdx in MP_MOVE_ONETARGET) ───

void MovementSystem::HandleKyungGong(entt::registry& registry, entt::entity entity,
                                      uint16_t skill_idx, const glm::vec3& direction) {
    if (!registry.valid(entity)) return;
    auto& xform = registry.get<Transform>(entity);
    auto& move = registry.get<Movement>(entity);

    float dash_distance = 8.0f; // Old KyungGong dash distance
    glm::vec3 dir = glm::length(direction) > 0.001f
                        ? glm::normalize(direction)
                        : glm::vec3(1.0f, 0.0f, 0.0f);

    glm::vec3 target = xform.position + dir * dash_distance;
    // Clamp to walkable area
    if (!IsWalkable(target)) {
        // Scan along direction for nearest walkable point
        for (float d = dash_distance; d > 1.0f; d -= 1.0f) {
            target = xform.position + dir * d;
            if (IsWalkable(target)) break;
        }
    }

    // Instant dash (no pathfinding — Old behavior: direct jump)
    xform.position = target;
    move.destination = target;
    move.is_moving = false;
    move.velocity = dir * 20.0f; // High burst velocity indicator
    move.use_pathfinding = false;
    move.path.clear();

    spdlog::info("KyungGong dash: entity={} skill_idx={} target=({:.1f},{:.1f})",
                 static_cast<uint32_t>(entity), skill_idx, target.x, target.z);
}

// ─── Knockback / forced movement (Old: MP_MOVE_EFFECTMOVE / HeroEffectMove) ───

void MovementSystem::ApplyKnockback(entt::registry& registry, entt::entity entity,
                                     const glm::vec3& direction, float distance) {
    if (!registry.valid(entity)) return;
    auto& xform = registry.get<Transform>(entity);
    auto& move = registry.get<Movement>(entity);

    glm::vec3 dir = glm::length(direction) > 0.001f
                        ? glm::normalize(direction)
                        : glm::vec3(0.0f, 0.0f, -1.0f);

    glm::vec3 target = xform.position + dir * distance;
    // Clamp to map bounds
    target.x = std::clamp(target.x, -50.0f, 50.0f);
    target.z = std::clamp(target.z, -50.0f, 50.0f);
    if (!IsWalkable(target)) {
        target = xform.position;
    }

    xform.position = target;
    move.destination = target;
    move.is_moving = false;
    move.velocity = dir * 10.0f;
    move.use_pathfinding = false;
    move.path.clear();

    spdlog::debug("Knockback: entity={} dist={:.1f} target=({:.1f},{:.1f})",
                  static_cast<uint32_t>(entity), distance, target.x, target.z);
}

// ─── Fade move / teleport (Old: MP_FADEMOVE_SYN) ───

void MovementSystem::HandleFadeMove(entt::registry& registry, entt::entity entity,
                                     const glm::vec3& target) {
    if (!registry.valid(entity)) return;
    if (!IsWalkable(target)) {
        spdlog::warn("FadeMove: target unwalkable ({} {} {})", target.x, target.y, target.z);
        return;
    }

    auto& xform = registry.get<Transform>(entity);
    auto& move = registry.get<Movement>(entity);

    xform.position = target;
    move.destination = target;
    move.is_moving = false;
    move.velocity = {0.0f, 0.0f, 0.0f};
    move.use_pathfinding = false;
    move.path.clear();

    spdlog::info("FadeMove: entity={} to ({:.1f},{:.1f})",
                 static_cast<uint32_t>(entity), target.x, target.z);
}
