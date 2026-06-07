#include "ProjectileSystem.hpp"
#include <rendering/UIRenderer.hpp>
#include <cmath>
#include <algorithm>
#include <cstdlib>

void ProjectileSystem::Update(float dt) {
    // Update projectiles
    for (auto& p : projectiles_) {
        if (!p.active) continue;
        p.age += dt;
        if (p.age > p.lifetime) { p.active = false; continue; }
        // Move
        glm::vec3 dir = p.velocity;
        float len = glm::length(dir);
        if (len > 0.001f) {
            dir /= len;
            p.position += dir * p.speed * dt;
        }
    }
    
    // Update AoEs
    for (auto& aoe : aoes_) {
        if (!aoe.active) continue;
        aoe.age += dt;
        if (aoe.age > aoe.lifetime) aoe.active = false;
    }
    
    // Cleanup
    projectiles_.erase(
        std::remove_if(projectiles_.begin(), projectiles_.end(),
            [](auto& p) { return !p.active; }),
        projectiles_.end());
    aoes_.erase(
        std::remove_if(aoes_.begin(), aoes_.end(),
            [](auto& a) { return !a.active; }),
        aoes_.end());
}

void ProjectileSystem::Render(UIRenderer& ui) {
    for (auto& p : projectiles_) {
        if (!p.active) continue;
        float sx = (p.position.x * 12.0f + 640.0f) - p.size * 0.5f;
        float sy = (p.position.z * 12.0f + 360.0f) - p.size * 0.5f - p.position.y * 8.0f;
        if (sx < -50 || sx > 1330 || sy < -50 || sy > 770) continue;
        ui.DrawRect(sx, sy, p.size, p.size, {
            (uint8_t)((p.color >> 24) & 0xFF), (uint8_t)((p.color >> 16) & 0xFF),
            (uint8_t)((p.color >> 8) & 0xFF), (uint8_t)(p.color & 0xFF)
        });
    }
    
    for (auto& aoe : aoes_) {
        if (!aoe.active) continue;
        float alpha = std::min(1.0f, (1.0f - aoe.age / aoe.lifetime) * 0.5f);
        uint8_t a = (uint8_t)(alpha * 255);
        UIColor c = {(uint8_t)((aoe.color >> 24) & 0xFF), (uint8_t)((aoe.color >> 16) & 0xFF),
                     (uint8_t)((aoe.color >> 8) & 0xFF), a};
        
        float sx = (aoe.origin.x * 12.0f + 640.0f);
        float sy = (aoe.origin.z * 12.0f + 360.0f);
        float r = aoe.radius * 12.0f;
        ui.DrawRect(sx - r, sy - r, r * 2, r * 2, c);
    }
}

uint32_t ProjectileSystem::Spawn(const glm::vec3& from, const glm::vec3& to,
                                  ProjectileType type, int damage, float speed) {
    Projectile p;
    p.id = next_id_++;
    p.type = type;
    p.position = from;
    p.velocity = to - from;
    p.target_pos = to;
    p.damage = damage;
    p.speed = speed;
    float len = glm::length(p.velocity);
    if (len > 0.001f) p.velocity /= len;
    p.color = (type == ProjectileType::Magic) ? 0x4488ffff : 0xffffaa44;
    p.size = (type == ProjectileType::Magic) ? 6.0f : 3.0f;
    projectiles_.push_back(p);
    return p.id;
}

uint32_t ProjectileSystem::SpawnTargeted(uint32_t source_id, uint32_t target_id,
                                          const glm::vec3& from, const glm::vec3& to,
                                          int damage, float speed) {
    auto id = Spawn(from, to, ProjectileType::Magic, damage, speed);
    for (auto& p : projectiles_) {
        if (p.id == id) {
            p.source_id = source_id;
            p.target_id = target_id;
            break;
        }
    }
    return id;
}

uint32_t ProjectileSystem::CreateAoE(const glm::vec3& origin, AoEShape shape, float radius,
                                      int damage, float lifetime) {
    AoEInstance aoe;
    aoe.id = next_id_++;
    aoe.origin = origin;
    aoe.shape = shape;
    aoe.radius = radius;
    aoe.damage = damage;
    aoe.lifetime = lifetime;
    aoes_.push_back(aoe);
    return aoe.id;
}

uint32_t ProjectileSystem::CreateCone(const glm::vec3& origin, const glm::vec3& direction,
                                       float angle, float length, int damage) {
    AoEInstance aoe;
    aoe.id = next_id_++;
    aoe.origin = origin;
    aoe.direction = direction;
    aoe.shape = AoEShape::Cone;
    aoe.angle = angle;
    aoe.length = length;
    aoe.damage = damage;
    aoes_.push_back(aoe);
    return aoe.id;
}

uint32_t ProjectileSystem::CreateLine(const glm::vec3& origin, const glm::vec3& direction,
                                       float length, float width, int damage) {
    AoEInstance aoe;
    aoe.id = next_id_++;
    aoe.origin = origin;
    aoe.direction = direction;
    aoe.shape = AoEShape::Line;
    aoe.length = length;
    aoe.width = width;
    aoe.damage = damage;
    aoes_.push_back(aoe);
    return aoe.id;
}

bool ProjectileSystem::TestCircle(const glm::vec3& origin, float radius, const glm::vec3& point) const {
    float dx = point.x - origin.x, dz = point.z - origin.z;
    return (dx * dx + dz * dz) <= (radius * radius);
}

bool ProjectileSystem::TestCone(const glm::vec3& origin, const glm::vec3& dir, float angle, float length, const glm::vec3& point) const {
    glm::vec3 to_point = point - origin;
    float dist = glm::length(to_point);
    if (dist > length || dist < 0.1f) return false;
    glm::vec3 norm_dir = glm::normalize(dir);
    glm::vec3 norm_pt = to_point / dist;
    float dot = glm::dot(norm_dir, norm_pt);
    float half_angle = cosf(glm::radians(angle * 0.5f));
    return dot >= half_angle;
}

bool ProjectileSystem::TestLine(const glm::vec3& origin, const glm::vec3& dir, float length, float width, const glm::vec3& point) const {
    glm::vec3 to_point = point - origin;
    float proj = glm::dot(to_point, dir);
    if (proj < 0 || proj > length) return false;
    glm::vec3 perp = to_point - dir * proj;
    float dist = glm::length(perp);
    return dist <= width * 0.5f;
}

void ProjectileSystem::Clear() {
    projectiles_.clear();
    aoes_.clear();
}
