#pragma once
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <unordered_map>
#include <cstdint>

enum CharAnim { CHAR_IDLE, CHAR_WALK, CHAR_RUN, CHAR_ATTACK, CHAR_DIE };

struct CharInstance {
    uint32_t id = 0;
    std::string model;
    float x = 0, y = 0, z = 0;
    float rot = 0;
    uint32_t color = 0xff4488cc;
    CharAnim anim = CHAR_IDLE;
    bool moving = false;
    float speed = 0;
};

void CharRenderer_Init();
void CharRenderer_SetFBSize(uint16_t w, uint16_t h);
void CharRenderer_SetFrameDelta(float dt);
uint32_t CharRenderer_LoadModel(const std::string& glb_path);
void CharRenderer_Spawn(uint32_t instance_id, const std::string& model,
                         float x, float y, float z, uint32_t color = 0xff4488cc);
void CharRenderer_Remove(uint32_t instance_id);
void CharRenderer_Move(uint32_t instance_id, float x, float y, float z, bool moving, CharAnim anim = CHAR_IDLE);

// 1.2C Attachment System
void CharRenderer_Attach(uint32_t instance_id, const std::string& model_path, const std::string& bone_name);
void CharRenderer_Detach(uint32_t instance_id, const std::string& bone_name);

struct EnvData;
void CharRenderer_Render(const glm::mat4& view, const glm::mat4& proj, float time);
void CharRenderer_Render(const glm::mat4& view, const glm::mat4& proj, float time, const EnvData& env);
void CharRenderer_Shutdown();
