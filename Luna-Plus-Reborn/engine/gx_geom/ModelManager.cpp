#include "ModelManager.hpp"
#include <engine/gx_render/VFS.h>
#include <spdlog/spdlog.h>

Model* ModelManager::Load(const std::string& name, const std::string& path) {
    auto it = models_.find(name);
    if (it != models_.end()) {
        return it->second.get();
    }

    std::string resolved = ResolveModelPath(path);
    if (resolved.empty()) {
        resolved = path;
    }

    auto model = std::make_unique<Model>();
    bool loaded = false;
    if (resolved.size() >= 4) {
        std::string ext = resolved.substr(resolved.size() - 4);
        if (ext == ".glb" || ext == ".GLB") {
            loaded = model->LoadFromGLB(resolved);
        } else {
            loaded = model->LoadFromOBJ(resolved);
        }
    } else {
        loaded = model->LoadFromOBJ(resolved);
    }

    if (!loaded) {
        spdlog::error("ModelManager: failed to load '{}' from '{}'", name, resolved);
        return nullptr;
    }

    Model* ptr = model.get();
    models_[name] = std::move(model);
    spdlog::debug("ModelManager: loaded '{}' from '{}'", name, resolved);
    return ptr;
}

Model* ModelManager::Get(const std::string& name) {
    auto it = models_.find(name);
    return (it != models_.end()) ? it->second.get() : nullptr;
}

bool ModelManager::IsLoaded(const std::string& name) const {
    return models_.find(name) != models_.end();
}

void ModelManager::Unload(const std::string& name) {
    models_.erase(name);
}

void ModelManager::UnloadAll() {
    models_.clear();
    spdlog::debug("ModelManager: all models unloaded");
}

std::string ModelManager::ResolveModelPath(const std::string& name) {
    std::string found = VFS::Find(name);
    if (!found.empty()) return found;
    return name;
}
