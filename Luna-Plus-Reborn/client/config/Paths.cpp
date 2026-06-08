#include "Paths.hpp"
#include <engine/gx_render/VFS.h>
#include <spdlog/spdlog.h>
#include <filesystem>

#ifndef ASSETS_PATH
#define ASSETS_PATH "./assets/"
#endif
#ifndef GAME_DATA_PATH
#define GAME_DATA_PATH "./assets/data/game_data.db"
#endif

std::string Paths::assets_root_ = ASSETS_PATH;
std::string Paths::game_data_db_ = GAME_DATA_PATH;
bool Paths::initialized_ = false;

static std::string PickGameDataDb(const std::string& root) {
    const char* candidates[] = {
        "assets/data/archive_game_data.db",
        "assets/data/game_data.db",
        "assets/data/luna_game.db",
    };
    for (const char* rel : candidates) {
        std::string path = root + rel;
        if (std::filesystem::exists(path) && std::filesystem::file_size(path) > 4096)
            return path;
    }
    return root + "assets/data/game_data.db";
}

void Paths::Init() {
    const char* asset_markers[] = {
        "assets/maps/51.hgt",
        "assets/data/game_data.db",
        "assets/textures/ui/Launcher/Launcher_01_01.png",
    };

    for (const auto& root : VFS::GetSearchRoots()) {
        for (const char* marker : asset_markers) {
            std::string full = root + marker;
            if (std::filesystem::exists(full)) {
                assets_root_ = root + "assets/";
                game_data_db_ = PickGameDataDb(root);
                VFS::Init(root);
                initialized_ = true;
                spdlog::info("Paths: assets root = {}", assets_root_);
                return;
            }
        }
    }

    if (!initialized_) {
        assets_root_ = ASSETS_PATH;
        game_data_db_ = PickGameDataDb("");
        if (!std::filesystem::exists(game_data_db_) || std::filesystem::file_size(game_data_db_) < 4096)
            game_data_db_ = GAME_DATA_PATH;
        initialized_ = true;
        spdlog::warn("Paths: using compile-time fallback assets={} db={}", assets_root_, game_data_db_);
    }
}

const std::string& Paths::Assets() {
    if (!initialized_) Init();
    return assets_root_;
}

const std::string& Paths::GameDataDb() {
    if (!initialized_) Init();
    return game_data_db_;
}

std::string Paths::Asset(const std::string& relative) {
    return Assets() + relative;
}

std::string Paths::FindAsset(const std::string& relative) {
    if (!initialized_) Init();
    return VFS::Find("assets/" + relative);
}
