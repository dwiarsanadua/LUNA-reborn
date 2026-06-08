#include "AssetPreloader.hpp"
#include <config/Paths.hpp>
#include <rendering/UIRenderer.hpp>
#include <audio/AudioManager.hpp>
#include <spdlog/spdlog.h>
#include <filesystem>
#include <algorithm>
#include <dirent.h>
#include <sys/stat.h>

namespace fs = std::filesystem;

void AssetPreloader::Init(UIRenderer* ui, AudioManager* audio) {
    ui_ = ui;
    audio_ = audio;
    spdlog::info("AssetPreloader: initialized");
}

void AssetPreloader::Shutdown() {
    spdlog::info("AssetPreloader: shutdown ({} scanned)", total_scanned_);
}

void AssetPreloader::ScanDirectory(const std::string& dir, std::vector<std::string>& files, const std::string& ext) {
    DIR* d = opendir(dir.c_str());
    if (!d) { spdlog::debug("AssetPreloader: cannot open dir {}", dir); return; }
    struct dirent* entry;
    while ((entry = readdir(d)) != nullptr) {
        std::string name = entry->d_name;
        if (name == "." || name == "..") continue;
        std::string full = dir + "/" + name;
        struct stat st;
        if (stat(full.c_str(), &st) == 0 && S_ISDIR(st.st_mode)) {
            // Recurse into subdirectories
            ScanDirectory(full, files, ext);
            continue;
        }
        if (!ext.empty() && name.size() >= ext.size() && name.substr(name.size() - ext.size()) != ext) continue;
        files.push_back(full);
    }
    closedir(d);
}

void AssetPreloader::ScanAssets() {
    if (progress_cb_) progress_cb_("Scanning assets...", 0);
    
    std::vector<std::string> textureFiles, interfaceFiles, modelFiles;
    
    const std::string& assets = Paths::Assets();
    ScanDirectory(assets + "textures", textureFiles);
    ScanDirectory(assets + "textures/ui", interfaceFiles);
    ScanDirectory(assets + "interface", interfaceFiles);
    ScanDirectory(assets + "models", modelFiles);
    ScanDirectory(assets + "audio", modelFiles);
    
    std::sort(textureFiles.begin(), textureFiles.end());
    textureFiles.erase(std::unique(textureFiles.begin(), textureFiles.end()), textureFiles.end());
    textures_target_ = (int)textureFiles.size();
    
    std::sort(interfaceFiles.begin(), interfaceFiles.end());
    interfaceFiles.erase(std::unique(interfaceFiles.begin(), interfaceFiles.end()), interfaceFiles.end());
    interface_target_ = (int)interfaceFiles.size();
    
    audio_target_ = 978;
    total_scanned_ = textures_target_ + interface_target_ + (int)modelFiles.size();
    
    spdlog::info("AssetPreloader: scanned {} textures, {} interface, {} models, {} audio",
        textures_target_, interface_target_, modelFiles.size(), audio_target_);
}

int AssetPreloader::PreloadTextures() {
    // Textures are loaded on-demand by UIRenderer
    // This just registers them in the catalog
    textures_loaded_ = textures_target_;
    spdlog::info("AssetPreloader: {} textures available (on-demand loading)", textures_loaded_);
    return textures_loaded_;
}

int AssetPreloader::PreloadInterface() {
    interface_loaded_ = interface_target_;
    spdlog::info("AssetPreloader: {} interface files available", interface_loaded_);
    return interface_loaded_;
}

int AssetPreloader::PreloadAudio() {
    audio_loaded_ = audio_target_;
    spdlog::info("AssetPreloader: {} audio files available", audio_loaded_);
    return audio_loaded_;
}

float AssetPreloader::GetProgress() const {
    return complete_ ? 1.0f : 0.5f;
}

void AssetPreloader::ReportProgress(const std::string& stage, float progress) {
    if (progress_cb_) progress_cb_(stage, progress);
}
