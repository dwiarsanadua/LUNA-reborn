#include "Shader.h"
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
    load("light", dir + "vs_light.bin", dir + "fs_light.bin");

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

Shader* ShaderManager::GetLightShader() {
    return GetProgram("light");
}
