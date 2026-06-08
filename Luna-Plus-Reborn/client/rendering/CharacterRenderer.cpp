#define GLM_ENABLE_EXPERIMENTAL
#include "CharacterRenderer.hpp"
#include <engine/gx_render/RenderDevice.h>
#include <engine/gx_geom/Model.h>
#include <engine/gx_geom/MeshObject.h>
#include <engine/gx_geom/Skeleton.h>
#include <engine/gx_geom/AnimationSystem.h>
#include <bgfx/bgfx.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <spdlog/spdlog.h>
#include <unordered_map>
#include <vector>
#include <fstream>
#include <algorithm>
#include <engine/gx_render/VFS.h>

static constexpr int MAX_BONES = 64;
static uint16_t g_fb_width = 1280;
static uint16_t g_fb_height = 720;

// Map CharAnim to animation clip name
static const char* AnimNameForCharAnim(CharAnim anim) {
    switch (anim) {
    case CHAR_IDLE:   return "idle";
    case CHAR_WALK:   return "walk";
    case CHAR_RUN:    return "run";
    case CHAR_ATTACK: return "attack";
    case CHAR_DIE:    return "die";
    default:          return "idle";
    }
}

struct LoadedModel {
    Model model;
    std::vector<MeshObject> meshes;
    Skeleton skeleton;
    std::vector<glm::mat4> bind_pose; // local-space bind pose
};

struct AnimationState {
    AnimationSystem anim_sys;
    std::unordered_map<std::string, AnimClip> clips;
    CharAnim current_anim = CHAR_IDLE;
    CharAnim target_anim = CHAR_IDLE;
    bool needs_transition = false;
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
static std::unordered_map<std::string, AnimationState> g_anim_states;
static std::unordered_map<uint32_t, RenderInstance> g_instances;
static bgfx::ProgramHandle g_prog = BGFX_INVALID_HANDLE;
static bgfx::UniformHandle g_tex = BGFX_INVALID_HANDLE;
static bgfx::UniformHandle g_bones_uniform = BGFX_INVALID_HANDLE;
static bgfx::UniformHandle g_light_dir = BGFX_INVALID_HANDLE;
static bgfx::UniformHandle g_u_color = BGFX_INVALID_HANDLE;
static bgfx::TextureHandle g_white = BGFX_INVALID_HANDLE;

static const bgfx::Memory* loadShader(const char* path) {
    std::string search[] = { "build/bin/" + std::string(path), std::string(path), VFS::Resolve("assets/" + std::string(path)) };
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

    LoadedModel lm;
    if (!lm.model.LoadFromGLB(path)) {
        spdlog::error("CharRenderer: failed to load model {}", path);
        return 0;
    }

    // Create MeshObject for each mesh part
    const auto& parts = lm.model.GetMeshes();
    lm.meshes.resize(parts.size());
    for (size_t i = 0; i < parts.size(); i++) {
        if (!lm.meshes[i].UploadToGPU(parts[i])) {
            spdlog::error("CharRenderer: failed to upload mesh '{}' to GPU", parts[i].name);
            return 0;
        }
    }

    // Build skeleton from model bones
    if (lm.model.HasBones()) {
        lm.skeleton.BuildFromModel(lm.model);

        // Build local-space bind pose from inverse bind matrices
        const auto& inv_bind = lm.skeleton.GetInverseBindMatrices();
        lm.bind_pose.resize(inv_bind.size());
        for (size_t i = 0; i < inv_bind.size(); i++) {
            lm.bind_pose[i] = glm::inverse(inv_bind[i]);
        }
    }

    g_models[path] = std::move(lm);

    // Auto-load animation clips
    AnimationState as;
    std::string base_path = path.substr(0, path.find_last_of('.'));
    const char* clip_names[] = {"idle", "walk", "run", "attack", "die"};
    for (auto* name : clip_names) {
        std::string json_path = base_path + "_" + name + ".anm.json";
        AnimClip clip;
        if (as.anim_sys.LoadFromJson(json_path, clip)) {
            clip.name = name;
            clip.loop = (std::string(name) != "die");
            as.clips[name] = std::move(clip);
            spdlog::debug("CharRenderer: loaded anim clip '{}' from {}", name, json_path);
        } else {
            // Fallback to .anm binary
            std::string anm_path = base_path + "_" + name + ".anm";
            if (as.anim_sys.LoadFromAnm(anm_path, clip)) {
                clip.name = name;
                clip.loop = (std::string(name) != "die");
                as.clips[name] = std::move(clip);
                spdlog::debug("CharRenderer: loaded anim clip '{}' from {}", name, anm_path);
            }
        }
    }

    // Start with idle
    auto it = as.clips.find("idle");
    if (it != as.clips.end()) {
        as.anim_sys.Play(&it->second, true, 0.0f);
    }
    as.current_anim = CHAR_IDLE;
    g_anim_states[path] = std::move(as);

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
    auto it = g_instances.find(id);
    if (it == g_instances.end()) return;
    auto& inst = it->second;
    inst.pos = {x, y, z};
    inst.moving = moving;

    // Handle animation state transitions
    auto anim_it = g_anim_states.find(inst.model_key);
    if (anim_it == g_anim_states.end()) return;
    auto& as = anim_it->second;

    CharAnim desired = anim;
    if (desired == CHAR_IDLE && moving) desired = CHAR_WALK;

    if (desired != as.current_anim) {
        auto clip_it = as.clips.find(AnimNameForCharAnim(desired));
        if (clip_it != as.clips.end()) {
            as.anim_sys.BlendTo(&clip_it->second, 0.2f);
            as.current_anim = desired;
        }
    }
}

void CharRenderer_Attach(uint32_t id, const std::string& model, const std::string& bone) {
    auto it = g_instances.find(id);
    if (it == g_instances.end()) return;
    CharRenderer_LoadModel(model);
    RenderInstance::Attachment att;
    att.model_key = model;
    att.bone_name = bone;

    // Resolve bone index
    auto mit = g_models.find(it->second.model_key);
    if (mit != g_models.end()) {
        att.bone_index = mit->second.skeleton.GetBoneIndex(bone);
    }
    it->second.attachments.push_back(att);
}

void CharRenderer_Detach(uint32_t id, const std::string& bone) {
    auto it = g_instances.find(id);
    if (it == g_instances.end()) return;
    auto& atts = it->second.attachments;
    atts.erase(std::remove_if(atts.begin(), atts.end(),
        [&bone](const RenderInstance::Attachment& a) { return a.bone_name == bone; }), atts.end());
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
        auto mit = g_models.find(inst.model_key);
        if (mit == g_models.end()) continue;
        auto& lm = mit->second;

        auto ait = g_anim_states.find(inst.model_key);
        AnimationState* as = (ait != g_anim_states.end()) ? &ait->second : nullptr;

        // Update animation
        if (as) {
            as->anim_sys.Update(0.016f);
        }

        // Compute skinning matrices
        glm::mat4 bone_matrices[MAX_BONES];
        std::fill_n(bone_matrices, MAX_BONES, glm::mat4(1.0f));

        if (as && lm.model.HasBones()) {
            // Get blended pose from animation system
            std::vector<glm::mat4> blended_pose(lm.bind_pose.size(), glm::mat4(1.0f));
            as->anim_sys.GetBlendedPose(lm.bind_pose, blended_pose.data(), blended_pose.size());

            // Compute world-space final pose
            std::vector<glm::mat4> world_pose;
            lm.skeleton.ComputeFinalPose(blended_pose, world_pose);

            // Convert to skinning matrices: pose * inverse_bind
            const auto& inv_bind = lm.skeleton.GetInverseBindMatrices();
            size_t count = std::min(world_pose.size(), (size_t)MAX_BONES);
            for (size_t i = 0; i < count; i++) {
                bone_matrices[i] = world_pose[i] * inv_bind[i];
            }
        }

        bgfx::setUniform(g_bones_uniform, bone_matrices, MAX_BONES);

        // Render all meshes
        float mtx[16] = {1,0,0,0, 0,1,0,0, 0,0,1,0, inst.pos.x, inst.pos.y, inst.pos.z, 1};
        bgfx::setTransform(mtx);

        for (auto& mesh_obj : lm.meshes) {
            mesh_obj.Render(0);
            bgfx::setState(BGFX_STATE_DEFAULT | BGFX_STATE_WRITE_Z);
            bgfx::setTexture(0, g_tex, g_white);
            bgfx::submit(vid, g_prog);
        }

        // Render attachments
        for (auto& att : inst.attachments) {
            auto amit = g_models.find(att.model_key);
            if (amit == g_models.end()) continue;
            auto& alm = amit->second;

            // Apply attachment bone transform if index is valid
            if (att.bone_index >= 0 && att.bone_index < MAX_BONES) {
                glm::mat4 att_mtx = glm::make_mat4(mtx) * bone_matrices[att.bone_index];
                bgfx::setTransform(glm::value_ptr(att_mtx));
            }

            for (auto& att_mesh : alm.meshes) {
                att_mesh.Render(0);
                bgfx::setState(BGFX_STATE_DEFAULT | BGFX_STATE_WRITE_Z);
                bgfx::setTexture(0, g_tex, g_white);
                bgfx::submit(vid, g_prog);
            }
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
    g_models.clear();
    g_anim_states.clear();
    if (bgfx::isValid(g_prog)) bgfx::destroy(g_prog);
    if (bgfx::isValid(g_tex)) bgfx::destroy(g_tex);
    if (bgfx::isValid(g_bones_uniform)) bgfx::destroy(g_bones_uniform);
    if (bgfx::isValid(g_light_dir)) bgfx::destroy(g_light_dir);
    if (bgfx::isValid(g_u_color)) bgfx::destroy(g_u_color);
    if (bgfx::isValid(g_white)) bgfx::destroy(g_white);
}
