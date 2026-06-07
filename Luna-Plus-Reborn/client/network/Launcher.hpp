#pragma once
#include <string>
#include <vector>
#include <cstdint>
#include <functional>

struct PatchEntry {
    std::string relative_path;
    std::string sha256;
    int64_t file_size = 0;
    std::string download_url;
};

struct PatchManifest {
    std::string version;
    std::vector<PatchEntry> files;
};

class Launcher {
public:
    bool Init(const std::string& base_url, const std::string& install_path);
    void Shutdown();
    
    // Manifest management
    bool FetchRemoteManifest();
    bool LoadLocalManifest();
    bool SaveLocalManifest(const std::string& path = "");
    
    // Delta update
    std::vector<PatchEntry> ComputeDelta() const;
    bool DownloadUpdates(const std::vector<PatchEntry>& updates);
    bool VerifyInstallation();
    
    // Status
    int GetTotalFiles() const { return (int)remote_manifest_.files.size(); }
    int GetFilesToUpdate() const { return (int)ComputeDelta().size(); }
    int64_t GetDownloadSize() const;
    float GetProgress() const { return progress_; }
    bool IsUpdating() const { return updating_; }
    const std::string& GetVersion() const { return local_version_; }
    
    // Callbacks
    using ProgressCallback = std::function<void(float pct, const std::string& file)>;
    void SetProgressCallback(ProgressCallback cb) { progress_cb_ = cb; }

private:
    std::string base_url_;
    std::string install_path_;
    std::string local_version_;
    PatchManifest remote_manifest_;
    PatchManifest local_manifest_;
    bool updating_ = false;
    float progress_ = 0;
    ProgressCallback progress_cb_;
    
    std::string Sha256File(const std::string& path) const;
    bool DownloadFile(const std::string& url, const std::string& dest);
};
