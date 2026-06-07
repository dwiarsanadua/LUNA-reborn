#pragma once
#include <glm/glm.hpp>
#include <rendering/TerrainRenderer.hpp>

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
    
    // Get terrain height at position, with collision
    float GetHeight(float x, float z) const {
        if (!terrain_) return 0;
        return terrain_->GetHeight(x, z);
    }
    
    // Check if position is walkable (not too steep)
    bool IsWalkable(float x, float z) const {
        float h = GetHeight(x, z);
        // Check nearby points to detect steepness
        float hx = GetHeight(x + 0.5f, z);
        float hz = GetHeight(x, z + 0.5f);
        float slope = glm::max(glm::abs(hx - h), glm::abs(hz - h));
        return slope < 5.0f; // Max 5 unit rise per 0.5 unit run
    }
    
    // Resolve collision: clamp position to walkable area
    glm::vec3 Resolve(const glm::vec3& pos, float radius) const {
        glm::vec3 result = pos;
        if (!terrain_) return result;
        
        // Prevent going outside terrain bounds
        result.x = glm::clamp(result.x, -100.0f, 100.0f);
        result.z = glm::clamp(result.z, -100.0f, 100.0f);
        
        // Get terrain height
        float h = GetHeight(result.x, result.z);
        if (h > -1000 && h < 10000) result.y = h + 1.0f;
        
        return result;
    }

private:
    TerrainRenderer* terrain_ = nullptr;
};
