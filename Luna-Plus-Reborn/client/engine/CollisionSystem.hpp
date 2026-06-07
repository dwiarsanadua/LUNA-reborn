#pragma once
#include <glm/glm.hpp>
#include <rendering/TerrainRenderer.hpp>

class PhysicsWorld;

struct AABB {
    glm::vec3 min{0}, max{0};
    bool Contains(const glm::vec3& point) const {
        return point.x >= min.x && point.x <= max.x &&
               point.z >= min.z && point.z <= max.z;
    }
};

class CollisionSystem {
public:
    void SetTerrain(TerrainRenderer* t) { terrain_ = t; }
    void SetPhysicsWorld(PhysicsWorld* world) { physics_world_ = world; }
    
    float GetHeight(float x, float z) const {
        if (!terrain_) return 0;
        return terrain_->GetHeight(x, z);
    }
    
    bool IsWalkable(float x, float z) const {
        float h = GetHeight(x, z);
        float hx = GetHeight(x + 0.5f, z);
        float hz = GetHeight(x, z + 0.5f);
        float slope = glm::max(glm::abs(hx - h), glm::abs(hz - h));
        if (slope >= 5.0f) return false;

        // Physics raycast check: cast from above to detect ground
        if (physics_world_) {
            glm::vec3 from(x, h + 10.0f, z);
            glm::vec3 to(x, h - 10.0f, z);
            glm::vec3 hit;
            if (physics_world_->RayCast(from, to, hit)) {
                float ground_dist = h - hit.y;
                if (ground_dist > 2.0f) return false; // too far from ground
            }
        }
        return true;
    }
    
    glm::vec3 Resolve(const glm::vec3& pos, float radius) const {
        glm::vec3 result = pos;
        if (!terrain_) return result;
        
        result.x = glm::clamp(result.x, -100.0f, 100.0f);
        result.z = glm::clamp(result.z, -100.0f, 100.0f);
        
        float h = GetHeight(result.x, result.z);
        if (h > -1000 && h < 10000) result.y = h + 1.0f;

        // Physics resolve
        if (physics_world_) {
            glm::vec3 hit;
            // Raycast downward to snap to terrain
            glm::vec3 from(result.x, result.y + 5.0f, result.z);
            glm::vec3 to(result.x, result.y - 5.0f, result.z);
            if (physics_world_->RayCast(from, to, hit)) {
                result.y = hit.y + 1.0f;
            }
        }
        
        return result;
    }

    // New overload: resolves with velocity-based collision
    void Resolve(glm::vec3 pos, glm::vec3 vel, glm::vec3& out_new_pos) const {
        glm::vec3 result = pos + vel * 0.016f; // ~60fps step
        result = Resolve(result, 0.5f);
        if (physics_world_) {
            glm::vec3 hit;
            if (physics_world_->RayCast(pos, result, hit)) {
                result = hit;
            }
        }
        out_new_pos = result;
    }

private:
    TerrainRenderer* terrain_ = nullptr;
    PhysicsWorld* physics_world_ = nullptr;
};
