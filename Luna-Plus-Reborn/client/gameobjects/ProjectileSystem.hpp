#pragma once
#include <string>
#include <glm/glm.hpp>
#include <cstdint>
#include <vector>
#include <functional>

class UIRenderer;

enum class ProjectileType {
    Arrow, Magic, Thrown, Bullet
};

struct Projectile {
    uint32_t id = 0;
    ProjectileType type = ProjectileType::Magic;
    glm::vec3 position{0};
    glm::vec3 velocity{0};
    glm::vec3 target_pos{0};
    uint32_t source_id = 0;
    uint32_t target_id = UINT32_MAX;
    int damage = 0;
    float lifetime = 5.0f;
    float age = 0;
    float speed = 10.0f;
    float radius = 0.3f;    // Collision radius
    bool active = true;
    bool hit = false;
    uint32_t color = 0xffff8800;
    float size = 4.0f;
    std::string hit_effect;
    std::function<void(uint32_t target_id, int damage)> on_hit;
};

enum class AoEShape {
    Circle, Cone, Line, Cross
};

struct AoEInstance {
    uint32_t id = 0;
    AoEShape shape = AoEShape::Circle;
    glm::vec3 origin{0};
    glm::vec3 direction{0, 0, -1}; // For cone/line
    float radius = 3.0f;           // Circle radius
    float angle = 45.0f;           // Cone angle in degrees
    float length = 8.0f;           // Line/cone length
    float width = 2.0f;            // Line width
    int damage = 0;
    float lifetime = 0.5f;
    float age = 0;
    uint32_t source_id = 0;
    bool active = true;
    bool friendly = false;
    std::string hit_effect;
    uint32_t color = 0x44aaff44;
    std::vector<uint32_t> hit_targets; // Already hit
    std::function<void(uint32_t target_id, int damage)> on_hit;
};

class ProjectileSystem {
public:
    void Update(float dt);
    void Render(UIRenderer& ui);
    
    uint32_t Spawn(const glm::vec3& from, const glm::vec3& to, 
                   ProjectileType type, int damage, float speed = 10.0f);
    uint32_t SpawnTargeted(uint32_t source_id, uint32_t target_id,
                           const glm::vec3& from, const glm::vec3& to,
                           int damage, float speed = 10.0f);
    
    uint32_t CreateAoE(const glm::vec3& origin, AoEShape shape, float radius,
                       int damage, float lifetime = 0.5f);
    uint32_t CreateCone(const glm::vec3& origin, const glm::vec3& direction,
                        float angle, float length, int damage);
    uint32_t CreateLine(const glm::vec3& origin, const glm::vec3& direction,
                        float length, float width, int damage);
    
    bool TestCircle(const glm::vec3& origin, float radius, const glm::vec3& point) const;
    bool TestCone(const glm::vec3& origin, const glm::vec3& dir, float angle, float length, const glm::vec3& point) const;
    bool TestLine(const glm::vec3& origin, const glm::vec3& dir, float length, float width, const glm::vec3& point) const;
    
    const std::vector<Projectile>& GetProjectiles() const { return projectiles_; }
    const std::vector<AoEInstance>& GetAoEs() const { return aoes_; }
    void Clear();
    
private:
    std::vector<Projectile> projectiles_;
    std::vector<AoEInstance> aoes_;
    uint32_t next_id_ = 1;
};
