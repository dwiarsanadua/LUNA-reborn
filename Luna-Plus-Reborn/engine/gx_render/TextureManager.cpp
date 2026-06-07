#include "TextureManager.h"
#include <spdlog/spdlog.h>

Texture* TextureManager::Load(const std::string& name, const std::string& path) {
    auto it = textures_.find(name);
    if (it != textures_.end()) {
        return it->second.get();
    }

    auto tex = std::make_unique<Texture>();
    if (!tex->LoadFromFile(path)) {
        spdlog::error("TextureManager: failed to load '{}' from '{}'", name, path);
        return nullptr;
    }

    Texture* ptr = tex.get();
    textures_[name] = std::move(tex);
    spdlog::debug("TextureManager: loaded '{}' from '{}'", name, path);
    return ptr;
}

Texture* TextureManager::Get(const std::string& name) {
    auto it = textures_.find(name);
    return (it != textures_.end()) ? it->second.get() : nullptr;
}

void TextureManager::Unload(const std::string& name) {
    textures_.erase(name);
}

void TextureManager::UnloadAll() {
    textures_.clear();
    spdlog::debug("TextureManager: all textures unloaded");
}
