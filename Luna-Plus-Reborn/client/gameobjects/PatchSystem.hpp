#pragma once
#include <string>
#include <functional>
#include <cstdint>

struct VersionInfo {
    int major = 1;
    int minor = 1;
    int patch = 0;
    std::string ToString() const {
        return std::to_string(major) + "." + std::to_string(minor) + "." + std::to_string(patch);
    }
    bool operator<(const VersionInfo& o) const {
        if (major != o.major) return major < o.major;
        if (minor != o.minor) return minor < o.minor;
        return patch < o.patch;
    }
};

class PatchSystem {
public:
    void Init();
    
    // Version management
    VersionInfo GetCurrentVersion() const { return current_; }
    void SetCurrentVersion(int major, int minor, int patch);
    bool CheckForUpdate(const VersionInfo& latest);
    
    // Patch notes
    void SetPatchNotes(const std::string& notes) { patch_notes_ = notes; }
    std::string GetPatchNotes() const { return patch_notes_; }
    
    // Update simulation
    bool IsUpdateAvailable() const { return update_available_; }
    VersionInfo GetLatestVersion() const { return latest_; }
    float GetDownloadProgress() const { return download_progress_; }
    bool IsDownloading() const { return downloading_; }
    void StartDownload();
    void UpdateDownload(float dt);
    void ApplyUpdate();
    
    // Changelog
    std::string GetChangelog() const;
    
    // Callbacks
    using UpdateCallback = std::function<void(const std::string& status, float progress)>;
    void SetUpdateCallback(UpdateCallback cb) { update_cb_ = cb; }

private:
    VersionInfo current_{1, 1, 0};
    VersionInfo latest_{1, 2, 0};
    bool update_available_ = false;
    bool downloading_ = false;
    float download_progress_ = 0;
    float download_speed_ = 0.05f; // 5% per tick
    std::string patch_notes_;
    UpdateCallback update_cb_;
};
