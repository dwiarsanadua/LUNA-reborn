// AGENT Titan — DO NOT MODIFY WITHOUT COORDINATION
#pragma once
#include <glm/glm.hpp>
#include <functional>
#include <vector>
#include <cstdint>
#include <cstddef>

struct CollisionResult {
    float meet_time = 1.0f;
    glm::vec3 meet_pivot{0.0f};
    glm::vec3 where_meet{0.0f};
    glm::vec3 meet_plane_normal{0.0f};
    float meet_plane_d = 0.0f;
    uint32_t component_type = 0xffffffff;
};

struct MovingSphere {
    glm::vec3 from{0.0f};
    float radius = 1.0f;
    glm::vec3 velocity{0.0f};
};

struct MovingEllipsoid {
    glm::vec3 from{0.0f};
    float width = 1.0f;
    float height = 1.0f;
    glm::vec3 velocity{0.0f};
};

class PhysicsWorld {
public:
    PhysicsWorld() = default;
    ~PhysicsWorld() { Shutdown(); }

    PhysicsWorld(const PhysicsWorld&) = delete;
    PhysicsWorld& operator=(const PhysicsWorld&) = delete;

    bool Initialize();
    void Shutdown();
    void Update(float delta_time);

    int  CreateCharacter(glm::vec3 position, float radius, float height);
    void RemoveCharacter(int id);
    void SetCharacterPosition(int id, glm::vec3 pos);
    glm::vec3 GetCharacterPosition(int id) const;

    bool RayCast(glm::vec3 from, glm::vec3 to, glm::vec3& out_hit) const;
    float GetTerrainHeight(float x, float z) const;

    void SetTerrainHeightFunction(std::function<float(float, float)> func) {
        terrain_height_func_ = std::move(func);
    }

    bool CollisionTestMovingSphereMeetTriangle(
        uint32_t& out_component_type,
        glm::vec3& out_where_meet,
        float& out_t,
        const MovingSphere& sphere,
        const glm::vec3 triangle[3]);

    bool AAEllipsoidCollisionTest(
        CollisionResult& out_result,
        const MovingEllipsoid& ellipsoid,
        const glm::vec3 triangle[3]);

    bool CollisionTestMovingEllipsoidMeetTriangle(
        CollisionResult& out_result,
        const MovingEllipsoid& ellipsoid,
        const glm::vec3 triangle[3]);

private:
    struct Character {
        glm::vec3 position{0.0f};
        float radius = 0.4f;
        float height = 1.8f;
        bool active = false;
    };

    static bool TestMovingSphereMeetPlane(
        glm::vec3& out_where_meet,
        float& out_t,
        const MovingSphere& sphere,
        const glm::vec3 triangle[3]);

    static bool TestMovingSphereMeetTriEdge(
        glm::vec3& out_where_meet,
        float& out_t,
        const MovingSphere& sphere,
        const glm::vec3 triangle[3]);

    static bool TestMovingSphereMeetLine(
        glm::vec3& out_where_meet,
        float& out_t,
        const MovingSphere& sphere,
        const glm::vec3& line_from,
        const glm::vec3& line_to);

    static bool TestMovingSphereMeetVertex(
        glm::vec3& out_where_meet,
        float& out_t,
        const MovingSphere& sphere,
        const glm::vec3& vertex);

    static bool CalculatePlaneOnEllipsoidWithVertex(
        glm::vec3& out_normal,
        float& out_d,
        const glm::vec3& ellipsoid_center,
        float width,
        float height,
        const glm::vec3& vertex);

    std::vector<Character> characters_;
    std::function<float(float, float)> terrain_height_func_;
    bool initialized_ = false;
};
