#pragma once
#include <rendering/TerrainRenderer.hpp>
#include <rendering/PropRenderer.hpp>
#include <engine/gx_render/RenderDevice.h>
#include <string>

class EngineMap {
public:
    void SetTerrain(TerrainRenderer* t) { terrain_ = t; }
    void SetProps(PropRenderer* p) { props_ = p; }
    
    bool Load(const std::string& map_id);
    void Unload();
    bool IsLoaded() const { return loaded_; }
    
    float GetHeight(float x, float z) const { return terrain_ ? terrain_->GetHeight(x, z) : 0; }
    const std::string& GetCurrentMapID() const { return current_map_; }
    
    void LoadSceneObjects(const std::string& json_path);
    void LoadFarmProps(const std::string& farm_dir);

    const EnvData& GetEnv() const { return env_; }

private:
    TerrainRenderer* terrain_ = nullptr;
    PropRenderer* props_ = nullptr;
    std::string current_map_;
    EnvData env_;
    bool loaded_ = false;
};
