#pragma once
#include <bgfx/bgfx.h>
#include <string>
#include <unordered_map>
#include <memory>

class Shader {
public:
    bool LoadProgram(const std::string& name,
                     const std::string& vs_path,
                     const std::string& fs_path);
    void Bind() const;
    void Unbind() const;
    void Destroy();

    bgfx::ProgramHandle GetProgram() const { return program_; }
    const std::string& GetName() const { return name_; }
    bool IsValid() const { return bgfx::isValid(program_); }

private:
    bgfx::ProgramHandle program_ = BGFX_INVALID_HANDLE;
    std::string name_;
};

namespace ShaderUtils {
    const bgfx::Memory* LoadShaderBin(const std::string& name);
    bgfx::ProgramHandle LoadProgram(const std::string& vs_name, const std::string& fs_name);
}

class ShaderManager {
public:
    bool Init(const std::string& shader_dir);
    Shader* GetProgram(const std::string& name);
    void DestroyAll();

    Shader* GetDefaultShader();
    Shader* GetUnlitShader();
    Shader* GetLitShader();

private:
    std::unordered_map<std::string, std::unique_ptr<Shader>> programs_;
    std::string shader_dir_;
};
