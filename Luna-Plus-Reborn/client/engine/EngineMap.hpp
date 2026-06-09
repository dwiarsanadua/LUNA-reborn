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
class PhysicsWorld;

struct NPCPosition;
struct MonsterSpawn;

struct MapBounds {
    float min_x = -500.0f, max_x = 500.0f;
    float min_z = -500.0f, max_z = 500.0f;
    float min_y = -50.0f, max_y = 200.0f;
};

class EngineMap {
public:
    void SetTerrain(TerrainRenderer* t) { terrain_ = t; }
    void SetProps(PropRenderer* p) { props_ = p; }
    void SetAudio(AudioManager* a) { audio_ = a; }
    void SetGameDataDB(GameDataDB* db) { gamedb_ = db; }
    void SetSpawnSystem(SpawnSystem* sys) { spawn_sys_ = sys; }
    void SetRegistry(entt::registry* reg) { registry_ = reg; }
    void SetPhysicsWorld(PhysicsWorld* phys) { physics_ = phys; }

    bool Load(const std::string& map_id);
    void Unload();
    bool IsLoaded() const { return loaded_; }

    float GetHeight(float x, float z) const { return terrain_ ? terrain_->GetHeight(x, z) : 0; }
    const std::string& GetCurrentMapID() const { return current_map_; }

    void LoadSceneObjects(const std::string& json_path);
    void LoadFarmProps(const std::string& farm_dir);

    const EnvData& GetEnv() const { return env_; }

    static bool CollisionLine(const TerrainRenderer* terrain,
                              const glm::vec3& start, const glm::vec3& end,
                              glm::vec3* out_target);

    bool TryMove(glm::vec3 from, glm::vec3 to, float radius, entt::entity entity);
    glm::vec3 CalculateSlide(const glm::vec3& from, const glm::vec3& to) const;
    MapBounds GetMapBounds() const;

    void SetEntityPosition(entt::entity entity, const glm::vec3& pos);

private:
    TerrainRenderer* terrain_ = nullptr;
    PropRenderer* props_ = nullptr;
    AudioManager* audio_ = nullptr;
    GameDataDB* gamedb_ = nullptr;
    SpawnSystem* spawn_sys_ = nullptr;
    PhysicsWorld* physics_ = nullptr;
    entt::registry* registry_ = nullptr;
    std::string current_map_;
    EnvData env_;
    bool loaded_ = false;

    void InitBGMMap();
    std::string GetBGMForMap(const std::string& map_id) const;
    std::unordered_map<std::string, std::string> bgm_map_;
};
