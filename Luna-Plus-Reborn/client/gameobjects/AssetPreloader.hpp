#pragma once
#include <string>
#include <vector>
#include <functional>
#include <cstdint>
#include <ctime>

class UIRenderer;
class AudioManager;

class AssetPreloader {
public:
    void Init(UIRenderer* ui, AudioManager* audio);
    void Shutdown();
    
    // Scan and catalog all available assets
    void ScanAssets();
    
    // Preload textures into UI cache
    int PreloadTextures();
    int PreloadInterface();
    int PreloadAudio();
    
    // Stats
    int GetTotalScanned() const { return total_scanned_; }
    int GetTexturesLoaded() const { return textures_loaded_; }
    int GetInterfaceLoaded() const { return interface_loaded_; }
    int GetAudioLoaded() const { return audio_loaded_; }
    
    // Progress
    float GetProgress() const;
    bool IsComplete() const { return complete_; }
    
    using ProgressCallback = std::function<void(const std::string& stage, float progress)>;
    void SetProgressCallback(ProgressCallback cb) { progress_cb_ = cb; }

private:
    UIRenderer* ui_ = nullptr;
    AudioManager* audio_ = nullptr;
    int total_scanned_ = 0;
    int textures_loaded_ = 0;
    int interface_loaded_ = 0;
    int audio_loaded_ = 0;
    int textures_target_ = 0;
    int interface_target_ = 0;
    int audio_target_ = 0;
    bool complete_ = false;
    ProgressCallback progress_cb_;
    
    void ScanDirectory(const std::string& dir, std::vector<std::string>& files, const std::string& ext = "");
    void ReportProgress(const std::string& stage, float progress);
};
