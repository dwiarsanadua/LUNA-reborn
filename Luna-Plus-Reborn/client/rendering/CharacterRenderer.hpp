#pragma once
#include <bgfx/bgfx.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <cstdint>
#include <engine/gx_render/RenderDevice.h>

enum class CharAnim { Idle, Walk, Run, Attack, Die };
struct CharInstanceData;  // PIMPL

class CharacterRenderer {
public:
    CharacterRenderer();
    ~CharacterRenderer();

    bool Init(const std::string& shader_dir = "shaders/");
    void SetFramebufferSize(uint16_t w, uint16_t h);
    void SetFrameDelta(float dt);

    uint32_t LoadModel(const std::string& path);
    uint32_t Spawn(uint32_t instance_id, const std::string& model,
                   float x, float y, float z, uint32_t color = 0xff4488cc);
    void Remove(uint32_t instance_id);
    void Move(uint32_t id, float x, float y, float z, bool moving, CharAnim anim = CharAnim::Idle);

    void Attach(uint32_t id, const std::string& model, const std::string& bone);
    void Detach(uint32_t id, const std::string& bone);

    void Render(const glm::mat4& view, const glm::mat4& proj, float time, const EnvData& env = EnvData());
    void Shutdown();

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

inline CharacterRenderer* g_char_renderer = nullptr;

// Backward compatibility wrappers
inline void CharRenderer_Init() { if (g_char_renderer) g_char_renderer->Init(); }
inline void CharRenderer_SetFBSize(uint16_t w, uint16_t h) { if (g_char_renderer) g_char_renderer->SetFramebufferSize(w, h); }
inline void CharRenderer_SetFrameDelta(float dt) { if (g_char_renderer) g_char_renderer->SetFrameDelta(dt); }
inline uint32_t CharRenderer_LoadModel(const std::string& path) { return g_char_renderer ? g_char_renderer->LoadModel(path) : 0; }
inline void CharRenderer_Spawn(uint32_t id, const std::string& model, float x, float y, float z, uint32_t color = 0xff4488cc) { if (g_char_renderer) g_char_renderer->Spawn(id, model, x, y, z, color); }
inline void CharRenderer_Remove(uint32_t id) { if (g_char_renderer) g_char_renderer->Remove(id); }
inline void CharRenderer_Move(uint32_t id, float x, float y, float z, bool moving, CharAnim anim = CharAnim::Idle) { if (g_char_renderer) g_char_renderer->Move(id, x, y, z, moving, anim); }
inline void CharRenderer_Attach(uint32_t id, const std::string& model, const std::string& bone) { if (g_char_renderer) g_char_renderer->Attach(id, model, bone); }
inline void CharRenderer_Detach(uint32_t id, const std::string& bone) { if (g_char_renderer) g_char_renderer->Detach(id, bone); }
inline void CharRenderer_Render(const glm::mat4& view, const glm::mat4& proj, float time) { if (g_char_renderer) g_char_renderer->Render(view, proj, time); }
inline void CharRenderer_Render(const glm::mat4& view, const glm::mat4& proj, float time, const EnvData& env) { if (g_char_renderer) g_char_renderer->Render(view, proj, time, env); }
inline void CharRenderer_Shutdown() { if (g_char_renderer) g_char_renderer->Shutdown(); }
