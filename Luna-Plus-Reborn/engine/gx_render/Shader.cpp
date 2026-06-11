#include "Shader.h"
#include "VFS.h"
#include <spdlog/spdlog.h>
#include <fstream>
#include <vector>

static bgfx::ShaderHandle LoadShader(const std::string& path) {
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file) {
        spdlog::error("Shader: failed to load '{}'", path);
        return BGFX_INVALID_HANDLE;
    }

    size_t size = (size_t)file.tellg();
    file.seekg(0);
    // Use bgfx::alloc so bgfx owns the memory (safe with makeRef-based paths too)
    const bgfx::Memory* mem = bgfx::alloc(static_cast<uint32_t>(size));
    if (!file.read(reinterpret_cast<char*>(mem->data), (std::streamsize)size)) {
        spdlog::error("Shader: failed to read '{}'", path);
        return BGFX_INVALID_HANDLE;
    }
    file.close();

    bgfx::ShaderHandle handle = bgfx::createShader(mem);
    if (!bgfx::isValid(handle)) {
        spdlog::error("Shader: failed to create bgfx shader from '{}'", path);
    }
    return handle;
}

bool Shader::LoadProgram(const std::string& name,
                          const std::string& vs_path,
                          const std::string& fs_path) {
    name_ = name;

    bgfx::ShaderHandle vs = LoadShader(vs_path);
    bgfx::ShaderHandle fs = LoadShader(fs_path);

    if (!bgfx::isValid(vs) || !bgfx::isValid(fs)) {
        spdlog::error("Shader: failed to load program '{}'", name);
        if (bgfx::isValid(vs)) bgfx::destroy(vs);
        if (bgfx::isValid(fs)) bgfx::destroy(fs);
        return false;
    }

    program_ = bgfx::createProgram(vs, fs, true);
    if (!bgfx::isValid(program_)) {
        spdlog::error("Shader: failed to link program '{}'", name);
        return false;
    }

    spdlog::debug("Shader: loaded program '{}'", name);
    return true;
}

void Shader::Bind() const {
    if (bgfx::isValid(program_)) {
        bgfx::submit(0, program_);
    }
}

void Shader::Unbind() const {
}

void Shader::Destroy() {
    if (bgfx::isValid(program_)) {
        bgfx::destroy(program_);
        program_ = BGFX_INVALID_HANDLE;
    }
}

// ─── ShaderUtils ───────────────────────────────────────────

// Return bgfx renderer sub-directory name for platform-specific shader binaries.
// bgfx compiles .sc files per-backend; the .bin files in the root shaders/ dir
// are Metal (macOS) by default.  Windows D3D11 bins live in shaders/dx11/ etc.
static const char* GetShaderSubdir() {
    switch (bgfx::getRendererType()) {
        case bgfx::RendererType::Metal:         return "metal";
        case bgfx::RendererType::Direct3D11:    return "dx11";
        case bgfx::RendererType::Direct3D12:    return "dx12";
        case bgfx::RendererType::Vulkan:        return "spirv";
        case bgfx::RendererType::OpenGL:        return "glsl";
        case bgfx::RendererType::OpenGLES:      return "essl";
        default:                                return "";
    }
}

const bgfx::Memory* ShaderUtils::LoadShaderBin(const std::string& name) {
    const char* subdir = GetShaderSubdir();

    // Build candidate list: platform-specific subfolder first, then root fallback.
    // name may be "shaders/vs_ui.bin" or just "vs_ui.bin" — handle both forms.
    std::string base = name;
    std::string dir_prefix;
    auto slash = name.rfind('/');
    if (slash != std::string::npos) {
        dir_prefix = name.substr(0, slash + 1);
        base = name.substr(slash + 1);
    }

    std::vector<std::string> candidates;

    // Priority 1: platform subfolder (e.g. shaders/metal/vs_ui.bin)
    if (subdir[0] != '\0') {
        const std::string sub_name = dir_prefix + std::string(subdir) + "/" + base;
        for (const std::string& prefix : {"", "build/bin/", "bin/"}) {
            candidates.push_back(prefix + sub_name);
        }
        // Also try via VFS
        candidates.push_back(VFS::Resolve(sub_name));
    }

    // Priority 2: as-given (root shaders/ dir, pre-compiled for current platform)
    for (const std::string& prefix : {"", "build/bin/", "bin/", "../"}) {
        candidates.push_back(prefix + name);
    }
    candidates.push_back(VFS::Resolve(name));
    candidates.push_back(VFS::Resolve("assets/shaders/" + base));

    for (const auto& p : candidates) {
        if (p.empty()) continue;
        std::ifstream file(p, std::ios::binary | std::ios::ate);
        if (file) {
            size_t size = (size_t)file.tellg();
            file.seekg(0);
            auto* mem = bgfx::alloc(static_cast<uint32_t>(size));
            file.read(reinterpret_cast<char*>(mem->data), (std::streamsize)size);
            spdlog::info("ShaderUtils: loaded {} ({} bytes) [{}]", p, size, subdir[0] ? subdir : "generic");
            return mem;
        }
    }
    spdlog::error("ShaderUtils: failed to load '{}' (renderer={})", name,
                  bgfx::getRendererName(bgfx::getRendererType()));
    return nullptr;
}

bgfx::ProgramHandle ShaderUtils::LoadProgram(const std::string& vs_name, const std::string& fs_name) {
    auto vs = LoadShaderBin(vs_name);
    auto fs = LoadShaderBin(fs_name);
    if (!vs || !fs) return BGFX_INVALID_HANDLE;

    bgfx::ShaderHandle vs_h = bgfx::createShader(vs);
    bgfx::ShaderHandle fs_h = bgfx::createShader(fs);

    if (!bgfx::isValid(vs_h) || !bgfx::isValid(fs_h)) {
        spdlog::error("ShaderUtils: failed to create shader handles");
        if (bgfx::isValid(vs_h)) bgfx::destroy(vs_h);
        if (bgfx::isValid(fs_h)) bgfx::destroy(fs_h);
        return BGFX_INVALID_HANDLE;
    }

    bgfx::ProgramHandle prog = bgfx::createProgram(vs_h, fs_h, true);
    if (!bgfx::isValid(prog)) {
        spdlog::error("ShaderUtils: failed to create program from '{}' / '{}'", vs_name, fs_name);
    }
    return prog;
}

// ─── ShaderManager ─────────────────────────────────────────

bool ShaderManager::Init(const std::string& shader_dir) {
    shader_dir_ = shader_dir;

    auto load = [&](const std::string& name,
                    const std::string& vs, const std::string& fs) -> bool {
        auto prog = std::make_unique<Shader>();
        if (!prog->LoadProgram(name, vs, fs)) return false;
        programs_[name] = std::move(prog);
        return true;
    };

    auto dir = shader_dir_ + "/";
    load("default", dir + "vs_default.bin", dir + "fs_default.bin");
    load("unlit", dir + "vs_unlit.bin", dir + "fs_unlit.bin");
    load("lit", dir + "vs_default.bin", dir + "fs_lit.bin");
    load("main", dir + "vs_main.bin", dir + "fs_main.bin");

    spdlog::info("ShaderManager: initialized ({} programs)", programs_.size());
    return !programs_.empty();
}

Shader* ShaderManager::GetProgram(const std::string& name) {
    auto it = programs_.find(name);
    return (it != programs_.end()) ? it->second.get() : nullptr;
}

void ShaderManager::DestroyAll() {
    programs_.clear();
}

Shader* ShaderManager::GetDefaultShader() {
    return GetProgram("default");
}

Shader* ShaderManager::GetUnlitShader() {
    return GetProgram("unlit");
}

Shader* ShaderManager::GetLitShader() {
    return GetProgram("lit");
}

Shader* ShaderManager::GetMainShader() {
    return GetProgram("main");
}
