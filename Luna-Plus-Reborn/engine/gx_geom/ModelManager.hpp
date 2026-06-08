#pragma once
#include "Model.h"
#include <string>
#include <unordered_map>
#include <memory>

class ModelManager {
public:
    Model* Load(const std::string& name, const std::string& path);
    Model* Get(const std::string& name);
    bool IsLoaded(const std::string& name) const;
    void Unload(const std::string& name);
    void UnloadAll();

    static std::string ResolveModelPath(const std::string& name);

private:
    std::unordered_map<std::string, std::unique_ptr<Model>> models_;
};
