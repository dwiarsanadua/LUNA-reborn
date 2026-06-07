#define GLM_ENABLE_EXPERIMENTAL
#include "CharacterRenderer.hpp"
#include "AnmParser.h"
#include <engine/gx_render/RenderDevice.h>
#include <bgfx/bgfx.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <spdlog/spdlog.h>
#include <unordered_map>
#include <vector>
#include <fstream>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

static constexpr int MAX_BONES = 64;
static uint16_t g_fb_width = 1280;
static uint16_t g_fb_height = 720;

struct SkinnedVertex {
    float x, y, z;
    float nx, ny, nz;
    uint32_t color;
    float u, v;
    uint8_t bone_indices[4] = {0,0,0,0};
    float bone_weights[4] = {0,0,0,0};
};

struct BoneInfo {
    std::string name;
    int parent = -1;
    glm::mat4 bind_matrix{1.0f};
};

struct LoadedModel {
    bgfx::VertexBufferHandle vb = BGFX_INVALID_HANDLE;
    bgfx::IndexBufferHandle ib = BGFX_INVALID_HANDLE;
    bgfx::TextureHandle tex = BGFX_INVALID_HANDLE;
    std::vector<BoneInfo> bones;
};

struct RenderInstance {
    std::string model_key;
    glm::vec3 pos{0};
    float rot = 0;
    uint32_t color = 0xffffffff;
    CharAnim anim = CHAR_IDLE;
    bool moving = false;
    float anim_time = 0;

    struct Attachment {
        std::string model_key;
        std::string bone_name;
        int bone_index = -1;
    };
    std::vector<Attachment> attachments;
};

static std::unordered_map<std::string, LoadedModel> g_models;
static std::unordered_map<uint32_t, RenderInstance> g_instances;
static bgfx::ProgramHandle g_prog = BGFX_INVALID_HANDLE;
static bgfx::UniformHandle g_tex = BGFX_INVALID_HANDLE;
static bgfx::UniformHandle g_bones_uniform = BGFX_INVALID_HANDLE;
static bgfx::UniformHandle g_light_dir = BGFX_INVALID_HANDLE;
static bgfx::UniformHandle g_u_color = BGFX_INVALID_HANDLE;
static bgfx::TextureHandle g_white = BGFX_INVALID_HANDLE;
static bgfx::VertexLayout s_layout;
static bool s_layout_init = false;

static bgfx::VertexLayout getSkinnedLayout() {
    if (!s_layout_init) {
        s_layout.begin()
            .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
            .add(bgfx::Attrib::Normal,   3, bgfx::AttribType::Float)
            .add(bgfx::Attrib::Color0,   4, bgfx::AttribType::Uint8, true)
            .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
            .add(bgfx::Attrib::Indices,  4, bgfx::AttribType::Uint8)
            .add(bgfx::Attrib::Weight,   4, bgfx::AttribType::Float)
            .end();
        s_layout_init = true;
    }
    return s_layout;
}

static const bgfx::Memory* loadShader(const char* path) {
    std::string search[] = { "build/bin/" + std::string(path), std::string(path), "assets/" + std::string(path) };
    for (auto& p : search) {
        std::ifstream file(p, std::ios::binary | std::ios::ate);
        if (file) {
            size_t size = file.tellg();
            file.seekg(0);
            auto* mem = bgfx::alloc(static_cast<uint32_t>(size));
            file.read(reinterpret_cast<char*>(mem->data), size);
            spdlog::info("Shader: loaded {} ({} bytes)", p, size);
            return mem;
        }
    }
    return nullptr;
}

void CharRenderer_Init() {
    auto vs = loadShader("shaders/vs_skinned.bin");
    auto fs = loadShader("shaders/fs_lit.bin");
    if (vs && fs) g_prog = bgfx::createProgram(bgfx::createShader(vs), bgfx::createShader(fs), true);
    g_tex = bgfx::createUniform("s_texColor", bgfx::UniformType::Sampler);
    g_bones_uniform = bgfx::createUniform("u_bones", bgfx::UniformType::Mat4, MAX_BONES);
    g_light_dir = bgfx::createUniform("u_lightDir", bgfx::UniformType::Vec4);
    g_u_color = bgfx::createUniform("u_color", bgfx::UniformType::Vec4);
    uint32_t white = 0xffffffff;
    g_white = bgfx::createTexture2D(1, 1, false, 1, bgfx::TextureFormat::RGBA8, 0, bgfx::makeRef(&white, 4));
}

void CharRenderer_SetFBSize(uint16_t w, uint16_t h) {
    g_fb_width = w; g_fb_height = h;
}

uint32_t CharRenderer_LoadModel(const std::string& path) {
    if (g_models.count(path)) return 1;
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_LimitBoneWeights);
    if (!scene || !scene->mNumMeshes) return 0;
    
    auto* mesh = scene->mMeshes[0];
    LoadedModel result;
    std::vector<SkinnedVertex> verts(mesh->mNumVertices);
    for (uint32_t i = 0; i < mesh->mNumVertices; i++) {
        verts[i].x = mesh->mVertices[i].x * 0.03f;
        verts[i].y = mesh->mVertices[i].y * 0.03f;
        verts[i].z = mesh->mVertices[i].z * 0.03f;
        if (mesh->mNormals) { verts[i].nx = mesh->mNormals[i].x; verts[i].ny = mesh->mNormals[i].y; verts[i].nz = mesh->mNormals[i].z; }
        verts[i].color = 0xffffffff;
        verts[i].u = mesh->mTextureCoords[0] ? mesh->mTextureCoords[0][i].x : 0;
        verts[i].v = mesh->mTextureCoords[0] ? mesh->mTextureCoords[0][i].y : 0;
    }

    if (mesh->HasBones()) {
        for (uint32_t b = 0; b < mesh->mNumBones; b++) {
            for (uint32_t w = 0; w < mesh->mBones[b]->mNumWeights; w++) {
                uint32_t vid = mesh->mBones[b]->mWeights[w].mVertexId;
                float weight = mesh->mBones[b]->mWeights[w].mWeight;
                for (int k = 0; k < 4; k++) {
                    if (verts[vid].bone_weights[k] == 0) {
                        verts[vid].bone_indices[k] = (uint8_t)b;
                        verts[vid].bone_weights[k] = weight;
                        break;
                    }
                }
            }
        }
    }

    std::vector<uint16_t> indices;
    for (uint32_t i = 0; i < mesh->mNumFaces; i++) {
        indices.push_back((uint16_t)mesh->mFaces[i].mIndices[0]);
        indices.push_back((uint16_t)mesh->mFaces[i].mIndices[1]);
        indices.push_back((uint16_t)mesh->mFaces[i].mIndices[2]);
    }

    result.vb = bgfx::createVertexBuffer(bgfx::copy(verts.data(), (uint32_t)(verts.size()*sizeof(SkinnedVertex))), getSkinnedLayout());
    result.ib = bgfx::createIndexBuffer(bgfx::copy(indices.data(), (uint32_t)(indices.size()*sizeof(uint16_t))));
    g_models[path] = result;
    return 1;
}

void CharRenderer_Spawn(uint32_t id, const std::string& model, float x, float y, float z, uint32_t color) {
    CharRenderer_LoadModel(model);
    RenderInstance inst;
    inst.model_key = model;
    inst.pos = {x, y, z};
    inst.color = color;
    g_instances[id] = inst;
}

void CharRenderer_Remove(uint32_t id) { g_instances.erase(id); }

void CharRenderer_Move(uint32_t id, float x, float y, float z, bool moving, CharAnim anim) {
    if (g_instances.count(id)) {
        auto& inst = g_instances[id];
        inst.pos = {x, y, z};
        inst.moving = moving;
        inst.anim = anim;
    }
}

void CharRenderer_Attach(uint32_t id, const std::string& model, const std::string& bone) {
    if (g_instances.count(id)) {
        CharRenderer_LoadModel(model);
        RenderInstance::Attachment att;
        att.model_key = model;
        att.bone_name = bone;
        g_instances[id].attachments.push_back(att);
    }
}

void CharRenderer_Render(const glm::mat4& view, const glm::mat4& proj, float time, const EnvData& env) {
    if (!bgfx::isValid(g_prog)) return;
    bgfx::ViewId vid = (bgfx::ViewId)ViewId::Character;
    bgfx::setViewTransform(vid, &view, &proj);
    bgfx::setViewClear(vid, BGFX_CLEAR_NONE, 0, 1.0f, 0);
    bgfx::setViewRect(vid, 0, 0, g_fb_width, g_fb_height);

    float white[4] = {1, 1, 1, 1};
    bgfx::setUniform(g_light_dir, glm::value_ptr(env.light_dir));
    bgfx::setUniform(g_u_color, white);

    for (auto& [id, inst] : g_instances) {
        auto it = g_models.find(inst.model_key);
        if (it == g_models.end()) continue;
        auto& model = it->second;

        float mtx[16] = {1,0,0,0, 0,1,0,0, 0,0,1,0, inst.pos.x, inst.pos.y, inst.pos.z, 1};
        bgfx::setTransform(mtx);
        bgfx::setVertexBuffer(0, model.vb);
        bgfx::setIndexBuffer(model.ib);
        bgfx::setState(BGFX_STATE_DEFAULT | BGFX_STATE_WRITE_Z);
        bgfx::submit(vid, g_prog);

        for (auto& att : inst.attachments) {
            auto ait = g_models.find(att.model_key);
            if (ait == g_models.end()) continue;
            auto& amodel = ait->second;
            bgfx::setTransform(mtx);
            bgfx::setVertexBuffer(0, amodel.vb);
            bgfx::setIndexBuffer(amodel.ib);
            bgfx::setState(BGFX_STATE_DEFAULT | BGFX_STATE_WRITE_Z);
            bgfx::submit(vid, g_prog);
        }
    }
}

void CharRenderer_Render(const glm::mat4& view, const glm::mat4& proj, float time) {
    EnvData env; CharRenderer_Render(view, proj, time, env);
}

void CharRenderer_Shutdown() {
    static bool is_shutdown = false;
    if (is_shutdown) return;
    is_shutdown = true;
    for (auto& [path, model] : g_models) {
        if (bgfx::isValid(model.vb)) bgfx::destroy(model.vb);
        if (bgfx::isValid(model.ib)) bgfx::destroy(model.ib);
    }
    g_models.clear();
    if (bgfx::isValid(g_prog)) bgfx::destroy(g_prog);
    if (bgfx::isValid(g_tex)) bgfx::destroy(g_tex);
    if (bgfx::isValid(g_bones_uniform)) bgfx::destroy(g_bones_uniform);
    if (bgfx::isValid(g_light_dir)) bgfx::destroy(g_light_dir);
    if (bgfx::isValid(g_u_color)) bgfx::destroy(g_u_color);
    if (bgfx::isValid(g_white)) bgfx::destroy(g_white);
}

void CharRenderer_Detach(uint32_t id, const std::string& bone) {}
