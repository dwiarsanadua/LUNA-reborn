#pragma once
#include <string>
#include <vector>
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

struct PatchFileEntry {
    std::string path;
    std::string url;
    int64_t size = 0;
    std::string sha256;
};

struct PatchManifest {
    std::string version;
    std::vector<PatchFileEntry> files;
};

class PatchSystem {
public:
    void Init();

    // Version management
    VersionInfo GetCurrentVersion() const { return current_; }
    void SetCurrentVersion(int major, int minor, int patch);

    // Patch notes
    void SetPatchNotes(const std::string& notes) { patch_notes_ = notes; }
    std::string GetPatchNotes() const { return patch_notes_; }

    // Update check
    bool IsUpdateAvailable() const { return update_available_; }
    VersionInfo GetLatestVersion() const { return latest_; }
    float GetDownloadProgress() const { return download_progress_; }
    float GetDownloadSpeed() const { return download_speed_bytes_; }
    int64_t GetDownloadedBytes() const { return downloaded_bytes_; }
    int64_t GetTotalBytes() const { return total_bytes_; }
    bool IsDownloading() const { return downloading_; }
    int GetPendingFiles() const { return static_cast<int>(pending_files_.size()); }
    int GetTotalFiles() const { return total_files_; }
    const std::string& GetCurrentFile() const { return current_file_; }
    const std::string& GetErrorMessage() const { return last_error_; }

    // Server URL
    void SetPatchServer(const std::string& url) { patch_server_url_ = url; }
    const std::string& GetPatchServer() const { return patch_server_url_; }

    // Download operations
    bool CheckForUpdate(const VersionInfo& latest);
    bool FetchManifest();
    void StartDownload();
    void UpdateDownload(float dt);
    void ApplyUpdate();
    void CancelDownload();
    bool VerifyFiles();

    // Changelog
    std::string GetChangelog() const;

    // Callbacks
    using UpdateCallback = std::function<void(const std::string& status, float progress)>;
    void SetUpdateCallback(UpdateCallback cb) { update_cb_ = cb; }

    using FileCallback = std::function<void(const std::string& filename, float file_progress)>;
    void SetFileCallback(FileCallback cb) { file_cb_ = cb; }

private:
    VersionInfo current_{1, 1, 0};
    VersionInfo latest_{1, 2, 0};
    bool update_available_ = false;
    bool downloading_ = false;
    bool cancelled_ = false;
    float download_progress_ = 0;
    float download_speed_bytes_ = 0;
    int64_t downloaded_bytes_ = 0;
    int64_t total_bytes_ = 0;
    int total_files_ = 0;
    std::string patch_notes_;
    std::string patch_server_url_;
    std::string current_file_;
    std::string last_error_;
    PatchManifest manifest_;
    std::vector<PatchFileEntry> pending_files_;
    UpdateCallback update_cb_;
    FileCallback file_cb_;

    bool HttpGet(const std::string& url, std::vector<uint8_t>& response, int64_t max_size = 0);
    bool HttpDownloadFile(const std::string& url, const std::string& dest_path,
                          int64_t expected_size, std::function<void(float)> progress_cb);
    std::string ComputeSHA256(const std::string& path) const;
    bool SHA256Verify(const std::string& path, const std::string& expected) const;
    void ScanInstallDirectory();
};
