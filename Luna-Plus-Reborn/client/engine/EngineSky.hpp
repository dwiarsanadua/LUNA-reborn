#pragma once
#include <cstdint>
#include <glm/glm.hpp>
#include <bgfx/bgfx.h>

class UIRenderer;

class EngineSky {
public:
    void Init();
    void SetSampler(bgfx::UniformHandle sampler, bgfx::TextureHandle white_tex);
    void Update(float dt);
    void Render(class UIRenderer& ui, const glm::mat4& view, const glm::mat4& proj);
    void Shutdown();
    
    // Time of day (0.0 = midnight, 0.5 = noon, 1.0 = midnight)
    void SetTimeOfDay(float t) { time_of_day_ = t; }
    float GetTimeOfDay() const { return time_of_day_; }
    void SetTimeSpeed(float speed) { time_speed_ = speed; }
    
    // Sky colors
    glm::vec3 GetSkyTopColor() const;
    glm::vec3 GetSunColor() const;
    glm::vec3 GetAmbientColor() const;
    glm::vec3 GetFogColor() const;
    float GetSunIntensity() const;
    
    // Cloud layer
    void SetCloudCover(float c) { cloud_cover_ = std::max(0.0f, std::min(1.0f, c)); }
    float GetCloudCover() const { return cloud_cover_; }
    
    // Weather integration
    void SetWeatherType(int type) { weather_type_ = type; } // 0=clear, 1=rain, 2=snow
    
    // Get light direction for shaders
    glm::vec3 GetLightDirection() const;

private:
    void BuildDomeMesh();
    bgfx::TextureHandle LoadSkyTexture(const std::string& name);

    float time_of_day_ = 0.5f;
    float time_speed_ = 0.01f;
    float cloud_cover_ = 0.2f;
    int weather_type_ = 0;
    float cloud_offset_ = 0;

    // Sky dome rendering
    bgfx::VertexBufferHandle vb_ = BGFX_INVALID_HANDLE;
    bgfx::IndexBufferHandle ib_ = BGFX_INVALID_HANDLE;
    bgfx::TextureHandle sky_tex_ = BGFX_INVALID_HANDLE;
    bgfx::TextureHandle cloud_tex_ = BGFX_INVALID_HANDLE;
    bgfx::ProgramHandle program_ = BGFX_INVALID_HANDLE;
    bgfx::UniformHandle sampler_ = BGFX_INVALID_HANDLE;
    bgfx::UniformHandle s_texSky_ = BGFX_INVALID_HANDLE;
    bgfx::TextureHandle white_tex_ = BGFX_INVALID_HANDLE;
    
    glm::vec3 LerpColor(const glm::vec3& a, const glm::vec3& b, float t) const;
};
