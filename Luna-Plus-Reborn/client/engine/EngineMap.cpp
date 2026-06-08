#include "EngineMap.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <spdlog/spdlog.h>
#include <audio/AudioManager.hpp>
#include <game/ecs/systems/GameDataDB.hpp>
#include <game/ecs/systems/SpawnSystem.hpp>
#include <entt/entt.hpp>

bool EngineMap::Load(const std::string& map_id) {
    current_map_ = map_id;
    if (!terrain_ || !props_) { spdlog::error("EngineMap: terrain/props not set"); return false; }
    
    std::string hgt_path = "assets_converted/maps/" + map_id + ".hgt";
    if (!terrain_->LoadFromHGT(hgt_path.c_str(), 0.1f)) {
        terrain_->Init(100, 12.0f);
        spdlog::info("EngineMap: procedural terrain for map {}", map_id);
    }
    
    std::string json_path = "assets_converted/maps/" + map_id + ".json";
    LoadSceneObjects(json_path);
    
    // Parse environment data from JSON
    std::ifstream jf(json_path);
    if (jf) {
        std::string jtext((std::istreambuf_iterator<char>(jf)), std::istreambuf_iterator<char>());
        auto parseVec4 = [&](const std::string& key, glm::vec4& out) {
            size_t pos = jtext.find("\"" + key + "\":");
            if (pos != std::string::npos) {
                size_t b1 = jtext.find('[', pos);
                size_t b2 = jtext.find(']', b1);
                if (b1 != std::string::npos && b2 != std::string::npos) {
                    float x, y, z, w = 1.0f;
                    if (sscanf(jtext.substr(b1+1, b2-b1-1).c_str(), "%f,%f,%f,%f", &x, &y, &z, &w) >= 3) {
                        out = glm::vec4(x, y, z, w);
                    }
                }
            }
        };
        parseVec4("light_dir", env_.light_dir);
        parseVec4("fog_data",  env_.fog_data);
        parseVec4("fog_color", env_.fog_color);
    }

    // Play BGM for this map with crossfade
    if (audio_) {
        std::string bgm = GetBGMForMap(map_id);
        audio_->SmoothBGMTransition(bgm, 1.0f);
    }

    // Spawn monsters and NPCs from GameDataDB
    if (spawn_sys_ && registry_) {
        try {
            uint32_t id = static_cast<uint32_t>(std::stoul(map_id));
            // Use external DB if provided, otherwise open internally
            GameDataDB* db = gamedb_;
            GameDataDB local_db;
            if (!db) {
                if (local_db.Open("data/game_data.db")) {
                    local_db.LoadMonsterTemplates();
                    local_db.LoadNPCTemplates();
                    local_db.LoadMapData();
                    db = &local_db;
                }
            }
            if (db) {
                auto spawns = db->GetMonsterSpawns(id);
                for (const auto& s : spawns) {
                    for (uint16_t i = 0; i < s.count; ++i) {
                        float ox = (static_cast<float>(rand() % 200) - 100.0f) * s.spawn_radius / 100.0f;
                        float oz = (static_cast<float>(rand() % 200) - 100.0f) * s.spawn_radius / 100.0f;
                        spawn_sys_->SpawnMonster(*registry_, s.monster_id, glm::vec3(ox, 0.0f, oz), 1);
                    }
                }
                auto npcs = db->GetNPCPositions(id);
                for (const auto& npc : npcs) {
                    spdlog::info("EngineMap: placing NPC '{}' at ({}, {}, {})", npc.name, npc.pos_x, npc.pos_y, npc.pos_z);
                }
            }
        } catch (...) {
            spdlog::warn("EngineMap: invalid map_id for DB spawn: {}", map_id);
        }
    }

    loaded_ = true;
    return true;
}

void EngineMap::InitBGMMap() {
    bgm_map_ = {
        {"1", "BGM_Title"}, {"2", "BGM_AlkerPlains"}, {"51", "BGM_AlkerPlains"},
        {"52", "BGM_BlueMoon"}, {"53", "BGM_AruaCity"}, {"54", "BGM_SnowyMountain"},
        {"61", "BGM_Desert"}, {"62", "BGM_Forest"}, {"71", "BGM_Dungeon"},
        {"72", "BGM_Boss"}, {"81", "BGM_Seaside"}, {"82", "BGM_Island"},
        {"91", "BGM_Castle"}, {"99", "BGM_Event"},
    };
}

std::string EngineMap::GetBGMForMap(const std::string& map_id) const {
    auto it = bgm_map_.find(map_id);
    if (it != bgm_map_.end()) return it->second;
    return "BGM_Map" + map_id;
}

void EngineMap::Unload() {
    if (audio_) audio_->StopBGM();
    loaded_ = false;
    current_map_.clear();
}

void EngineMap::LoadSceneObjects(const std::string& json_path) {
    if (!props_) return;
    std::ifstream sf(json_path);
    if (!sf) { spdlog::info("EngineMap: no scene file {}", json_path); return; }
    
    std::string text((std::istreambuf_iterator<char>(sf)), std::istreambuf_iterator<char>());
    size_t objPos = text.find("\"objects\"");
    if (objPos == std::string::npos) return;
    
    int loaded = 0, skipped_no_obj = 0;
    size_t p = text.find('{', objPos);
    while (p != std::string::npos) {
        auto mPos = text.find("\"model\":", p);
        auto posPos = text.find("\"pos\":", p);
        auto scalePos = text.find("\"scale\":", p);
        if (mPos == std::string::npos || posPos == std::string::npos) break;

        auto q1 = text.find('"', mPos + 8); auto q2 = text.find('"', q1 + 1);
        std::string modelName = text.substr(q1 + 1, q2 - q1 - 1);

        auto b1 = text.find('[', posPos); auto b2 = text.find(']', b1);
        std::string posStr = text.substr(b1 + 1, b2 - b1 - 1);
        float px, py, pz; sscanf(posStr.c_str(), "%f,%f,%f", &px, &py, &pz);

        // Parse per-object scale
        float sx = 1.0f, sy = 1.0f, sz = 1.0f;
        if (scalePos != std::string::npos && scalePos < text.find('}', p)) {
            auto sb1 = text.find('[', scalePos);
            auto sb2 = text.find(']', sb1);
            if (sb1 != std::string::npos && sb2 != std::string::npos) {
                std::string scaleStr = text.substr(sb1 + 1, sb2 - sb1 - 1);
                sscanf(scaleStr.c_str(), "%f,%f,%f", &sx, &sy, &sz);
            }
        }

        std::string modLower = modelName;
        std::transform(modLower.begin(), modLower.end(), modLower.begin(), ::tolower);
        size_t dot = modLower.find_last_of('.');
        if (dot != std::string::npos) modLower = modLower.substr(0, dot);

        // Try .glb first, then .obj, then .MOD with fresh ifstream each time
        auto tryLoad = [&](const std::string& ext) -> bool {
            std::string fname = "assets/models/" + modLower + ext;
            std::ifstream test(fname);
            if (test.good()) {
                test.close();
                int mesh_idx = props_->LoadObj(fname);
                if (mesh_idx >= 0) {
                    float avgScale = (sx + sy + sz) / 3.0f * 0.005f;
                    props_->AddInstance(mesh_idx, {px * 0.0001f, py * 0.0001f, pz * 0.0001f}, avgScale);
                }
                return true;
            }
            return false;
        };

        if (tryLoad(".glb") || tryLoad(".obj") || tryLoad(".mod")) {
            loaded++;
        } else {
            skipped_no_obj++;
        }
        p = text.find('{', b2);
    }
    spdlog::info("EngineMap: loaded {} scene objects ({} skipped)", loaded, skipped_no_obj);
}

void EngineMap::LoadFarmProps(const std::string& farm_dir) {
    if (!props_) return;
    struct PropData { const char* file; float x, z; float scale; };
    PropData props[] = {
        {"01_jakmul03_lv2", 15, 15, 0.5f}, {"01_farm_fence_lv2", -10, -10, 0.8f},
        {"01_farm_stable_lv3", 20, -15, 0.6f}, {"01_farm_house_lv3", -20, 20, 0.7f},
        {"01_farm_warehouse_lv3", 30, 10, 0.6f}, {"01_farm_hill_lv2", 0, 30, 0.5f},
        {"01_farm_mu_lv3", -30, -20, 0.5f}, {"01_farm_produce_lv2", 25, -25, 0.4f},
        {"01_farm_bflower3", -5, 35, 0.3f}, {"01_farm_rflower3", 35, -5, 0.3f},
        {"01_jakmul01_lv2", -25, 25, 0.5f}, {"01_jakmul02_lv1", 10, -35, 0.4f},
        {"01_farm_stable_lv1", -35, -30, 0.6f},
    };
    for (auto& p : props) {
        std::string path = farm_dir + "/" + std::string(p.file);
        std::ifstream test(path + ".glb");
        int mesh_idx = -1;
        if (test.good()) {
            test.close();
            mesh_idx = props_->LoadObj(path + ".glb");
        } else {
            mesh_idx = props_->LoadObj(path + ".obj");
        }
        if (mesh_idx >= 0) {
            props_->AddInstance(mesh_idx, {p.x, 0, p.z}, p.scale);
        }
    }
    spdlog::info("EngineMap: loaded {} farm props", (int)(sizeof(props)/sizeof(props[0])));
}
