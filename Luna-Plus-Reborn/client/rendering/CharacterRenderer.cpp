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
#include <stb_image.h>

#include <audio/AnimationSfxSync.hpp>

namespace {
// ... existing anonymous namespace ...
constexpr int MAX_BONES = 64;
constexpr size_t MAX_PREVIEW_SLOTS = 5;

const char* AnimNameForCharAnim(CharAnim anim) {
    switch (anim) {
    case CharAnim::Idle:       return "idle";
    case CharAnim::Walk:       return "walk";
    case CharAnim::Run:        return "run";
    case CharAnim::Attack:     return "attack";
    case CharAnim::Die:        return "die";
    case CharAnim::Selected:   return "selected";
    case CharAnim::Deselected: return "deselected";
    default:                   return "idle";
    }
}

struct LoadedModel {
    Model model;
    std::vector<MeshObject> meshes;
    Skeleton skeleton;
    std::vector<glm::mat4> bind_pose;
    std::vector<bgfx::TextureHandle> textures;
};

struct SharedAnimationData {
    std::unordered_map<std::string, AnimClip> clips;
};

struct InstanceAnimationState {
    AnimationSystem anim_sys;
    CharAnim current_anim = CharAnim::Idle;
};

struct RenderInstance {
    std::string model_key;
    glm::vec3 pos{0};
    float rot = 0;
    uint32_t color = 0xffffffff;
    CharAnim anim = CharAnim::Idle;
    bool moving = false;
    InstanceAnimationState anim_state;

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
    std::unordered_map<std::string, SharedAnimationData> shared_anims;
    std::unordered_map<uint32_t, RenderInstance> instances;

    bgfx::ProgramHandle prog = BGFX_INVALID_HANDLE;
    bgfx::ProgramHandle preview_prog = BGFX_INVALID_HANDLE;
    bgfx::UniformHandle tex = BGFX_INVALID_HANDLE;
    bgfx::UniformHandle bones_uniform = BGFX_INVALID_HANDLE;
    bgfx::UniformHandle light_dir = BGFX_INVALID_HANDLE;
    bgfx::UniformHandle u_color = BGFX_INVALID_HANDLE;
    bgfx::UniformHandle u_ambient = BGFX_INVALID_HANDLE;
    bgfx::TextureHandle white = BGFX_INVALID_HANDLE;

    ModelManager model_mgr;
    AnimationSfxSync sfx_sync;
    bool initialized = false;

    PreviewSlotConfig preview_slots[MAX_PREVIEW_SLOTS];
    PreviewCameraConfig preview_camera;
    PreviewLightingConfig preview_lighting;
    int selected_slot = -1;
    float select_anim_time = 0.0f;
    bool preview_config_loaded = false;
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
    std::string preview_vs = shader_dir + "vs_skinned_preview.bin";
    std::string preview_fs = shader_dir + "fs_preview.bin";
    i.preview_prog = ShaderUtils::LoadProgram(preview_vs, preview_fs);
    if (!bgfx::isValid(i.preview_prog))
        i.preview_prog = i.prog;

    i.tex = bgfx::createUniform("s_texColor", bgfx::UniformType::Sampler);
    i.bones_uniform = bgfx::createUniform("u_bones", bgfx::UniformType::Mat4, MAX_BONES);
    i.light_dir = bgfx::createUniform("u_lightDir", bgfx::UniformType::Vec4);
    i.u_color = bgfx::createUniform("u_color", bgfx::UniformType::Vec4);
    i.u_ambient = bgfx::createUniform("u_ambient", bgfx::UniformType::Vec4);
    uint32_t white_val = 0xffffffff;
    i.white = bgfx::createTexture2D(1, 1, false, 1, bgfx::TextureFormat::RGBA8, 0, bgfx::copy(&white_val, 4));

    i.sfx_sync.Init();

    for (size_t si = 0; si < MAX_PREVIEW_SLOTS; si++) {

        i.preview_slots[si].position = glm::vec3((static_cast<float>(si) - 2.0f) * 120.0f, 27930.0f, 7834.0f);
        i.preview_slots[si].scale = 0.4f;
    }

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

    // Load textures
    const auto& tex_paths = lm.model.GetTextures();
    lm.textures.resize(tex_paths.size(), i.white);
    for (size_t ti = 0; ti < tex_paths.size(); ti++) {
        std::string tname = tex_paths[ti];
        
        // Resolve texture path with multiple fallbacks
        std::vector<std::string> candidates = {
            "assets_converted/mod_objs/" + tname,
            "assets_converted/textures/" + tname,
            "assets/textures/" + tname,
            tname
        };
        
        // Add case-insensitive or prefix-aware variants if needed
        if (tname.find("[r]") == std::string::npos) {
            candidates.insert(candidates.begin(), "assets_converted/mod_objs/[r]" + tname);
        }

        std::string resolved_tex;
        for (const auto& c : candidates) {
            resolved_tex = VFS::Resolve(c);
            if (!resolved_tex.empty()) break;
        }
        
        if (!resolved_tex.empty()) {
            int w, h, n;
            unsigned char* data = stbi_load(resolved_tex.c_str(), &w, &h, &n, 4);
            if (data) {
                lm.textures[ti] = bgfx::createTexture2D(
                    (uint16_t)w, (uint16_t)h, false, 1,
                    bgfx::TextureFormat::RGBA8, 0,
                    bgfx::copy(data, w * h * 4)
                );
                stbi_image_free(data);
                spdlog::debug("CharRenderer: loaded texture {} for {}", tname, path);
            }
        }
    }

    i.models[resolved] = std::move(lm);

    SharedAnimationData sa;
    std::string model_filename = resolved.substr(resolved.find_last_of("/\\") + 1);
    std::string base_name = model_filename.substr(0, model_filename.find_last_of('.'));
    
    const char* clip_names[] = {"idle", "walk", "run", "attack", "die"};
    for (auto* name : clip_names) {
        // Search in animations directory
        std::string anim_base = "assets/animations/" + base_name + "_" + name;
        std::string json_path = VFS::Find(anim_base + ".anm.json");
        if (json_path.empty()) json_path = anim_base + ".anm.json";
        AnimClip clip;
        // Use a temporary AnimationSystem just to load the data
        AnimationSystem loader;
        if (loader.LoadFromJson(json_path, clip)) {
            clip.name = name;
            clip.loop = (std::string(name) != "die");
            sa.clips[name] = std::move(clip);
            spdlog::debug("CharRenderer: loaded anim clip '{}' from {}", name, json_path);
        } else {
            std::string anm_path = VFS::Find(anim_base + ".anm");
            if (anm_path.empty()) anm_path = anim_base + ".anm";
            if (loader.LoadFromAnm(anm_path, clip)) {
                clip.name = name;
                clip.loop = (std::string(name) != "die");
                sa.clips[name] = std::move(clip);
                spdlog::debug("CharRenderer: loaded anim clip '{}' from {}", name, anm_path);
            }
        }
    }
    i.shared_anims[resolved] = std::move(sa);

    return 1;
}

uint32_t CharacterRenderer::Spawn(uint32_t id, const std::string& model,
                                   float x, float y, float z, uint32_t color) {
    auto& i = *impl_;
    LoadModel(model);
    RenderInstance inst;
    std::string found = VFS::Find(model);
    inst.model_key = found.empty() ? model : found;
    inst.pos = {x, y, z};
    inst.color = color;

    // Init instance animation
    auto ait = i.shared_anims.find(inst.model_key);
    if (ait != i.shared_anims.end()) {
        auto cit = ait->second.clips.find("idle");
        if (cit != ait->second.clips.end()) {
            inst.anim_state.anim_sys.Play(&cit->second, true, 0.0f);
        }
    }

    i.instances[id] = std::move(inst);
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

    auto anim_it = i.shared_anims.find(inst.model_key);
    if (anim_it == i.shared_anims.end()) return;
    auto& sa = anim_it->second;

    CharAnim desired = anim;
    if (desired == CharAnim::Idle && moving) desired = CharAnim::Walk;

    if (desired != inst.anim_state.current_anim) {
        auto clip_it = sa.clips.find(AnimNameForCharAnim(desired));
        if (clip_it != sa.clips.end()) {
            inst.anim_state.anim_sys.BlendTo(&clip_it->second, 0.2f);
            inst.anim_state.current_anim = desired;
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
    // Characters should NOT clear color
    bgfx::setViewClear(vid, BGFX_CLEAR_DEPTH, 0, 1.0f, 0);

        float white_col[4] = {1, 1, 1, 1};
        float ambient_col[4] = {0.5f, 0.5f, 0.5f, 1.0f}; // Default ambient
        
        bgfx::setUniform(i.light_dir, glm::value_ptr(env.light_dir));
        bgfx::setUniform(i.u_color, white_col);
        bgfx::setUniform(i.u_ambient, ambient_col);

        for (auto& [id_, inst] : i.instances) {
            (void)id_;
            auto mit = i.models.find(inst.model_key);
            if (mit == i.models.end()) continue;
            auto& lm = mit->second;

            inst.anim_state.anim_sys.Update(i.frame_dt);

            const AnimClip* current = inst.anim_state.anim_sys.GetCurrentClip();
            if (current) {
                int frame = (int)(inst.anim_state.anim_sys.GetCurrentTime() * current->fps);
                i.sfx_sync.OnAnimationFrame(current->name, frame, inst.pos.x, inst.pos.y, inst.pos.z);
            }

            glm::mat4 bone_matrices[MAX_BONES];
            std::fill_n(bone_matrices, MAX_BONES, glm::mat4(1.0f));

            if (lm.model.HasBones()) {
                std::vector<glm::mat4> blended_pose(lm.bind_pose.size(), glm::mat4(1.0f));
                inst.anim_state.anim_sys.GetBlendedPose(lm.bind_pose, blended_pose.data(), blended_pose.size());

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

        const auto& mesh_parts = lm.model.GetMeshes();
        for (size_t mi = 0; mi < lm.meshes.size(); mi++) {
            auto& mesh_obj = lm.meshes[mi];
            uint32_t mtl_idx = mesh_parts[mi].material_index;
            bgfx::TextureHandle mesh_tex = i.white;
            if (mtl_idx < lm.textures.size()) {
                mesh_tex = lm.textures[mtl_idx];
            }

            mesh_obj.Render(0);
            bgfx::setState(BGFX_STATE_DEFAULT | BGFX_STATE_WRITE_Z);
            bgfx::setTexture(0, i.tex, mesh_tex);
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

bool CharacterRenderer::LoadPreviewConfig(const std::string& cfg_path) {
    auto& i = *impl_;
    std::ifstream file(cfg_path);
    if (!file.is_open()) {
        if (!cfg_path.empty())
            spdlog::warn("CharRenderer: cannot open preview config: {}", cfg_path);
        return false;
    }

    std::string section, line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;
        if (line[0] == '[') { section = line; continue; }

        std::istringstream iss(line);
        std::string key;
        iss >> key;

        if (section.find("select") != std::string::npos || section.find("preview") != std::string::npos) {
            if (key == "cameraPosition") {
                iss >> i.preview_camera.position.x >> i.preview_camera.position.y >> i.preview_camera.position.z;
            } else if (key == "cameraRotation") {
                float rx, ry, rz;
                iss >> rx >> ry >> rz;
                glm::vec3 dir;
                dir.x = cosf(ry) * cosf(rx);
                dir.y = sinf(rx);
                dir.z = sinf(ry) * cosf(rx);
                i.preview_camera.target = i.preview_camera.position + dir * 300.0f;
            } else if (key == "orbitSpeed") {
                iss >> i.preview_camera.orbit_speed;
            } else if (key == "fov") {
                iss >> i.preview_camera.fov;
            } else if (key == "autoOrbit") {
                iss >> i.preview_camera.auto_orbit;
            } else if (key == "lightDir") {
                iss >> i.preview_lighting.light_dir.x >> i.preview_lighting.light_dir.y >> i.preview_lighting.light_dir.z;
            } else if (key == "ambient") {
                iss >> i.preview_lighting.ambient.r >> i.preview_lighting.ambient.g >> i.preview_lighting.ambient.b;
            } else if (key == "bgColor") {
                iss >> i.preview_lighting.bg_color.r >> i.preview_lighting.bg_color.g >> i.preview_lighting.bg_color.b >> i.preview_lighting.bg_color.a;
            } else if (key == "player1" || key == "player2" || key == "player3" || key == "player4" || key == "player5") {
                size_t idx = key[6] - '1';
                if (idx < MAX_PREVIEW_SLOTS) {
                    iss >> i.preview_slots[idx].position.x >> i.preview_slots[idx].position.y >> i.preview_slots[idx].position.z;
                }
            } else if (key == "playerScale") {
                float s; iss >> s;
                for (auto& slot : i.preview_slots) slot.scale = s;
            }
        }
    }

    i.preview_config_loaded = true;
    return true;
}

void CharacterRenderer::SetPreviewSlot(size_t index, const glm::vec3& pos, float scale) {
    if (index < MAX_PREVIEW_SLOTS) {
        impl_->preview_slots[index].position = pos;
        impl_->preview_slots[index].scale = scale;
    }
}

void CharacterRenderer::SetPreviewCamera(const glm::vec3& pos, const glm::vec3& target, float fov, float orbit_speed) {
    impl_->preview_camera.position = pos;
    impl_->preview_camera.target = target;
    impl_->preview_camera.fov = fov;
    impl_->preview_camera.orbit_speed = orbit_speed;
}

void CharacterRenderer::SetPreviewLighting(const glm::vec3& light_dir, const glm::vec3& ambient, const glm::vec4& bg_color) {
    impl_->preview_lighting.light_dir = light_dir;
    impl_->preview_lighting.ambient = ambient;
    impl_->preview_lighting.bg_color = bg_color;
}

void CharacterRenderer::SetupPreviewView(glm::mat4& out_view, glm::mat4& out_proj,
                                         float time, uint16_t fb_w, uint16_t fb_h) const {
    auto& i = *impl_;
    float aspect = (fb_w > 0 && fb_h > 0) ? static_cast<float>(fb_w) / fb_h : 16.0f / 9.0f;

    if (i.preview_camera.auto_orbit) {
        float angle = time * i.preview_camera.orbit_speed;
        glm::vec3 offset = i.preview_camera.position - i.preview_camera.target;
        float radius = glm::length(offset);
        glm::vec3 orbited(
            i.preview_camera.target.x + radius * cosf(angle),
            i.preview_camera.position.y,
            i.preview_camera.target.z + radius * sinf(angle)
        );
        out_view = glm::lookAt(orbited, i.preview_camera.target, glm::vec3(0.0f, 1.0f, 0.0f));
    } else {
        out_view = glm::lookAt(i.preview_camera.position, i.preview_camera.target, glm::vec3(0.0f, 1.0f, 0.0f));
    }

    out_proj = glm::perspective(glm::radians(i.preview_camera.fov), aspect, 1.0f, 5000.0f);
}

void CharacterRenderer::SelectSlot(uint32_t id) {
    auto& i = *impl_;
    auto it = i.instances.find(id);
    if (it == i.instances.end()) return;
    i.selected_slot = static_cast<int>(id);
    i.select_anim_time = 0.0f;

    auto& inst = it->second;
    auto ait = i.shared_anims.find(inst.model_key);
    if (ait != i.shared_anims.end()) {
        auto clip_it = ait->second.clips.find("selected");
        if (clip_it != ait->second.clips.end()) {
            inst.anim_state.anim_sys.Play(&clip_it->second, false, 0.0f);
            inst.anim_state.current_anim = CharAnim::Selected;
        }
    }
}

void CharacterRenderer::DeselectSlot(uint32_t id) {
    auto& i = *impl_;
    auto it = i.instances.find(id);
    if (it == i.instances.end()) return;
    i.selected_slot = -1;
    i.select_anim_time = 0.0f;

    auto& inst = it->second;
    auto ait = i.shared_anims.find(inst.model_key);
    if (ait != i.shared_anims.end()) {
        auto clip_it = ait->second.clips.find("deselected");
        if (clip_it != ait->second.clips.end()) {
            inst.anim_state.anim_sys.Play(&clip_it->second, false, 0.0f);
            inst.anim_state.current_anim = CharAnim::Deselected;
        }
    }
}

const PreviewSlotConfig& CharacterRenderer::GetPreviewSlot(size_t index) const {
    static PreviewSlotConfig fallback;
    if (index < MAX_PREVIEW_SLOTS) return impl_->preview_slots[index];
    return fallback;
}

const PreviewCameraConfig& CharacterRenderer::GetPreviewCamera() const {
    return impl_->preview_camera;
}

const PreviewLightingConfig& CharacterRenderer::GetPreviewLighting() const {
    return impl_->preview_lighting;
}

void CharacterRenderer::Shutdown() {
    if (!impl_->initialized) return;
    impl_->initialized = false;

    impl_->models.clear();
    impl_->shared_anims.clear();
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
