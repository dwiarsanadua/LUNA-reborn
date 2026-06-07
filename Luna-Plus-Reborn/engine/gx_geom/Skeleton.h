// AGENT Titan — DO NOT MODIFY WITHOUT COORDINATION
#pragma once
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Model;

class Skeleton {
public:
    Skeleton() = default;
    ~Skeleton() = default;

    void BuildFromModel(const Model& model);
    void ComputeFinalPose(const std::vector<glm::mat4>& local_pose,
                          std::vector<glm::mat4>& out_world_pose) const;

    int GetBoneIndex(const std::string& name) const;
    size_t GetBoneCount() const { return bones_.size(); }

    const std::vector<glm::mat4>& GetInverseBindMatrices() const {
        return inverse_bind_matrices_;
    }

private:
    struct Bone {
        std::string name;
        int parent_index = -1;
    };
    std::vector<Bone> bones_;
    std::vector<glm::mat4> inverse_bind_matrices_;
};
