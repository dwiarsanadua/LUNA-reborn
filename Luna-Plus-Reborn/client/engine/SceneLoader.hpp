#pragma once
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <bgfx/bgfx.h>

struct SceneObject {
    std::string model_name;
    glm::vec3 position{0};
    glm::vec3 scale{1,1,1};
    glm::quat rotation{1,0,0,0};
    bool is_animated = false;
};

struct SceneData {
    int map_id = 0;
    glm::vec3 box_min{0};
    glm::vec3 box_max{0};
    std::vector<SceneObject> objects;
    std::string heightmap_file;
    std::string static_mesh;
};

class SceneLoader {
public:
    static SceneData Load(const std::string& path);
    static bool Instantiate(const SceneData& scene, class CharacterRenderer* chars);
    static bgfx::VertexBufferHandle LoadModel(const std::string& name);
private:
    static std::string ResolveModelPath(const std::string& model_ref);
};
