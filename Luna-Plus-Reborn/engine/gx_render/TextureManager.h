#pragma once
#include "Texture.h"
#include <string>
#include <unordered_map>
#include <memory>

class TextureManager {
public:
    Texture* Load(const std::string& name, const std::string& path);
    Texture* Get(const std::string& name);
    void Unload(const std::string& name);
    void UnloadAll();

private:
    std::unordered_map<std::string, std::unique_ptr<Texture>> textures_;
};
