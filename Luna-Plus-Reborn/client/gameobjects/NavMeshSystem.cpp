#include "NavMeshSystem.hpp"
#include <functional>
#include <queue>
#include <algorithm>
#include <cmath>
#include <queue>
#include <unordered_map>
#include <cstdint>

NavMeshSystem::NavMeshSystem() {}
NavMeshSystem::~NavMeshSystem() { Shutdown(); }

void NavMeshSystem::Init(float world_size, float grid_size) {
    world_size_ = world_size;
    grid_size_ = std::max(0.5f, grid_size);
    grid_w_ = static_cast<int>((world_size_ * 2.0f) / grid_size_) + 1;
    grid_h_ = grid_w_;
    blocked_.assign(static_cast<size_t>(grid_w_ * grid_h_), 0);
    initialized_ = true;
}

void NavMeshSystem::Shutdown() {
    obstacles_.clear();
    blocked_.clear();
    initialized_ = false;
}

int NavMeshSystem::WorldToGridX(float x) const {
    return static_cast<int>((x + world_size_) / grid_size_);
}

int NavMeshSystem::WorldToGridZ(float z) const {
    return static_cast<int>((z + world_size_) / grid_size_);
}

void NavMeshSystem::GridToWorld(int gx, int gz, float& x, float& z) const {
    x = gx * grid_size_ - world_size_ + grid_size_ * 0.5f;
    z = gz * grid_size_ - world_size_ + grid_size_ * 0.5f;
}

bool NavMeshSystem::IsGridWalkable(int gx, int gz) const {
    if (gx < 0 || gz < 0 || gx >= grid_w_ || gz >= grid_h_) return false;
    return blocked_[static_cast<size_t>(gz * grid_w_ + gx)] == 0;
}

void NavMeshSystem::SetBlocked(int gx, int gz, bool blocked) {
    if (gx < 0 || gz < 0 || gx >= grid_w_ || gz >= grid_h_) return;
    blocked_[static_cast<size_t>(gz * grid_w_ + gx)] = blocked ? 1 : 0;
}

NavPath NavMeshSystem::FindPath(float start_x, float start_z, float end_x, float end_z) const {
    NavPath path;
    if (!initialized_) {
        path.points.push_back({start_x, start_z});
        path.points.push_back({end_x, end_z});
        path.valid = true;
        return path;
    }

    int sx = WorldToGridX(start_x), sz = WorldToGridZ(start_z);
    int ex = WorldToGridX(end_x), ez = WorldToGridZ(end_z);
    if (!IsGridWalkable(ex, ez)) {
        path.points.push_back({start_x, start_z});
        path.points.push_back({end_x, end_z});
        path.valid = true;
        return path;
    }

    struct Node { int gx, gz; float g, f; };
    auto key = [](int gx, int gz) { return (static_cast<int64_t>(gz) << 32) | static_cast<uint32_t>(gx); };
    auto heuristic = [](int ax, int az, int bx, int bz) {
        return std::hypot(static_cast<float>(ax - bx), static_cast<float>(az - bz));
    };

    std::priority_queue<Node, std::vector<Node>, std::function<bool(const Node&, const Node&)>> open(
        [](const Node& a, const Node& b) { return a.f > b.f; });
    std::unordered_map<int64_t, int64_t> came_from;
    std::unordered_map<int64_t, float> g_score;

    Node start{sx, sz, 0.0f, heuristic(sx, sz, ex, ez)};
    open.push(start);
    g_score[key(sx, sz)] = 0.0f;

    static const int dirs[8][2] = {{1,0},{-1,0},{0,1},{0,-1},{1,1},{1,-1},{-1,1},{-1,-1}};
    bool found = false;
    int limit = 0;

    while (!open.empty() && limit++ < 8192) {
        Node cur = open.top();
        open.pop();
        if (cur.gx == ex && cur.gz == ez) {
            found = true;
            break;
        }
        for (auto& d : dirs) {
            int nx = cur.gx + d[0], nz = cur.gz + d[1];
            if (!IsGridWalkable(nx, nz)) continue;
            float step = (d[0] != 0 && d[1] != 0) ? 1.414f : 1.0f;
            float tentative = cur.g + step;
            int64_t nk = key(nx, nz);
            auto it = g_score.find(nk);
            if (it != g_score.end() && tentative >= it->second) continue;
            came_from[nk] = key(cur.gx, cur.gz);
            g_score[nk] = tentative;
            open.push({nx, nz, tentative, tentative + heuristic(nx, nz, ex, ez)});
        }
    }

    if (!found) {
        path.points.push_back({start_x, start_z});
        path.points.push_back({end_x, end_z});
        path.valid = true;
        return path;
    }

    std::vector<NavPoint> rev;
    int cx = ex, cz = ez;
    rev.push_back({end_x, end_z});
    while (!(cx == WorldToGridX(start_x) && cz == WorldToGridZ(start_z))) {
        float wx, wz;
        GridToWorld(cx, cz, wx, wz);
        rev.push_back({wx, wz});
        int64_t ck = key(cx, cz);
        auto it = came_from.find(ck);
        if (it == came_from.end()) break;
        int64_t pk = it->second;
        cx = static_cast<int>(pk & 0xFFFFFFFF);
        cz = static_cast<int>(pk >> 32);
    }
    rev.push_back({start_x, start_z});
    std::reverse(rev.begin(), rev.end());
    path.points = std::move(rev);
    path.valid = true;
    return path;
}

bool NavMeshSystem::IsWalkable(float x, float z) const {
    if (fabs(x) > world_size_ || fabs(z) > world_size_) return false;
    return IsGridWalkable(WorldToGridX(x), WorldToGridZ(z));
}

void NavMeshSystem::AddObstacle(float x, float z, float radius) {
    obstacles_.push_back({x, z});
    int r = std::max(1, static_cast<int>(radius / grid_size_));
    int gx = WorldToGridX(x), gz = WorldToGridZ(z);
    for (int dz = -r; dz <= r; ++dz) {
        for (int dx = -r; dx <= r; ++dx) {
            if (dx * dx + dz * dz <= r * r)
                SetBlocked(gx + dx, gz + dz, true);
        }
    }
}

void NavMeshSystem::ClearObstacles() {
    obstacles_.clear();
    std::fill(blocked_.begin(), blocked_.end(), 0);
}

glm::vec2 NavMeshSystem::Seek(glm::vec2 current, glm::vec2 target, float speed, float dt) const {
    glm::vec2 dir = target - current;
    float dist = glm::length(dir);
    if (dist < 0.1f) return current;
    dir /= dist;
    float step = speed * dt;
    if (step > dist) step = dist;
    glm::vec2 next = current + dir * step;
    if (initialized_ && !IsWalkable(next.x, next.y))
        return current;
    return next;
}

glm::vec2 NavMeshSystem::Flee(glm::vec2 current, glm::vec2 target, float speed, float dt) const {
    glm::vec2 dir = current - target;
    float dist = glm::length(dir);
    if (dist < 0.1f) return current + glm::vec2(1.0f, 0.0f) * speed * dt;
    dir /= dist;
    return current + dir * speed * dt;
}
