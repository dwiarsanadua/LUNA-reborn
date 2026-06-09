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
    std::vector<uint8_t> data(size);
    if (!file.read(reinterpret_cast<char*>(data.data()), (std::streamsize)size)) {
        spdlog::error("Shader: failed to read '{}'", path);
        return BGFX_INVALID_HANDLE;
    }
    file.close();

    bgfx::ShaderHandle handle = bgfx::createShader(
        bgfx::makeRef(data.data(), (uint32_t)data.size()));

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

const bgfx::Memory* ShaderUtils::LoadShaderBin(const std::string& name) {
    std::string searchPaths[] = {
        "build/bin/" + name,
        "bin/" + name,
        name,
        "../" + name,
        "shaders/" + name,
        VFS::Resolve("assets/shaders/" + name),
        VFS::Resolve("assets/" + name),
    };

    for (const auto& p : searchPaths) {
        std::ifstream file(p, std::ios::binary | std::ios::ate);
        if (file) {
            size_t size = file.tellg();
            file.seekg(0);
            auto* mem = bgfx::alloc(static_cast<uint32_t>(size));
            file.read(reinterpret_cast<char*>(mem->data), size);
            spdlog::info("ShaderUtils: loaded {} ({} bytes)", p, size);
            return mem;
        }
    }
    spdlog::error("ShaderUtils: failed to load '{}'", name);
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
