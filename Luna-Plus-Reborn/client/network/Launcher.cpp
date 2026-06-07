#include "Launcher.hpp"
#include <fstream>
#include <sstream>
#include <cstdio>
#include <spdlog/spdlog.h>

bool Launcher::Init(const std::string& base_url, const std::string& install_path) {
    base_url_ = base_url;
    install_path_ = install_path;
    if (install_path_.back() != '/') install_path_ += '/';
    spdlog::info("Launcher: init (url={}, path={})", base_url_, install_path_);
    return true;
}

void Launcher::Shutdown() {
    spdlog::info("Launcher: shutdown");
}

bool Launcher::FetchRemoteManifest() {
    // In production: HTTP GET base_url_ + "manifest.json"
    // Stub: create sample manifest
    remote_manifest_.version = "1.1.0";
    remote_manifest_.files = {
        {"LunaPlusClient", "abc123", 6291456, base_url_ + "/LunaPlusClient"},
        {"shaders/vs_default.bin", "def456", 875, base_url_ + "/shaders/vs_default.bin"},
        {"shaders/fs_ui.bin", "ghi789", 644, base_url_ + "/shaders/fs_ui.bin"},
    };
    spdlog::info("Launcher: remote manifest v{} ({} files)", remote_manifest_.version, remote_manifest_.files.size());
    return true;
}

bool Launcher::LoadLocalManifest() {
    std::string path = install_path_ + "manifest.json";
    std::ifstream f(path);
    if (!f) {
        spdlog::info("Launcher: no local manifest, fresh install");
        local_manifest_.version = "0.0.0";
        return false;
    }
    // Parse JSON (simplified)
    std::string line;
    while (std::getline(f, line)) {
        if (line.find("\"version\"") != std::string::npos) {
            auto q1 = line.find('"', line.find("version") + 8);
            auto q2 = line.find('"', q1 + 1);
            local_manifest_.version = line.substr(q1 + 1, q2 - q1 - 1);
        }
    }
    local_version_ = local_manifest_.version;
    spdlog::info("Launcher: local manifest v{}", local_manifest_.version);
    return true;
}

bool Launcher::SaveLocalManifest(const std::string& path) {
    std::string out_path = path.empty() ? install_path_ + "manifest.json" : path;
    std::ofstream f(out_path);
    if (!f) return false;
    f << "{\n  \"version\": \"" << remote_manifest_.version << "\",\n  \"files\": [\n";
    for (size_t i = 0; i < remote_manifest_.files.size(); i++) {
        auto& e = remote_manifest_.files[i];
        f << "    {\"path\":\"" << e.relative_path << "\",\"sha256\":\"" << e.sha256
          << "\",\"size\":" << e.file_size << "}";
        if (i < remote_manifest_.files.size() - 1) f << ",";
        f << "\n";
    }
    f << "  ]\n}\n";
    local_version_ = remote_manifest_.version;
    spdlog::info("Launcher: manifest saved v{}", local_version_);
    return true;
}

std::vector<PatchEntry> Launcher::ComputeDelta() const {
    std::vector<PatchEntry> delta;
    for (auto& remote : remote_manifest_.files) {
        std::string local_path = install_path_ + remote.relative_path;
        std::ifstream test(local_path);
        if (!test) {
            delta.push_back(remote); // File doesn't exist locally
            continue;
        }
        test.close();
        std::string local_hash = Sha256File(local_path);
        if (local_hash != remote.sha256) {
            delta.push_back(remote); // File changed
        }
    }
    return delta;
}

bool Launcher::DownloadUpdates(const std::vector<PatchEntry>& updates) {
    if (updates.empty()) { spdlog::info("Launcher: no updates needed"); return true; }
    
    updating_ = true;
    spdlog::info("Launcher: downloading {} files...", updates.size());
    
    for (size_t i = 0; i < updates.size(); i++) {
        progress_ = (float)(i + 1) / (float)updates.size();
        if (progress_cb_) progress_cb_(progress_, updates[i].relative_path);
        
        std::string dest = install_path_ + updates[i].relative_path;
        // Create parent directories
        size_t slash = dest.rfind('/');
        if (slash != std::string::npos) {
            std::string dir = dest.substr(0, slash);
            std::string cmd = "mkdir -p " + dir;
            system(cmd.c_str());
        }
        
        // In production: actually download
        spdlog::info("Launcher:   [{}/{}] {}", i + 1, updates.size(), updates[i].relative_path);
    }
    
    SaveLocalManifest();
    updating_ = false;
    progress_ = 1.0f;
    spdlog::info("Launcher: update complete");
    return true;
}

bool Launcher::VerifyInstallation() {
    auto delta = ComputeDelta();
    return delta.empty();
}

int64_t Launcher::GetDownloadSize() const {
    int64_t total = 0;
    for (auto& e : remote_manifest_.files) total += e.file_size;
    return total;
}

std::string Launcher::Sha256File(const std::string& path) const {
    std::string cmd = "shasum -a 256 \"" + path + "\" 2>/dev/null | cut -d' ' -f1";
    FILE* fp = popen(cmd.c_str(), "r");
    if (!fp) return "";
    char buf[65] = {};
    if (fgets(buf, sizeof(buf), fp)) buf[64] = 0;
    pclose(fp);
    return buf;
}

bool Launcher::DownloadFile(const std::string& url, const std::string& dest) {
    (void)url; (void)dest;
    // In production: use libcurl or similar
    return true;
}
