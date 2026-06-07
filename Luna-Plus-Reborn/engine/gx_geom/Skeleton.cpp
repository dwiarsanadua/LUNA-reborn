// AGENT Titan — DO NOT MODIFY WITHOUT COORDINATION
#include "Skeleton.h"
#include "Model.h"
#include <spdlog/spdlog.h>

void Skeleton::BuildFromModel(const Model& model) {
    bones_.clear();
    inverse_bind_matrices_.clear();

    const auto& model_bones = model.GetBones();
    bones_.reserve(model_bones.size());
    inverse_bind_matrices_.reserve(model_bones.size());

    for (const auto& mb : model_bones) {
        Bone bone;
        bone.name = mb.name;
        bone.parent_index = mb.parent_index;
        bones_.push_back(bone);
        inverse_bind_matrices_.push_back(mb.inverse_bind_matrix);
    }

    spdlog::info("Skeleton: built from model ({} bones)", bones_.size());
}

void Skeleton::ComputeFinalPose(const std::vector<glm::mat4>& local_pose,
                                std::vector<glm::mat4>& out_world_pose) const {
    size_t n = bones_.size();
    out_world_pose.resize(n);

    for (size_t i = 0; i < n; ++i) {
        glm::mat4 local = (i < local_pose.size()) ? local_pose[i] : glm::mat4(1.0f);

        int parent = bones_[i].parent_index;
        if (parent >= 0 && parent < (int)i) {
            out_world_pose[i] = out_world_pose[parent] * local;
        } else {
            out_world_pose[i] = local;
        }
    }
}

int Skeleton::GetBoneIndex(const std::string& name) const {
    for (size_t i = 0; i < bones_.size(); ++i) {
        if (bones_[i].name == name) return (int)i;
    }
    return -1;
}
