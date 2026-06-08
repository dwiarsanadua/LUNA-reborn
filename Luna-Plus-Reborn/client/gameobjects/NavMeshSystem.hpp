#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <cstdint>

// Stub for NavMesh pathfinding integration.
// Full implementation requires Recast/Detour library.
// This stub provides:
// - Simple straight-line pathfinding (direct chase)
// - Obstacle avoidance via waypoints
// - Interface for future Recast/Detour integration

struct NavPoint {
    float x, z;
};

struct NavPath {
    std::vector<NavPoint> points;
    int current = 0;
    bool valid = false;
    
    NavPoint GetTarget() const {
        if (current < (int)points.size()) return points[current];
        return {0, 0};
    }
    
    bool IsFinished() const { return current >= (int)points.size(); }
    void Advance() { if (current < (int)points.size()) current++; }
};

class NavMeshSystem {
public:
    NavMeshSystem();
    ~NavMeshSystem();
    
    void Init(float world_size = 200.0f, float grid_size = 1.0f);
    void Shutdown();
    
    // Find path from start to end (stub: returns straight line)
    NavPath FindPath(float start_x, float start_z, float end_x, float end_z) const;
    
    // Check if a point is walkable
    bool IsWalkable(float x, float z) const;
    
    // Set obstacles (for future Recast/Detour integration)
    void AddObstacle(float x, float z, float radius);
    void ClearObstacles();
    
    // Simple steering: returns new position seeking target
    glm::vec2 Seek(glm::vec2 current, glm::vec2 target, float speed, float dt) const;
    glm::vec2 Flee(glm::vec2 current, glm::vec2 target, float speed, float dt) const;
    
    bool IsInitialized() const { return initialized_; }

private:
    int WorldToGridX(float x) const;
    int WorldToGridZ(float z) const;
    void GridToWorld(int gx, int gz, float& x, float& z) const;
    bool IsGridWalkable(int gx, int gz) const;
    void SetBlocked(int gx, int gz, bool blocked);

    bool initialized_ = false;
    float world_size_ = 200.0f;
    float grid_size_ = 1.0f;
    int grid_w_ = 0;
    int grid_h_ = 0;
    std::vector<uint8_t> blocked_;
    std::vector<glm::vec2> obstacles_;
};
