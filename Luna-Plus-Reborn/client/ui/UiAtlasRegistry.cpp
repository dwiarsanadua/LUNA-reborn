#include "UiAtlasRegistry.hpp"
#include <engine/gx_render/VFS.h>
#include <spdlog/spdlog.h>
#include <fstream>
#include <filesystem>

std::unordered_map<int, UiAtlasEntry> UiAtlasRegistry::entries_;
std::unordered_map<int, TextureInfo> UiAtlasRegistry::loaded_;
bool UiAtlasRegistry::ready_ = false;

static std::string ResolveAtlasFile(const std::string& legacy_path) {
    namespace fs = std::filesystem;
    std::string name = legacy_path;
    auto slash = name.find_last_of("/\\");
    if (slash != std::string::npos) name = name.substr(slash + 1);

    const std::string candidates[] = {
        "assets/textures/" + name,
        "assets/textures/unpacked/image/" + name,
        "assets/interface/2DImage/image/" + name,
    };
    for (const auto& rel : candidates) {
        std::string found = VFS::Find(rel);
        if (!found.empty() && fs::exists(found)) return found;
    }
    std::string base = name;
    auto dot = base.find_last_of('.');
    if (dot != std::string::npos) {
        std::string stem = base.substr(0, dot);
        std::string png = stem + ".png";
        const std::string png_candidates[] = {
            "assets/textures/" + png,
            "assets/textures/unpacked/image/" + png,
        };
        for (const auto& rel : png_candidates) {
            std::string found = VFS::Find(rel);
            if (!found.empty() && fs::exists(found)) return found;
        }
    }
    return {};
}

void UiAtlasRegistry::Init(const std::string& image_path_file) {
    if (ready_) return;
    entries_.clear();
    loaded_.clear();

    std::string path = VFS::Find(image_path_file);
    if (path.empty()) path = image_path_file;

    std::ifstream in(path);
    if (!in.is_open()) {
        spdlog::warn("UiAtlasRegistry: cannot open {}", path);
        ready_ = true;
        return;
    }

    std::string line;
    if (!std::getline(in, line)) {
        ready_ = true;
        return;
    }

    while (std::getline(in, line)) {
        if (line.empty()) continue;
        int id = 0, w = 0, h = 0, extra = 0;
        char legacy[512] = {};
        if (sscanf(line.c_str(), "%d\t%511[^\t]\t%d\t%d\t%d", &id, legacy, &w, &h, &extra) < 2)
            continue;
        if (legacy[0] == '0' && legacy[1] == '\0') continue;

        UiAtlasEntry entry;
        entry.id = id;
        entry.filename = legacy;
        entry.width = w > 0 ? w : 1024;
        entry.height = h > 0 ? h : 1024;
        entry.resolved_path = ResolveAtlasFile(legacy);
        entries_[id] = std::move(entry);
    }

    spdlog::info("UiAtlasRegistry: loaded {} atlas entries", entries_.size());
    ready_ = true;
}

void UiAtlasRegistry::Shutdown() {
    entries_.clear();
    loaded_.clear();
    ready_ = false;
}

bool UiAtlasRegistry::IsReady() { return ready_; }

const UiAtlasEntry* UiAtlasRegistry::GetEntry(int atlas_id) {
    auto it = entries_.find(atlas_id);
    return it != entries_.end() ? &it->second : nullptr;
}

float UiAtlasRegistry::AtlasWidth(int atlas_id) {
    auto* e = GetEntry(atlas_id);
    return e ? (float)e->width : 1024.0f;
}

float UiAtlasRegistry::AtlasHeight(int atlas_id) {
    auto* e = GetEntry(atlas_id);
    return e ? (float)e->height : 1024.0f;
}

TextureInfo UiAtlasRegistry::LoadAtlasTexture(UIRenderer& ui, int atlas_id) {
    auto cached = loaded_.find(atlas_id);
    if (cached != loaded_.end()) return cached->second;

    auto* entry = GetEntry(atlas_id);
    if (!entry || entry->resolved_path.empty()) return {};

    char key[32];
    snprintf(key, sizeof(key), "ui_atlas_%d", atlas_id);
    TextureInfo tex = ui.LoadTexture(key, entry->resolved_path);
    if (bgfx::isValid(tex.handle)) loaded_[atlas_id] = tex;
    return tex;
}

void UiAtlasRegistry::PreloadAtlases(UIRenderer& ui, const int* ids, size_t count) {
    for (size_t i = 0; i < count; ++i) LoadAtlasTexture(ui, ids[i]);
}

void UiAtlasRegistry::PreloadAtlases(UIRenderer& ui, const std::unordered_map<int, int>& usage) {
    for (const auto& [id, _] : usage) LoadAtlasTexture(ui, id);
}
