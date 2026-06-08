#include "ModelResolver.hpp"
#include <config/Paths.hpp>
#include <engine/gx_render/VFS.h>
#include <algorithm>
#include <cctype>
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

namespace ModelResolver {

std::string NormalizeBaseName(const std::string& model_file) {
    std::string base = model_file;
    auto slash = base.find_last_of("/\\");
    if (slash != std::string::npos) base = base.substr(slash + 1);
    auto dot = base.find_last_of('.');
    if (dot != std::string::npos) base = base.substr(0, dot);
    std::transform(base.begin(), base.end(), base.begin(),
        [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return base;
}

static bool ExistsFile(const std::string& path) {
    return fs::exists(path) && fs::is_regular_file(path);
}

static std::string FirstExisting(const std::vector<std::string>& candidates) {
    for (const auto& c : candidates) {
        if (ExistsFile(c)) return c;
    }
    return {};
}

std::string ResolveCharacterModel(const std::string& model_file) {
    const std::string base = NormalizeBaseName(model_file);
    std::string chr_name = base;
    auto us = chr_name.find_last_of('_');
    if (us != std::string::npos && us > 2) {
        std::string suffix = chr_name.substr(us + 1);
        if (suffix.size() == 1 && suffix[0] >= '0' && suffix[0] <= '9')
            chr_name = chr_name.substr(0, us);
    }

    std::string chr_path = VFS::Find("assets/characters/" + chr_name + ".json");
    if (ExistsFile(chr_path)) {
        std::ifstream cf(chr_path);
        std::string json((std::istreambuf_iterator<char>(cf)), {});
        auto mpos = json.find("\"file\":");
        if (mpos != std::string::npos) {
            auto q1 = json.find('"', mpos + 7);
            auto q2 = json.find('"', q1 + 1);
            if (q1 != std::string::npos && q2 != std::string::npos) {
                return ResolveMonsterModel(json.substr(q1 + 1, q2 - q1 - 1));
            }
        }
    }
    return ResolveMonsterModel(base);
}

std::string ResolveMonsterModel(const std::string& model_file) {
    if (model_file.empty()) return {};

    if (model_file.find(".glb") != std::string::npos || model_file.find(".obj") != std::string::npos) {
        std::string direct = VFS::Find(model_file);
        if (ExistsFile(direct)) return direct;
    }

    const std::string base = NormalizeBaseName(model_file);
    const std::string ext = model_file.size() > 4 ? model_file.substr(model_file.size() - 4) : "";
    if (ext == ".chx" || ext == ".CHX") {
        auto resolved = ResolveCharacterModel(model_file);
        if (!resolved.empty()) return resolved;
    }

    const char* folders[] = {"monster/", "npc/", "character/", "map/", "effect/", "farm/", "housing/", ""};
    for (const char* folder : folders) {
        std::string prefix = Paths::Asset("models/") + folder;
        auto found = FirstExisting({
            prefix + base + ".glb",
            prefix + base + ".obj",
        });
        if (!found.empty()) return found;
    }

    return FirstExisting({
        Paths::Asset("models/monster/" + base + ".glb"),
        VFS::Find("assets/models/monster/" + base + ".glb"),
        Paths::Asset("models/" + base + ".glb"),
    });
}

}
