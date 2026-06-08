#define GLM_ENABLE_EXPERIMENTAL
#include "CharacterRenderer.hpp"
#include <engine/gx_render/RenderDevice.h>
#include <engine/gx_geom/Model.h>
#include <engine/gx_geom/ModelManager.hpp>
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
#include <engine/gx_render/Shader.h>

namespace {
constexpr int MAX_BONES = 64;

const char* AnimNameForCharAnim(CharAnim anim) {
    switch (anim) {
    case CharAnim::Idle:   return "idle";
    case CharAnim::Walk:   return "walk";
    case CharAnim::Run:    return "run";
    case CharAnim::Attack: return "attack";
    case CharAnim::Die:    return "die";
    default:               return "idle";
    }
}

struct LoadedModel {
    Model model;
    std::vector<MeshObject> meshes;
    Skeleton skeleton;
    std::vector<glm::mat4> bind_pose;
};

struct AnimationState {
    AnimationSystem anim_sys;
    std::unordered_map<std::string, AnimClip> clips;
    CharAnim current_anim = CharAnim::Idle;
    bool needs_transition = false;
};

struct RenderInstance {
    std::string model_key;
    glm::vec3 pos{0};
    float rot = 0;
    uint32_t color = 0xffffffff;
    CharAnim anim = CharAnim::Idle;
    bool moving = false;
    float anim_time = 0;

    struct Attachment {
        std::string model_key;
        std::string bone_name;
        int bone_index = -1;
    };
    std::vector<Attachment> attachments;
};
} // anonymous namespace

struct CharacterRenderer::Impl {
    uint16_t fb_width = 1280;
    uint16_t fb_height = 720;
    float frame_dt = 1.0f / 60.0f;

    std::unordered_map<std::string, LoadedModel> models;
    std::unordered_map<std::string, AnimationState> anim_states;
    std::unordered_map<uint32_t, RenderInstance> instances;

    bgfx::ProgramHandle prog = BGFX_INVALID_HANDLE;
    bgfx::UniformHandle tex = BGFX_INVALID_HANDLE;
    bgfx::UniformHandle bones_uniform = BGFX_INVALID_HANDLE;
    bgfx::UniformHandle light_dir = BGFX_INVALID_HANDLE;
    bgfx::UniformHandle u_color = BGFX_INVALID_HANDLE;
    bgfx::TextureHandle white = BGFX_INVALID_HANDLE;

    ModelManager model_mgr;
    bool initialized = false;
};

CharacterRenderer::CharacterRenderer()
    : impl_(std::make_unique<Impl>()) {}

CharacterRenderer::~CharacterRenderer() {
    Shutdown();
}

bool CharacterRenderer::Init(const std::string& shader_dir) {
    auto& i = *impl_;
    std::string vs = shader_dir + "vs_skinned.bin";
    std::string fs = shader_dir + "fs_lit.bin";

    i.prog = ShaderUtils::LoadProgram(vs, fs);
    i.tex = bgfx::createUniform("s_texColor", bgfx::UniformType::Sampler);
    i.bones_uniform = bgfx::createUniform("u_bones", bgfx::UniformType::Mat4, MAX_BONES);
    i.light_dir = bgfx::createUniform("u_lightDir", bgfx::UniformType::Vec4);
    i.u_color = bgfx::createUniform("u_color", bgfx::UniformType::Vec4);
    uint32_t white = 0xffffffff;
    i.white = bgfx::createTexture2D(1, 1, false, 1, bgfx::TextureFormat::RGBA8, 0, bgfx::makeRef(&white, 4));

    i.initialized = true;
    return true;
}

void CharacterRenderer::SetFramebufferSize(uint16_t w, uint16_t h) {
    impl_->fb_width = w;
    impl_->fb_height = h;
}

void CharacterRenderer::SetFrameDelta(float dt) {
    if (dt > 0.0f && dt < 0.5f) impl_->frame_dt = dt;
}

uint32_t CharacterRenderer::LoadModel(const std::string& path) {
    auto& i = *impl_;
    std::string resolved = path;

    if (!i.models.count(resolved)) {
        std::string found = VFS::Find(path);
        if (!found.empty()) resolved = found;
    }
    if (i.models.count(resolved)) return 1;

    Model* raw = i.model_mgr.Load(resolved, resolved);
    if (!raw) {
        spdlog::error("CharRenderer: failed to load model {}", path);
        return 0;
    }

    LoadedModel lm;
    lm.model = *raw;

    const auto& parts = lm.model.GetMeshes();
    lm.meshes.resize(parts.size());
    for (size_t mi = 0; mi < parts.size(); mi++) {
        if (!lm.meshes[mi].UploadToGPU(parts[mi])) {
            spdlog::error("CharRenderer: failed to upload mesh '{}' to GPU", parts[mi].name);
            return 0;
        }
    }

    if (lm.model.HasBones()) {
        lm.skeleton.BuildFromModel(lm.model);
        const auto& inv_bind = lm.skeleton.GetInverseBindMatrices();
        lm.bind_pose.resize(inv_bind.size());
        for (size_t bi = 0; bi < inv_bind.size(); bi++) {
            lm.bind_pose[bi] = glm::inverse(inv_bind[bi]);
        }
    }

    i.models[resolved] = std::move(lm);

    AnimationState as;
    std::string base_path = resolved.substr(0, resolved.find_last_of('.'));
    const char* clip_names[] = {"idle", "walk", "run", "attack", "die"};
    for (auto* name : clip_names) {
        std::string json_path = VFS::Find(base_path + "_" + name + ".anm.json");
        if (json_path.empty()) json_path = base_path + "_" + name + ".anm.json";
        AnimClip clip;
        if (as.anim_sys.LoadFromJson(json_path, clip)) {
            clip.name = name;
            clip.loop = (std::string(name) != "die");
            as.clips[name] = std::move(clip);
            spdlog::debug("CharRenderer: loaded anim clip '{}' from {}", name, json_path);
        } else {
            std::string anm_path = VFS::Find(base_path + "_" + name + ".anm");
            if (anm_path.empty()) anm_path = base_path + "_" + name + ".anm";
            if (as.anim_sys.LoadFromAnm(anm_path, clip)) {
                clip.name = name;
                clip.loop = (std::string(name) != "die");
                as.clips[name] = std::move(clip);
                spdlog::debug("CharRenderer: loaded anim clip '{}' from {}", name, anm_path);
            }
        }
    }

    auto it = as.clips.find("idle");
    if (it != as.clips.end()) {
        as.anim_sys.Play(&it->second, true, 0.0f);
    }
    as.current_anim = CharAnim::Idle;
    i.anim_states[resolved] = std::move(as);

    return 1;
}

uint32_t CharacterRenderer::Spawn(uint32_t id, const std::string& model,
                                   float x, float y, float z, uint32_t color) {
    LoadModel(model);
    RenderInstance inst;
    std::string found = VFS::Find(model);
    inst.model_key = found.empty() ? model : found;
    inst.pos = {x, y, z};
    inst.color = color;
    impl_->instances[id] = inst;
    return id;
}

void CharacterRenderer::Remove(uint32_t id) {
    impl_->instances.erase(id);
}

void CharacterRenderer::Move(uint32_t id, float x, float y, float z, bool moving, CharAnim anim) {
    auto& i = *impl_;
    auto it = i.instances.find(id);
    if (it == i.instances.end()) return;
    auto& inst = it->second;
    inst.pos = {x, y, z};
    inst.moving = moving;

    auto anim_it = i.anim_states.find(inst.model_key);
    if (anim_it == i.anim_states.end()) return;
    auto& as = anim_it->second;

    CharAnim desired = anim;
    if (desired == CharAnim::Idle && moving) desired = CharAnim::Walk;

    if (desired != as.current_anim) {
        auto clip_it = as.clips.find(AnimNameForCharAnim(desired));
        if (clip_it != as.clips.end()) {
            as.anim_sys.BlendTo(&clip_it->second, 0.2f);
            as.current_anim = desired;
        }
    }
}

void CharacterRenderer::Attach(uint32_t id, const std::string& model, const std::string& bone) {
    auto& i = *impl_;
    auto it = i.instances.find(id);
    if (it == i.instances.end()) return;
    LoadModel(model);
    RenderInstance::Attachment att;
    att.model_key = model;
    att.bone_name = bone;

    auto mit = i.models.find(it->second.model_key);
    if (mit != i.models.end()) {
        att.bone_index = mit->second.skeleton.GetBoneIndex(bone);
    }
    it->second.attachments.push_back(att);
}

void CharacterRenderer::Detach(uint32_t id, const std::string& bone) {
    auto& i = *impl_;
    auto it = i.instances.find(id);
    if (it == i.instances.end()) return;
    auto& atts = it->second.attachments;
    atts.erase(std::remove_if(atts.begin(), atts.end(),
        [&bone](const RenderInstance::Attachment& a) { return a.bone_name == bone; }), atts.end());
}

void CharacterRenderer::Render(const glm::mat4& view, const glm::mat4& proj, float time, const EnvData& env) {
    (void)time;
    auto& i = *impl_;
    if (!bgfx::isValid(i.prog)) return;
    bgfx::ViewId vid = (bgfx::ViewId)ViewId::Character;
    bgfx::setViewTransform(vid, &view, &proj);
    bgfx::setViewClear(vid, BGFX_CLEAR_NONE, 0, 1.0f, 0);
    bgfx::setViewRect(vid, 0, 0, i.fb_width, i.fb_height);

    float white[4] = {1, 1, 1, 1};
    bgfx::setUniform(i.light_dir, glm::value_ptr(env.light_dir));
    bgfx::setUniform(i.u_color, white);

    for (auto& [id_, inst] : i.instances) {
        (void)id_;
        auto mit = i.models.find(inst.model_key);
        if (mit == i.models.end()) continue;
        auto& lm = mit->second;

        auto ait = i.anim_states.find(inst.model_key);
        AnimationState* as = (ait != i.anim_states.end()) ? &ait->second : nullptr;

        if (as) {
            as->anim_sys.Update(i.frame_dt);
        }

        glm::mat4 bone_matrices[MAX_BONES];
        std::fill_n(bone_matrices, MAX_BONES, glm::mat4(1.0f));

        if (as && lm.model.HasBones()) {
            std::vector<glm::mat4> blended_pose(lm.bind_pose.size(), glm::mat4(1.0f));
            as->anim_sys.GetBlendedPose(lm.bind_pose, blended_pose.data(), blended_pose.size());

            std::vector<glm::mat4> world_pose;
            lm.skeleton.ComputeFinalPose(blended_pose, world_pose);

            const auto& inv_bind = lm.skeleton.GetInverseBindMatrices();
            size_t count = std::min(world_pose.size(), (size_t)MAX_BONES);
            for (size_t bi = 0; bi < count; bi++) {
                bone_matrices[bi] = world_pose[bi] * inv_bind[bi];
            }
        }

        bgfx::setUniform(i.bones_uniform, bone_matrices, MAX_BONES);

        float mtx[16] = {1,0,0,0, 0,1,0,0, 0,0,1,0, inst.pos.x, inst.pos.y, inst.pos.z, 1};
        bgfx::setTransform(mtx);

        for (auto& mesh_obj : lm.meshes) {
            mesh_obj.Render(0);
            bgfx::setState(BGFX_STATE_DEFAULT | BGFX_STATE_WRITE_Z);
            bgfx::setTexture(0, i.tex, i.white);
            bgfx::submit(vid, i.prog);
        }

        for (auto& att : inst.attachments) {
            auto amit = i.models.find(att.model_key);
            if (amit == i.models.end()) continue;
            auto& alm = amit->second;

            if (att.bone_index >= 0 && att.bone_index < MAX_BONES) {
                glm::mat4 att_mtx = glm::make_mat4(mtx) * bone_matrices[att.bone_index];
                bgfx::setTransform(glm::value_ptr(att_mtx));
            }

            for (auto& att_mesh : alm.meshes) {
                att_mesh.Render(0);
                bgfx::setState(BGFX_STATE_DEFAULT | BGFX_STATE_WRITE_Z);
                bgfx::setTexture(0, i.tex, i.white);
                bgfx::submit(vid, i.prog);
            }
        }
    }
}

void CharacterRenderer::Shutdown() {
    if (!impl_->initialized) return;
    impl_->initialized = false;

    impl_->models.clear();
    impl_->anim_states.clear();
    if (bgfx::isValid(impl_->prog)) bgfx::destroy(impl_->prog);
    if (bgfx::isValid(impl_->tex)) bgfx::destroy(impl_->tex);
    if (bgfx::isValid(impl_->bones_uniform)) bgfx::destroy(impl_->bones_uniform);
    if (bgfx::isValid(impl_->light_dir)) bgfx::destroy(impl_->light_dir);
    if (bgfx::isValid(impl_->u_color)) bgfx::destroy(impl_->u_color);
    if (bgfx::isValid(impl_->white)) bgfx::destroy(impl_->white);

    impl_->prog = BGFX_INVALID_HANDLE;
    impl_->tex = BGFX_INVALID_HANDLE;
    impl_->bones_uniform = BGFX_INVALID_HANDLE;
    impl_->light_dir = BGFX_INVALID_HANDLE;
    impl_->u_color = BGFX_INVALID_HANDLE;
    impl_->white = BGFX_INVALID_HANDLE;
}
