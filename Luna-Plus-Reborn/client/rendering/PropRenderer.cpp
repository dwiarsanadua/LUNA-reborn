#include "PropRenderer.hpp"
#include <spdlog/spdlog.h>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <cstring>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stb_image.h>
#include <engine/gx_render/VFS.h>

static const bgfx::Memory* loadShader(const char* path) {
    std::string searchPaths[] = {
        std::string("build/bin/") + path,
        std::string("bin/") + path,
        path,
        std::string("../") + path,
        VFS::Resolve(std::string("assets/") + path)
    };

    for (const auto& p : searchPaths) {
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

struct ObjVertex { float x, y, z; float nx, ny, nz; float u, v; };
struct ObjFace { int v[3]; int vn[3]; int vt[3]; };

PropRenderer::PropRenderer() = default;
PropRenderer::~PropRenderer() { Shutdown(); }

bgfx::TextureHandle PropRenderer::LoadTextureForMesh(const std::string& base_name) {
    if (base_name.empty()) return BGFX_INVALID_HANDLE;
    auto it = tex_cache_.find(base_name);
    if (it != tex_cache_.end()) return it->second;

    std::string variations[] = { base_name, base_name + ".png", base_name + ".dds" };
    std::string base_paths[] = { VFS::Resolve("assets/textures/unpacked/map/"), VFS::Resolve("assets/textures/unpacked/farm/"), VFS::Resolve("assets/textures/unpacked/character/"), VFS::Resolve("assets/textures/unpacked/monster/"), VFS::Resolve("assets/textures/unpacked/npc/"), VFS::Resolve("assets/textures/unpacked/"), VFS::Resolve("assets/textures/"), VFS::Resolve("assets_converted/mod_objs/"), VFS::Resolve("assets/unpacked/image/") };

    for (auto& base_path : base_paths) {
        for (auto& v : variations) {
            std::string p = base_path + v;
            int w, h, n;
            unsigned char* d = stbi_load(p.c_str(), &w, &h, &n, 4);
            if (d) {
                auto tex = bgfx::createTexture2D((uint16_t)w, (uint16_t)h, false, 1,
                    bgfx::TextureFormat::RGBA8, BGFX_SAMPLER_NONE,
                    bgfx::copy(d, w * h * 4));
                stbi_image_free(d);
                tex_cache_[base_name] = tex;
                spdlog::debug("PropRenderer: loaded texture {}", p);
                return tex;
            }
        }
    }
    tex_cache_[base_name] = BGFX_INVALID_HANDLE;
    return BGFX_INVALID_HANDLE;
}

bgfx::TextureHandle PropRenderer::LoadNormalMap(const std::string& base_name) {
    std::string variations[] = { base_name + "_n", base_name + "_normal", base_name + "_norm" };
    for (auto& v : variations) {
        auto handle = LoadTextureForMesh(v);
        if (bgfx::isValid(handle)) return handle;
    }
    return BGFX_INVALID_HANDLE;
}

bool PropRenderer::Init() {
    auto vs = loadShader("shaders/vs_default.bin");
    auto fs = loadShader("shaders/fs_lit.bin");
    if (!vs || !fs) { spdlog::error("PropRenderer: shader load failed"); return false; }
    program_ = bgfx::createProgram(bgfx::createShader(vs), bgfx::createShader(fs), true);
    u_color_ = bgfx::createUniform("u_color", bgfx::UniformType::Vec4);
    u_light_dir_ = bgfx::createUniform("u_lightDir", bgfx::UniformType::Vec4);
    s_tex_color_ = bgfx::createUniform("s_texColor", bgfx::UniformType::Sampler);
    s_tex_normal_ = bgfx::createUniform("s_texNormal", bgfx::UniformType::Sampler);
    uint32_t white = 0xffffffff;
    white_tex_ = bgfx::createTexture2D(1, 1, false, 1, bgfx::TextureFormat::RGBA8, 0, bgfx::makeRef(&white, 4));

    layout_.begin()
        .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
        .add(bgfx::Attrib::Normal, 3, bgfx::AttribType::Float)
        .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
        .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
        .end();

    spdlog::info("PropRenderer: initialized with GPU instancing support");
    return true;
}

int PropRenderer::LoadObj(const std::string& path) {
    // Check if mesh already loaded
    for (size_t i = 0; i < meshes_.size(); i++) {
        if (meshes_[i].name == path) return (int)i;
    }

    std::ifstream file(path);
    if (!file) { spdlog::error("PropRenderer: cannot open {}", path); return -1; }

    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> uvs;
    std::vector<ObjFace> faces;

    std::string line;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string cmd;
        ss >> cmd;
        if (cmd == "v") {
            float x, y, z; ss >> x >> y >> z;
            positions.push_back({x, y, z});
        } else if (cmd == "vn") {
            float x, y, z; ss >> x >> y >> z;
            normals.push_back({x, y, z});
        } else if (cmd == "vt") {
            float u, v; ss >> u >> v;
            uvs.push_back({u, 1.0f - v});
        } else if (cmd == "f") {
            ObjFace f;
            for (int i = 0; i < 3; i++) {
                std::string seg; ss >> seg;
                size_t slash1 = seg.find('/');
                size_t slash2 = seg.find('/', slash1 + 1);
                f.v[i] = std::stoi(seg.substr(0, slash1)) - 1;
                if (slash1 != std::string::npos && slash2 != std::string::npos && slash2 > slash1 + 1) {
                    f.vt[i] = std::stoi(seg.substr(slash1 + 1, slash2 - slash1 - 1)) - 1;
                    f.vn[i] = std::stoi(seg.substr(slash2 + 1)) - 1;
                } else if (slash1 != std::string::npos) {
                    f.vt[i] = std::stoi(seg.substr(slash1 + 1)) - 1;
                    f.vn[i] = -1;
                } else {
                    f.vt[i] = -1;
                    f.vn[i] = -1;
                }
            }
            faces.push_back(f);
        }
    }

    if (faces.empty()) return -1;

    struct Vertex { float x, y, z; float nx, ny, nz; uint32_t color; float u, v; };
    std::vector<Vertex> bverts;
    std::vector<uint16_t> bidx;

    for (size_t i = 0; i < faces.size(); i++) {
        for (int j = 0; j < 3; j++) {
            int vidx = faces[i].v[j];
            int nidx = faces[i].vn[j];
            int tidx = faces[i].vt[j];

            glm::vec3 p = positions[vidx];
            glm::vec3 n = (nidx >= 0 && nidx < (int)normals.size()) ? normals[nidx] : glm::vec3(0, 1, 0);
            glm::vec2 t = (tidx >= 0 && tidx < (int)uvs.size()) ? uvs[tidx] : glm::vec2(0, 0);

            bverts.push_back({p.x, p.y, p.z, n.x, n.y, n.z, 0xffffffff, t.x, t.y});
            bidx.push_back((uint16_t)bverts.size() - 1);
        }
    }

    // Try to load matching texture
    std::string fname = path.substr(path.find_last_of("/\\") + 1);
    size_t dot = fname.find_last_of('.');
    std::string base = (dot != std::string::npos) ? fname.substr(0, dot) : fname;
    std::transform(base.begin(), base.end(), base.begin(), ::tolower);

    PropMesh mesh;
    mesh.vb = bgfx::createVertexBuffer(bgfx::copy(bverts.data(), (uint32_t)(bverts.size() * sizeof(Vertex))), layout_);
    mesh.ib = bgfx::createIndexBuffer(bgfx::copy(bidx.data(), (uint32_t)(bidx.size() * sizeof(uint16_t))));
    mesh.num_indices = (uint32_t)bidx.size();
    mesh.tex = LoadTextureForMesh(base);
    mesh.normal_tex = LoadNormalMap(base);
    mesh.name = path;

    int idx = (int)meshes_.size();
    meshes_.push_back(mesh);
    spdlog::info("PropRenderer: loaded mesh '{}' ({} indices, {} verts)", path, bidx.size(), bverts.size());
    return idx;
}

int PropRenderer::AddInstance(int mesh_idx, glm::vec3 pos, float scale, glm::vec3 rot) {
    if (mesh_idx < 0 || mesh_idx >= (int)meshes_.size()) return -1;
    PropInstance inst;
    inst.position = pos;
    inst.scale = scale;
    inst.rotation = rot;
    inst.mesh_idx = (uint32_t)mesh_idx;
    instances_.push_back(inst);
    return (int)instances_.size() - 1;
}

void PropRenderer::Render(const glm::mat4& view, const glm::mat4& proj) {
    EnvData env;
    Render(view, proj, env);
}

void PropRenderer::Render(const glm::mat4& view, const glm::mat4& proj, const EnvData& env) {
    if (!bgfx::isValid(program_)) return;
    if (instances_.empty()) return;

    bgfx::setViewTransform(view_id_, &view, &proj);
    bgfx::setViewClear(view_id_, BGFX_CLEAR_NONE, 0, 1.0f, 0);
    bgfx::setViewRect(view_id_, 0, 0, (uint16_t)width, (uint16_t)height);

    if (bgfx::isValid(u_light_dir_)) {
        bgfx::setUniform(u_light_dir_, glm::value_ptr(env.light_dir));
    }

    last_draw_calls_ = 0;

    // Build instance groups: mesh_idx -> list of model matrices
    struct InstanceGroup {
        int mesh_idx;
        std::vector<glm::mat4> transforms;
    };

    std::unordered_map<int, std::vector<glm::mat4>> groups;
    for (auto& inst : instances_) {
        glm::mat4 m = glm::translate(glm::mat4(1.0f), inst.position);
        m = glm::rotate(m, inst.rotation.x, glm::vec3(1,0,0));
        m = glm::rotate(m, inst.rotation.y, glm::vec3(0,1,0));
        m = glm::rotate(m, inst.rotation.z, glm::vec3(0,0,1));
        m = glm::scale(m, glm::vec3(inst.scale));
        groups[(int)inst.mesh_idx].push_back(m);
    }

    // Submit each group as a single instanced draw call
    for (auto& [mid, xforms] : groups) {
        if (mid < 0 || mid >= (int)meshes_.size()) continue;
        auto& mesh = meshes_[mid];
        if (!bgfx::isValid(mesh.vb)) continue;
        if (xforms.empty()) continue;

        // Build instance data buffer: each instance is a 4x3 matrix (12 floats)
        const uint32_t instance_stride = sizeof(float) * 12;
        bgfx::InstanceDataBuffer idb;
        bgfx::allocInstanceDataBuffer(&idb, (uint16_t)xforms.size(), instance_stride);

        uint8_t* data = idb.data;
        for (auto& xf : xforms) {
            // Write 4x3 matrix (row-major, last row implied)
            const float* p = glm::value_ptr(xf);
            memcpy(data, p, sizeof(float) * 12);
            data += instance_stride;
        }

        // Single instanced draw call
        bgfx::setTexture(0, s_tex_color_, bgfx::isValid(mesh.tex) ? mesh.tex : white_tex_);
        if (bgfx::isValid(s_tex_normal_)) {
            bgfx::setTexture(1, s_tex_normal_, bgfx::isValid(mesh.normal_tex) ? mesh.normal_tex : white_tex_);
        }
        bgfx::setVertexBuffer(0, mesh.vb);
        bgfx::setIndexBuffer(mesh.ib);
        bgfx::setInstanceDataBuffer(&idb);
        bgfx::setState(BGFX_STATE_DEFAULT | BGFX_STATE_WRITE_Z);
        bgfx::submit(view_id_, program_);
        last_draw_calls_++;
    }
}

void PropRenderer::Shutdown() {
    static bool is_shutdown = false;
    if (is_shutdown) return;
    is_shutdown = true;

    for (auto& m : meshes_) {
        if (bgfx::isValid(m.vb)) bgfx::destroy(m.vb);
        if (bgfx::isValid(m.ib)) bgfx::destroy(m.ib);
        if (bgfx::isValid(m.tex)) bgfx::destroy(m.tex);
        if (bgfx::isValid(m.normal_tex)) bgfx::destroy(m.normal_tex);
    }
    meshes_.clear();
    instances_.clear();
    for (auto& [n, t] : tex_cache_) if (bgfx::isValid(t)) bgfx::destroy(t);
    tex_cache_.clear();
    if (bgfx::isValid(white_tex_)) bgfx::destroy(white_tex_);
    if (bgfx::isValid(s_tex_color_)) bgfx::destroy(s_tex_color_);
    if (bgfx::isValid(s_tex_normal_)) bgfx::destroy(s_tex_normal_);
    if (bgfx::isValid(program_)) bgfx::destroy(program_);
    if (bgfx::isValid(u_color_)) bgfx::destroy(u_color_);
    if (bgfx::isValid(u_light_dir_)) bgfx::destroy(u_light_dir_);
}

void PropRenderer::ClearProps() {
    instances_.clear();
}
