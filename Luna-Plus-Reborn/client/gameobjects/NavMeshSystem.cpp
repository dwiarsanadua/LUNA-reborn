#include "NavMeshSystem.hpp"
#include <algorithm>
#include <cmath>

NavMeshSystem::NavMeshSystem() {}
NavMeshSystem::~NavMeshSystem() { Shutdown(); }

void NavMeshSystem::Init(float world_size, float grid_size) {
    world_size_ = world_size;
    grid_size_ = grid_size;
    initialized_ = true;
}

void NavMeshSystem::Shutdown() {
    obstacles_.clear();
    initialized_ = false;
}

NavPath NavMeshSystem::FindPath(float start_x, float start_z, float end_x, float end_z) const {
    NavPath path;
    // Straight-line stub: just return start and end points
    path.points.push_back({start_x, start_z});
    path.points.push_back({end_x, end_z});
    path.valid = true;
    return path;
}

bool NavMeshSystem::IsWalkable(float x, float z) const {
    // Simple bounds check
    if (fabs(x) > world_size_ || fabs(z) > world_size_) return false;
    // Check obstacles
    for (auto& obs : obstacles_) {
        float dx = x - obs.x, dz = z - obs.y;
        if (dx * dx + dz * dz < 2.0f) return false;
    }
    return true;
}

void NavMeshSystem::AddObstacle(float x, float z, float radius) {
    (void)radius;
    obstacles_.push_back({x, z});
}

void NavMeshSystem::ClearObstacles() {
    obstacles_.clear();
}

glm::vec2 NavMeshSystem::Seek(glm::vec2 current, glm::vec2 target, float speed, float dt) const {
    glm::vec2 dir = target - current;
    float dist = glm::length(dir);
    if (dist < 0.1f) return current;
    dir /= dist;
    float step = speed * dt;
    if (step > dist) step = dist;
    return current + dir * step;
}

glm::vec2 NavMeshSystem::Flee(glm::vec2 current, glm::vec2 target, float speed, float dt) const {
    glm::vec2 dir = current - target;
    float dist = glm::length(dir);
    if (dist < 0.1f) return current + glm::vec2(rand() % 5, rand() % 5);
    dir /= dist;
    return current + dir * speed * dt;
}
