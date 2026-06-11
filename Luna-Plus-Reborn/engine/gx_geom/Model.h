// AGENT Titan — DO NOT MODIFY WITHOUT COORDINATION
#pragma once
#include <string>
#include <vector>
#include <cstdint>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct MeshPart {
    std::string name;
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> uvs;
    std::vector<uint32_t> colors;
    std::vector<uint16_t> indices;
    uint32_t material_index = 0;
    uint32_t bone_count = 0;

    // Skinning data
    std::vector<uint8_t> bone_indices;  // 4 per vertex
    std::vector<float>   bone_weights;  // 4 per vertex
};

struct ModelBone {
    std::string name;
    int parent_index = -1;
    glm::mat4 bind_matrix{1.0f};
    glm::mat4 inverse_bind_matrix{1.0f};
};

class Model {
public:
    Model() = default;
    ~Model() = default;

    bool LoadFromGLB(const std::string& path);
    bool LoadFromOBJ(const std::string& path);

    const std::vector<MeshPart>& GetMeshes() const { return meshes_; }
    const std::vector<ModelBone>& GetBones() const { return bones_; }
    bool HasBones() const { return !bones_.empty(); }

    void ComputeSkinningMatrices(const std::vector<glm::mat4>& bone_poses,
                                 glm::mat4* out_matrices, size_t max_count);

    const std::vector<std::string>& GetTextures() const { return textures_; }
    const std::string& GetPath() const { return path_; }

private:
    bool LoadAssimp(const std::string& path, unsigned int flags);

    std::string path_;
    std::vector<MeshPart> meshes_;
    std::vector<ModelBone> bones_;
    std::vector<std::string> textures_;
};
