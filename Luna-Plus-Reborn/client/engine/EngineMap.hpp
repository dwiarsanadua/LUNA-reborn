#pragma once
#include <rendering/TerrainRenderer.hpp>
#include <rendering/PropRenderer.hpp>
#include <engine/gx_render/RenderDevice.h>
#include <string>
#include <unordered_map>
#include <cstdint>
#include <glm/glm.hpp>
#include <entt/entt.hpp>

class AudioManager;
class GameDataDB;
class SpawnSystem;

struct NPCPosition;
struct MonsterSpawn;

class EngineMap {
public:
    void SetTerrain(TerrainRenderer* t) { terrain_ = t; }
    void SetProps(PropRenderer* p) { props_ = p; }
    void SetAudio(AudioManager* a) { audio_ = a; }
    void SetGameDataDB(GameDataDB* db) { gamedb_ = db; }
    void SetSpawnSystem(SpawnSystem* sys) { spawn_sys_ = sys; }
    void SetRegistry(entt::registry* reg) { registry_ = reg; }
    
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
    AudioManager* audio_ = nullptr;
    GameDataDB* gamedb_ = nullptr;
    SpawnSystem* spawn_sys_ = nullptr;
    entt::registry* registry_ = nullptr;
    std::string current_map_;
    EnvData env_;
    bool loaded_ = false;

    void InitBGMMap();
    std::string GetBGMForMap(const std::string& map_id) const;
    std::unordered_map<std::string, std::string> bgm_map_;
};
