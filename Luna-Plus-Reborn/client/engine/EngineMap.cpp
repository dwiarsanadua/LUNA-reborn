#include "EngineMap.hpp"
#include "SceneLoader.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <cfloat>
#include <spdlog/spdlog.h>
#include <engine/gx_render/VFS.h>
#include <engine/physics/PhysicsWorld.h>
#include <audio/AudioManager.hpp>
#include <game/ecs/systems/GameDataDB.hpp>
#include <game/ecs/systems/SpawnSystem.hpp>
#include <game/ecs/components/Tag.hpp>
#include <game/ecs/components/Transform.hpp>
#include <entt/entt.hpp>
#include <rendering/TerrainRenderer.hpp>

static const float COLLISION_CELL_SIZE = 2.0f;
static const float MAX_CLIMB_HEIGHT = 1.5f;

// ── CollisionLine ──
// DDA ray-march across terrain height grid. Returns TRUE if collision detected.
// pTarget receives the first collision point.
bool EngineMap::CollisionLine(const TerrainRenderer* terrain,
                               const glm::vec3& start, const glm::vec3& end,
                               glm::vec3* out_target)
{
    if (!terrain) return false;

    int x1 = static_cast<int>(start.x / COLLISION_CELL_SIZE);
    int z1 = static_cast<int>(start.z / COLLISION_CELL_SIZE);
    int x2 = static_cast<int>(end.x / COLLISION_CELL_SIZE);
    int z2 = static_cast<int>(end.z / COLLISION_CELL_SIZE);

    int dx = x2 - x1;
    int dz = z2 - z1;
    int abs_dx = std::abs(dx);
    int abs_dz = std::abs(dz);
    int max_delta = std::max(abs_dx, abs_dz);
    if (max_delta == 0) {
        if (out_target) *out_target = end;
        return false;
    }

    int cell_x = x1, cell_z = z1;
    int x_err = 0, z_err = 0;
    int sx = (dx > 0) ? 1 : (dx < 0) ? -1 : 0;
    int sz = (dz > 0) ? 1 : (dz < 0) ? -1 : 0;
    int prev_x = cell_x, prev_z = cell_z;
    float prev_height = terrain->GetHeight(start.x, start.z);

    for (int i = 0; i <= max_delta; ++i) {
        prev_x = cell_x; prev_z = cell_z;

        x_err += abs_dx;
        z_err += abs_dz;

        if (x_err > max_delta) { x_err -= max_delta; cell_x += sx; }
        if (z_err > max_delta) { z_err -= max_delta; cell_z += sz; }

        float cx = cell_x * COLLISION_CELL_SIZE;
        float cz = cell_z * COLLISION_CELL_SIZE;
        float h = terrain->GetHeight(cx, cz);

        // Check slope (height difference between consecutive cells)
        if (std::abs(h - prev_height) > MAX_CLIMB_HEIGHT) {
            if (out_target) {
                out_target->x = prev_x * COLLISION_CELL_SIZE;
                out_target->y = prev_height;
                out_target->z = prev_z * COLLISION_CELL_SIZE;
            }
            return true;
        }

        // Check under-map (below terrain surface)
        float start_h = terrain->GetHeight(
            start.x + (end.x - start.x) * (i / static_cast<float>(max_delta)),
            start.z + (end.z - start.z) * (i / static_cast<float>(max_delta)));
        if (std::abs(h - start_h) > MAX_CLIMB_HEIGHT * 3.0f) {
            if (out_target) {
                out_target->x = prev_x * COLLISION_CELL_SIZE;
                out_target->y = h;
                out_target->z = prev_z * COLLISION_CELL_SIZE;
            }
            return true;
        }

        prev_height = h;
    }

    if (out_target) *out_target = end;
    return false;
}

// ── Map change rollback state ──
struct RollbackPoint {
    std::string map_id;
    glm::vec3 position;
    bool valid = false;
};
static RollbackPoint g_rollback;

static void SaveRollbackPoint(const std::string& map_id, const glm::vec3& pos) {
    g_rollback.map_id = map_id;
    g_rollback.position = pos;
    g_rollback.valid = true;
    spdlog::info("EngineMap: saved rollback point {} at ({:.1f}, {:.1f}, {:.1f})",
                 map_id, pos.x, pos.y, pos.z);
}

static bool GetRollbackPoint(std::string& out_map, glm::vec3& out_pos) {
    if (!g_rollback.valid) return false;
    out_map = g_rollback.map_id;
    out_pos = g_rollback.position;
    g_rollback.valid = false;
    return true;
}

bool EngineMap::Load(const std::string& map_id) {
    // Save rollback point before attempting map change
    std::string prev_map = current_map_;
    glm::vec3 prev_pos(0);
    if (!prev_map.empty()) {
        SaveRollbackPoint(prev_map, prev_pos);
    }

    if (!terrain_ || !props_) {
        spdlog::error("EngineMap: terrain/props not set");
        if (!prev_map.empty()) {
            current_map_ = prev_map;
            spdlog::info("EngineMap: rolled back to previous map {}", prev_map);
        }
        return false;
    }
    InitBGMMap();
    
    std::string hgt_path = VFS::Find("assets/maps/" + map_id + ".hgt");
    if (!terrain_->LoadFromHGT(hgt_path.c_str(), 0.04f)) {  // tile=400, object_factor=0.0001, 400*0.0001=0.04
        terrain_->Init(100, 12.0f);
        spdlog::info("EngineMap: procedural terrain for map {}", map_id);
    }
    
    std::string json_path = VFS::Find("assets/maps/" + map_id + ".json");
    LoadSceneObjects(json_path);
    
    // SceneLoader: additional scene object loading with proper JSON parsing
    auto scene = SceneLoader::Load(json_path);
    SceneLoader::Instantiate(scene);
    
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

    // Load spawn data (flat spawns.json preferred, monsters.json fallback)
    if (spawn_sys_) {
        std::string spawn_path = VFS::Find("assets/data/spawns.json");
        if (!std::ifstream(spawn_path).good())
            spawn_path = VFS::Find("assets/data/monsters.json");
        spawn_sys_->LoadSpawnData(spawn_path);
        try {
            int mid = std::stoi(map_id);
            spawn_sys_->SpawnMonstersForMap(*registry_, mid);
        } catch (...) {
            spdlog::warn("EngineMap: invalid map_id for SpawnSystem: {}", map_id);
        }
    }

    // Spawn monsters and NPCs from GameDataDB
    if (spawn_sys_ && registry_) {
        try {
            uint32_t id = static_cast<uint32_t>(std::stoul(map_id));
            // Use external DB if provided, otherwise open internally
            GameDataDB* db = gamedb_;
            GameDataDB local_db;
            if (!db) {
                if (local_db.Open(VFS::Find("assets/data/game_data.db").c_str())) {
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
                    auto npc_entity = registry_->create();
                    registry_->emplace<TagNPC>(npc_entity);
                    registry_->emplace<Transform>(npc_entity,
                        glm::vec3(npc.pos_x, npc.pos_y, npc.pos_z));
                    spdlog::info("EngineMap: placed NPC '{}' (id={}) at ({}, {}, {})",
                        npc.name, npc.npc_id, npc.pos_x, npc.pos_y, npc.pos_z);
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
        {"01", "BGM_Login"}, {"01_loginpage_01", "BGM_Login"},
        {"13", "13_Moon_Blind_Swamp"}, {"14", "14_Red_Orc_Outpost"},
        {"15", "15_Moon_Blind_Forest"}, {"16", "16_17_Haunted_Mine"},
        {"17", "16_17_Haunted_Mine"}, {"19", "19_The_Gate_Of_Alker"},
        {"20", "20_Alker_Harbor"}, {"21", "21_Ruins_Of_Draconian"},
        {"22", "22_Zakandia"}, {"23", "23_Tarintus"},
        {"25", "25_MontBlanc_Port"}, {"26", "26_Dried_Gazell_Fall"},
        {"27", "27_Zakandia_Outpost"}, {"28", "28_The_Dark_Portal"},
        {"29", "29_Distorted_Crevice"}, {"31", "31_The_Way_To_The_Howling_Ravine"},
        {"32", "32_Howling_Ravine"}, {"33", "33_34_Howling_Cave"},
        {"34", "33_34_Howling_Cave"}, {"41", "41_Ghost_Tree_Swamp"},
        {"42", "42_Lair_Of_Kierra"}, {"51", "02_03_Alker_Plains"},
        {"52", "52_The_Town_Of_Nera_Castle"}, {"53", "53_The_Great_Garden"},
        {"54", "54_The_Knights_Grave"}, {"55", "55_Harbor_Of_Nera"},
        {"56", "56_Temple_Of_Greed"}, {"57", "57_Broken_Shipwrecked"},
        {"60", "60_The_Tower_Of_Wizardly_Low"}, {"63", "63_The_Death_Tower"},
        {"64", "64_The_Spirit_Forest"}, {"65", "65_crystal_empire"},
        {"71", "71_Sahel_Zone"}, {"72", "72_Mirage_In_The_Parasus"},
        {"73", "73_Ruins_Of_Ancient_Elpis"}, {"74", "74_Dragonians_Tomb1"},
        {"75", "75_Dragonians_Tomb2"}, {"81", "81_83_Siege_Castle"},
        {"82", "82_The_Culvert"}, {"83", "81_83_Siege_Castle"},
        {"84", "84_Advanced_Dungeon"}, {"91", "91_92_93_Challenge_Zone"},
        {"92", "91_92_93_Challenge_Zone"}, {"93", "91_92_93_Challenge_Zone"},
        {"96", "96_Desolated_Gorge"}, {"97", "97_Challenge_Zone"},
        {"99", "99_Alker_Residence"},
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
        std::transform(modLower.begin(), modLower.end(), modLower.begin(),
            [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
        size_t dot = modLower.find_last_of('.');
        if (dot != std::string::npos) modLower = modLower.substr(0, dot);

        // Try .glb first, then .obj, then .mod
        auto tryLoad = [&](const std::string& ext) -> bool {
            std::string fname = VFS::Resolve("assets/models/" + modLower + ext);
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

        // Try .chr — character definition files
        auto tryLoadChr = [&](const std::string& orig_name, const std::string& base_name, glm::vec3 pos, float scale) -> bool {
            std::string lower = orig_name;
            std::transform(lower.begin(), lower.end(), lower.begin(),
                [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
            if (lower.find(".chr") == std::string::npos) return false;
            std::string chr_name = base_name;
            auto us = chr_name.find_last_of('_');
            if (us != std::string::npos && us > 2) {
                std::string suffix = chr_name.substr(us + 1);
                if (suffix.size() == 1 && suffix[0] >= '0' && suffix[0] <= '9')
                    chr_name = chr_name.substr(0, us);
            }
            std::string chr_path = VFS::Resolve("assets/characters/" + chr_name + ".json");
            std::ifstream cf(chr_path);
            if (!cf.good()) return false;
            std::string json((std::istreambuf_iterator<char>(cf)), {});
            cf.close();
            auto mpos = json.find("\"file\":");
            if (mpos == std::string::npos) return false;
            auto q1 = json.find('"', mpos + 7);
            auto q2 = json.find('"', q1 + 1);
            if (q1 == std::string::npos || q2 == std::string::npos) return false;
            std::string model_path = json.substr(q1 + 1, q2 - q1 - 1);
            auto slash = model_path.find_last_of('/');
            std::string model_file = (slash != std::string::npos) ? model_path.substr(slash + 1) : model_path;
            auto dot2 = model_file.find_last_of('.');
            if (dot2 != std::string::npos) model_file = model_file.substr(0, dot2);
            std::string fname = VFS::Resolve("assets/models/" + model_file + ".glb");
            std::ifstream mtest(fname);
            if (!mtest.good()) { fname = VFS::Resolve("assets/models/" + model_file + ".obj"); mtest.open(fname); }
            if (!mtest.good()) return false;
            mtest.close();
            int mesh_idx = props_->LoadObj(fname);
            if (mesh_idx < 0) return false;
            props_->AddInstance(mesh_idx, pos, scale);
            return true;
        };

        if (tryLoad(".glb") || tryLoad(".obj") || tryLoad(".mod")) {
            loaded++;
        } else if (tryLoadChr(modelName, modLower, {px * 0.0001f, py * 0.0001f, pz * 0.0001f}, (sx + sy + sz) / 3.0f * 0.005f)) {
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

bool EngineMap::TryMove(glm::vec3 from, glm::vec3 to, float radius, entt::entity entity) {
    if (CollisionLine(terrain_, from, to, nullptr)) {
        glm::vec3 slide = CalculateSlide(from, to);
        if (!CollisionLine(terrain_, from, slide, nullptr)) {
            if (physics_) {
                glm::vec3 dir = glm::normalize(slide - from);
                float dist = glm::distance(from, slide);
                glm::vec3 vel = dir * dist;
                MovingEllipsoid ellipsoid;
                ellipsoid.from = from;
                ellipsoid.width = radius;
                ellipsoid.height = radius * 2.0f;
                ellipsoid.velocity = vel;
                glm::vec3 tri[3] = {
                    {to.x - radius, -10.0f, to.z - radius},
                    {to.x + radius, -10.0f, to.z - radius},
                    {to.x, 200.0f, to.z + radius},
                };
                CollisionResult result;
                if (physics_->CollisionTestMovingEllipsoidMeetTriangle(result, ellipsoid, tri)) {
                    return false;
                }
            }
            SetEntityPosition(entity, slide);
            return true;
        }
        return false;
    }

    if (physics_ && radius > 0.0f) {
        glm::vec3 dir = glm::normalize(to - from);
        float dist = glm::distance(from, to);
        glm::vec3 vel = dir * dist;
        MovingEllipsoid ellipsoid;
        ellipsoid.from = from;
        ellipsoid.width = radius;
        ellipsoid.height = radius * 2.0f;
        ellipsoid.velocity = vel;
        glm::vec3 tri[3] = {
            {to.x - radius, -10.0f, to.z - radius},
            {to.x + radius, -10.0f, to.z - radius},
            {to.x, 200.0f, to.z + radius},
        };
        CollisionResult result;
        if (physics_->CollisionTestMovingEllipsoidMeetTriangle(result, ellipsoid, tri)) {
            return false;
        }
    }

    MapBounds bounds = GetMapBounds();
    if (to.x < bounds.min_x || to.x > bounds.max_x ||
        to.z < bounds.min_z || to.z > bounds.max_z) {
        return false;
    }

    SetEntityPosition(entity, to);
    return true;
}

glm::vec3 EngineMap::CalculateSlide(const glm::vec3& from, const glm::vec3& to) const {
    glm::vec3 dir = glm::normalize(to - from);
    float dist = glm::distance(from, to);
    if (dist < 0.001f) return from;
    return from + dir * dist * 0.5f;
}

MapBounds EngineMap::GetMapBounds() const {
    MapBounds bounds;
    if (current_map_ == "13") {
        bounds = {-50.0f, 50.0f, -50.0f, 50.0f};
    } else if (current_map_ == "20") {
        bounds = {-60.0f, 60.0f, -60.0f, 60.0f};
    } else if (current_map_ == "51") {
        bounds = {-80.0f, 80.0f, -80.0f, 80.0f};
    } else {
        bounds = {-500.0f, 500.0f, -500.0f, 500.0f};
    }
    return bounds;
}

void EngineMap::SetEntityPosition(entt::entity entity, const glm::vec3& pos) {
    if (!registry_ || !registry_->valid(entity)) return;
    if (auto* xform = registry_->try_get<Transform>(entity)) {
        xform->position = pos;
    }
    if (physics_) {
        physics_->SetCharacterPosition(static_cast<int>(entt::to_entity(entity)), pos);
    }
}
