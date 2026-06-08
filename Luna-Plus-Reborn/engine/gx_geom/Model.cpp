// AGENT Titan — DO NOT MODIFY WITHOUT COORDINATION
#include "Model.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <spdlog/spdlog.h>
#include <cstring>

bool Model::LoadFromGLB(const std::string& path) {
    return LoadAssimp(path, aiProcess_Triangulate | aiProcess_FlipUVs |
                      aiProcess_LimitBoneWeights | aiProcess_GenNormals |
                      aiProcess_OptimizeMeshes);
}

bool Model::LoadFromOBJ(const std::string& path) {
    return LoadAssimp(path, aiProcess_Triangulate | aiProcess_FlipUVs |
                      aiProcess_GenNormals | aiProcess_OptimizeMeshes);
}

bool Model::LoadAssimp(const std::string& path, unsigned int flags) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, flags);
    if (!scene || !scene->mNumMeshes) {
        spdlog::error("Model::LoadAssimp: failed to load {}", path);
        return false;
    }

    path_ = path;
    meshes_.clear();
    bones_.clear();

    // Extract meshes
    for (unsigned int m = 0; m < scene->mNumMeshes; ++m) {
        aiMesh* mesh = scene->mMeshes[m];
        MeshPart part;
        part.name = mesh->mName.C_Str();
        part.material_index = mesh->mMaterialIndex;

        const uint32_t vc = mesh->mNumVertices;
        part.positions.resize(vc);
        part.normals.resize(vc);
        part.uvs.resize(vc);
        part.colors.resize(vc, 0xffffffff);

        for (uint32_t i = 0; i < vc; ++i) {
            part.positions[i] = glm::vec3(mesh->mVertices[i].x,
                                          mesh->mVertices[i].y,
                                          mesh->mVertices[i].z);
            if (mesh->mNormals) {
                part.normals[i] = glm::vec3(mesh->mNormals[i].x,
                                            mesh->mNormals[i].y,
                                            mesh->mNormals[i].z);
            }
            if (mesh->mTextureCoords[0]) {
                part.uvs[i] = glm::vec2(mesh->mTextureCoords[0][i].x,
                                        mesh->mTextureCoords[0][i].y);
            }
            if (mesh->mColors[0]) {
                uint8_t r = (uint8_t)(mesh->mColors[0][i].r * 255.0f);
                uint8_t g = (uint8_t)(mesh->mColors[0][i].g * 255.0f);
                uint8_t b = (uint8_t)(mesh->mColors[0][i].b * 255.0f);
                uint8_t a = (uint8_t)(mesh->mColors[0][i].a * 255.0f);
                part.colors[i] = (uint32_t)a << 24 | (uint32_t)b << 16 |
                                 (uint32_t)g << 8 | (uint32_t)r;
            }
        }

        // Extract indices
        part.indices.reserve(mesh->mNumFaces * 3);
        for (unsigned int f = 0; f < mesh->mNumFaces; ++f) {
            aiFace& face = mesh->mFaces[f];
            for (unsigned int j = 0; j < face.mNumIndices; ++j) {
                part.indices.push_back((uint16_t)face.mIndices[j]);
            }
        }

        // Extract bone data
        if (mesh->HasBones()) {
            part.bone_count = mesh->mNumBones;
            part.bone_indices.resize(vc * 4, 0);
            part.bone_weights.resize(vc * 4, 0.0f);

            // Build bone map (bone index -> local bone index for this mesh)
            for (uint32_t b = 0; b < mesh->mNumBones; ++b) {
                aiBone* bone = mesh->mBones[b];
                for (uint32_t w = 0; w < bone->mNumWeights; ++w) {
                    uint32_t vid = bone->mWeights[w].mVertexId;
                    float weight = bone->mWeights[w].mWeight;
                    for (int k = 0; k < 4; ++k) {
                        if (part.bone_weights[vid * 4 + k] == 0.0f) {
                            part.bone_indices[vid * 4 + k] = (uint8_t)b;
                            part.bone_weights[vid * 4 + k] = weight;
                            break;
                        }
                    }
                }
            }
        }

        meshes_.push_back(std::move(part));
    }

    // Extract bones
    if (scene->mRootNode) {
        // Collect all bones from the scene
        struct BoneCollector {
            Model* model;
            void Walk(aiNode* node, int parent) {
                int idx = parent;
                // Check if this node name matches any bone from meshes
                for (auto& mesh : model->meshes_) {
                    (void)mesh;
                }
                // Store as bone regardless (for skeleton hierarchy)
                ModelBone bone;
                bone.name = node->mName.C_Str();
                bone.parent_index = parent;
                aiMatrix4x4 m = node->mTransformation;
                glm::mat4 bind(
                    m.a1, m.a2, m.a3, m.a4,
                    m.b1, m.b2, m.b3, m.b4,
                    m.c1, m.c2, m.c3, m.c4,
                    m.d1, m.d2, m.d3, m.d4
                );
                bone.bind_matrix = bind;
                bone.inverse_bind_matrix = glm::inverse(bind);
                idx = (int)model->bones_.size();
                model->bones_.push_back(std::move(bone));

                for (unsigned int c = 0; c < node->mNumChildren; ++c) {
                    Walk(node->mChildren[c], idx);
                }
            }
        };
        BoneCollector bc{this};
        bc.Walk(scene->mRootNode, -1);
    }

    // Fill in inverse bind matrices from actual bone data in meshes
    for (unsigned int mi = 0; mi < scene->mNumMeshes; ++mi) {
        auto* mesh = scene->mMeshes[mi];
        for (uint32_t b = 0; b < mesh->mNumBones; ++b) {
            aiBone* bone = mesh->mBones[b];
            std::string bname = bone->mName.C_Str();
            for (auto& mb : bones_) {
                if (mb.name == bname) {
                    aiMatrix4x4 inv = bone->mOffsetMatrix;
                    mb.inverse_bind_matrix = glm::mat4(
                        inv.a1, inv.a2, inv.a3, inv.a4,
                        inv.b1, inv.b2, inv.b3, inv.b4,
                        inv.c1, inv.c2, inv.c3, inv.c4,
                        inv.d1, inv.d2, inv.d3, inv.d4
                    );
                    break;
                }
            }
        }
    }

    spdlog::info("Model: loaded {} ({} meshes, {} bones)", path,
                 meshes_.size(), bones_.size());
    return true;
}

void Model::ComputeSkinningMatrices(const std::vector<glm::mat4>& bone_poses,
                                    glm::mat4* out_matrices, size_t max_count) {
    size_t count = std::min(bones_.size(), max_count);
    for (size_t i = 0; i < count; ++i) {
        size_t bone_index = i;
        if (bone_index < bones_.size()) {
            out_matrices[i] = bone_poses[i] * bones_[i].inverse_bind_matrix;
        }
    }
}
