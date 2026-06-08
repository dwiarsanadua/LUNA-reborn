#pragma once
#include <string>
#include <vector>
#include <cstdint>
#include <functional>
#include <map>

struct PatchEntry {
    std::string relative_path;
    std::string sha256;
    int64_t file_size = 0;
    std::string download_url;
    std::string patch_url;       // URL to binary patch (differential)
    int64_t patch_size = 0;
    std::string patch_sha256;    // SHA-256 of the patch itself
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
    std::vector<PatchEntry> ComputeDifferentialDelta() const;
    bool DownloadUpdates(const std::vector<PatchEntry>& updates);
    bool ApplyDifferentialPatch(const PatchEntry& entry, const std::string& local_path);
    bool VerifyInstallation();
    bool Rollback();

    // Status
    int GetTotalFiles() const { return (int)remote_manifest_.files.size(); }
    int GetFilesToUpdate() const { return (int)ComputeDelta().size(); }
    int64_t GetDownloadSize() const;
    float GetProgress() const { return progress_; }
    bool IsUpdating() const { return updating_; }
    bool IsRollingBack() const { return rolling_back_; }
    const std::string& GetVersion() const { return local_version_; }
    const std::string& GetErrorMessage() const { return last_error_; }

    // Callbacks
    using ProgressCallback = std::function<void(float pct, const std::string& file)>;
    void SetProgressCallback(ProgressCallback cb) { progress_cb_ = cb; }

    using StatusCallback = std::function<void(const std::string& status)>;
    void SetStatusCallback(StatusCallback cb) { status_cb_ = cb; }

    using ErrorCallback = std::function<void(const std::string& error)>;
    void SetErrorCallback(ErrorCallback cb) { error_cb_ = cb; }

    // Backup directory for rollback
    std::string GetBackupPath() const { return install_path_ + ".backup/"; }

private:
    std::string base_url_;
    std::string install_path_;
    std::string local_version_;
    PatchManifest remote_manifest_;
    PatchManifest local_manifest_;
    bool updating_ = false;
    bool rolling_back_ = false;
    float progress_ = 0;
    std::string last_error_;

    ProgressCallback progress_cb_;
    StatusCallback status_cb_;
    ErrorCallback error_cb_;

    // Rollback state
    std::vector<std::string> updated_files_;
    bool backup_created_ = false;

    std::string ComputeSHA256(const uint8_t* data, size_t len) const;
    std::string SHA256File(const std::string& path) const;
    bool SHA256FileVerify(const std::string& path, const std::string& expected_sha256) const;
    bool CreateBackup(const std::string& path);
    bool RestoreBackup(const std::string& path);
    bool DownloadFile(const std::string& url, const std::string& dest);
    bool ApplyBsdiffPatch(const std::string& old_file, const std::string& patch_file, const std::string& new_file);
};
