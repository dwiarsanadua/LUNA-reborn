#include "SceneLoader.hpp"
#include <rendering/CharacterRenderer.hpp>
#include <fstream>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <engine/gx_render/VFS.h>
#include <engine/gx_geom/ModelManager.hpp>

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

    if (j.contains("map")) {
        auto& m = j["map"];
        if (m.is_string()) data.map_id = std::stoi(m.get<std::string>());
        else if (m.is_number()) data.map_id = m.get<int>();
    }

    if (j.contains("box_min")) {
        auto& b = j["box_min"];
        data.box_min = {b[0].get<float>(), b[1].get<float>(), b[2].get<float>()};
    }
    if (j.contains("box_max")) {
        auto& b = j["box_max"];
        data.box_max = {b[0].get<float>(), b[1].get<float>(), b[2].get<float>()};
    }

    if (j.contains("objects")) {
        for (auto& obj : j["objects"]) {
            SceneObject so;
            std::string raw = obj["model"].get<std::string>();
            so.model_name = raw;
            auto dot = raw.find_last_of('.');
            if (dot != std::string::npos) so.model_name = raw.substr(0, dot);
            so.is_animated = (raw.find(".chr") != std::string::npos ||
                             raw.find(".CHR") != std::string::npos);

            if (obj.contains("pos") && obj["pos"].is_array() && obj["pos"].size() >= 3) {
                so.position = {obj["pos"][0].get<float>(),
                              obj["pos"][1].get<float>(),
                              obj["pos"][2].get<float>()};
            }

            if (obj.contains("scale") && obj["scale"].is_array() && obj["scale"].size() >= 3) {
                so.scale = {obj["scale"][0].get<float>(),
                           obj["scale"][1].get<float>(),
                           obj["scale"][2].get<float>()};
            }

            if (obj.contains("rot") && obj["rot"].is_array() && obj["rot"].size() >= 4) {
                so.rotation = {obj["rot"][3].get<float>(),
                              obj["rot"][0].get<float>(),
                              obj["rot"][1].get<float>(),
                              obj["rot"][2].get<float>()};
            }

            data.objects.push_back(so);
        }
    }

    if (j.contains("static_model")) data.static_mesh = j["static_model"].get<std::string>();
    if (j.contains("heightfield")) {
        auto& h = j["heightfield"];
        if (h.contains("file")) data.heightmap_file = h["file"].get<std::string>();
    }

    spdlog::info("SceneLoader: loaded {} ({} objects)", path, data.objects.size());
    return data;
}

std::string SceneLoader::ResolveModelPath(const std::string& model_ref) {
    std::string base = model_ref;
    auto dot = base.find_last_of('.');
    if (dot != std::string::npos) base = base.substr(0, dot);

    std::string paths[] = {
        VFS::Resolve("assets_converted/mod_objs/[r]" + base + ".obj"),
        VFS::Resolve("assets_converted/mod_objs/" + base + ".obj"),
        VFS::Resolve("assets/models/" + base + ".obj"),
        VFS::Resolve("assets/models/prop/" + base + ".obj"),
        VFS::Resolve("assets/models/map/" + base + ".obj"),
        VFS::Resolve("assets/models/" + base + ".glb"),
        VFS::Resolve("assets/models/prop/" + base + ".glb"),
        VFS::Resolve("assets/models/map/" + base + ".glb"),
    };

    for (auto& p : paths) {
        std::ifstream test(p);
        if (test.good()) { test.close(); return p; }
    }

    spdlog::warn("SceneLoader: model not found: {} (tried .glb/.obj)", base);
    return "";
}

bool SceneLoader::Instantiate(const SceneData& scene, CharacterRenderer* chars) {
    if (!chars) return false;
    int loaded = 0, failed = 0;
    static uint32_t scene_id_base = 100000; // Offset IDs to avoid collision with players/monsters

    for (auto& obj : scene.objects) {
        std::string model_path = ResolveModelPath(obj.model_name);
        if (model_path.empty()) { failed++; continue; }

        uint32_t id = scene_id_base++;

        // Spawn using CharacterRenderer. It handles .glb models natively.
        // We use the rotation parameter internally if we expand the API, but for now
        // pos and scale are the primary requirements.
        chars->Spawn(id, model_path, obj.position.x * 0.0001f, obj.position.y * 0.0001f, obj.position.z * 0.0001f, 0xffffffff);

        // If we needed scale/rotation, we would add setters to CharacterRenderer.
        // For login map, just getting the models on screen is the priority.

        loaded++;
    }
    spdlog::info("SceneLoader: instantiated {}/{} objects via CharacterRenderer", loaded, loaded + failed);
    return failed == 0;
}


bgfx::VertexBufferHandle SceneLoader::LoadModel(const std::string& name) {
    spdlog::warn("SceneLoader::LoadModel not fully implemented for '{}'", name);
    return BGFX_INVALID_HANDLE;
}
