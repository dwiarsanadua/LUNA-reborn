// AGENT Titan — DO NOT MODIFY WITHOUT COORDINATION
#pragma once
#include <glm/glm.hpp>
#include <functional>
#include <vector>
#include <cstdint>

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

private:
    struct Character {
        glm::vec3 position{0.0f};
        float radius = 0.4f;
        float height = 1.8f;
        bool active = false;
    };

    std::vector<Character> characters_;
    std::function<float(float, float)> terrain_height_func_;
    bool initialized_ = false;
};
