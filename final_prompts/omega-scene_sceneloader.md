# OMEGA-SCENE — SceneLoader: Implement Loader untuk scene.json

Lokasi: /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn
Build: cmake --build build/macos-debug -j$(sysctl -n hw.ncpu)

## Tugas

Buat SceneLoader yang membaca `assets/maps/{map_id}.json` dan menempatkan model (.glb) ke world. Saat ini PropRenderer, TerrainRenderer, WorldRenderer sudah ada tapi tidak ada yang membaca scene.json. Tanpa ini, map hanya berisi terrain kosong.

## Aturan Ketat

1. ✅ BACA dulu `PropRenderer.hpp`, `TerrainRenderer.hpp`, `WorldRenderer.hpp` — pahami API yang sudah ada
2. ✅ BACA `engine/gx_geom/Model.hpp` — pahami cara load .glb
3. 🔧 BUAT file BARU `client/engine/SceneLoader.hpp` + `SceneLoader.cpp`
4. 🔧 UPDATE `WorldRenderer` atau `EngineMap` untuk panggil SceneLoader
5. JANGAN ubah renderer — cukup load data dan feed ke renderer
6. Build verify — 0 error

## File Target (tidak conflict dengan agent lain)

- `client/engine/SceneLoader.hpp` — BARU
- `client/engine/SceneLoader.cpp` — BARU
- `client/engine/EngineMap.hpp` atau `client/rendering/WorldRenderer.hpp` — UPDATE (tambah panggilan)

## Spesifikasi

### SceneLoader.hpp

```cpp
#pragma once
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <bgfx/bgfx.h>

struct SceneObject {
    std::string model_name;   // "01_farm_fishpost" (tanpa .MOD/.chr)
    glm::vec3 position{0};
    glm::vec3 scale{1,1,1};
    glm::quat rotation{1,0,0,0};
    bool is_animated = false; // .chr = animated, .MOD = static
};

struct SceneData {
    int map_id = 0;
    glm::vec3 box_min{0};
    glm::vec3 box_max{0};
    std::vector<SceneObject> objects;
    // Terrain: heightmap file
    std::string heightmap_file;
    // Static mesh (optional — .stm legacy, bisa skip)
    std::string static_mesh;
};

class SceneLoader {
public:
    // Load scene.json → SceneData
    static SceneData Load(const std::string& path);
    
    // Instantiate all objects in the scene ke world renderer
    static bool Instantiate(SceneData& scene);
    
    // Helper: cari dan load model .glb
    static bgfx::VertexBufferHandle LoadModel(const std::string& name);
    
private:
    // Resolve nama model: "01_farm_fishpost.MOD" → cari .glb
    static std::string ResolveModelPath(const std::string& model_ref);
};
```

### SceneLoader.cpp — Implementasi

```cpp
#include "SceneLoader.hpp"
#include <fstream>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <engine/gx_render/VFS.h>

using json = nlohmann::json;

SceneData SceneLoader::Load(const std::string& path) {
    SceneData data;
    std::ifstream f(path);
    if (!f.is_open()) {
        spdlog::error("SceneLoader: cannot open {}", path);
        return data;
    }
    
    json j;
    try { f >> j; } catch (...) {
        spdlog::error("SceneLoader: JSON parse error in {}", path);
        return data;
    }
    
    // Parse map ID dari filename atau field
    if (j.contains("map")) data.map_id = j["map"].get<int>();
    
    // Parse bounds
    if (j.contains("box_min")) {
        auto& b = j["box_min"];
        data.box_min = {b[0].get<float>(), b[1].get<float>(), b[2].get<float>()};
    }
    if (j.contains("box_max")) {
        auto& b = j["box_max"];
        data.box_max = {b[0].get<float>(), b[1].get<float>(), b[2].get<float>()};
    }
    
    // Parse objects
    if (j.contains("objects")) {
        for (auto& obj : j["objects"]) {
            SceneObject so;
            // Convert "01_farm_fishpost.MOD" → "01_farm_fishpost"
            std::string raw = obj["model"].get<std::string>();
            so.model_name = raw;
            // Remove extension for loading
            auto dot = raw.find_last_of('.');
            if (dot != std::string::npos) so.model_name = raw.substr(0, dot);
            so.is_animated = (raw.find(".chr") != std::string::npos || 
                             raw.find(".CHR") != std::string::npos);
            
            // Position
            if (obj.contains("pos") && obj["pos"].is_array() && obj["pos"].size() >= 3) {
                so.position = {obj["pos"][0].get<float>(), 
                              obj["pos"][1].get<float>(), 
                              obj["pos"][2].get<float>()};
            }
            
            // Scale
            if (obj.contains("scale") && obj["scale"].is_array() && obj["scale"].size() >= 3) {
                so.scale = {obj["scale"][0].get<float>(), 
                           obj["scale"][1].get<float>(), 
                           obj["scale"][2].get<float>()};
            }
            
            // Rotation (quaternion: x, y, z, w)
            if (obj.contains("rot") && obj["rot"].is_array() && obj["rot"].size() >= 4) {
                so.rotation = {obj["rot"][3].get<float>(),  // glm quat: w, x, y, z
                              obj["rot"][0].get<float>(),
                              obj["rot"][1].get<float>(),
                              obj["rot"][2].get<float>()};
            }
            
            data.objects.push_back(so);
        }
    }
    
    // Static mesh & heightmap
    if (j.contains("static_model")) data.static_mesh = j["static_model"].get<std::string>();
    
    spdlog::info("SceneLoader: loaded {} ({} objects, {} static)",
                 path, data.objects.size(), data.static_mesh.empty() ? "none" : data.static_mesh);
    return data;
}

std::string SceneLoader::ResolveModelPath(const std::string& model_ref) {
    // Hapus .MOD/.chr/.stm extension, cari .glb
    std::string base = model_ref;
    auto dot = base.find_last_of('.');
    if (dot != std::string::npos) base = base.substr(0, dot);
    
    // Cari di assets/models/
    std::string paths[] = {
        VFS::Resolve("assets/models/" + base + ".glb"),
        VFS::Resolve("assets/models/prop/" + base + ".glb"),
        VFS::Resolve("assets/models/character/" + base + ".glb"),
        VFS::Resolve("assets/models/map/" + base + ".glb"),
    };
    
    for (auto& p : paths) {
        if (std::filesystem::exists(p)) return p;
    }
    
    spdlog::warn("SceneLoader: model not found: {} (tried .glb)", base);
    return "";
}

bool SceneLoader::Instantiate(SceneData& scene) {
    int loaded = 0, failed = 0;
    for (auto& obj : scene.objects) {
        std::string glb_path = ResolveModelPath(obj.model_name);
        if (glb_path.empty()) { failed++; continue; }
        
        // Load GLB + add to renderer
        // Panggil API yang ada di PropRenderer atau WorldRenderer
        // PropRenderer::AddInstance(name, position, rotation, scale)
        // Atau langsung via Assimp + bgfx
        
        loaded++;
    }
    spdlog::info("SceneLoader: instantiated {}/{} objects", loaded, loaded + failed);
    return failed == 0;
}
```

### Integrasi

Di `WorldRenderer.cpp` atau `EngineMap.cpp`, tambah panggilan:
```cpp
// Saat load map:
std::string scene_path = "assets/maps/" + std::to_string(map_id) + ".json";
auto scene = SceneLoader::Load(scene_path);
SceneLoader::Instantiate(scene);
```

## ✅ Kembalikan: "OMEGA-SCENE done: SceneLoader created — loads .json + instantiates .glb, 0 errors"
