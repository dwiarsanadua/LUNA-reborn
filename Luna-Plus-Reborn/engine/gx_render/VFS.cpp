#include "VFS.h"
#include <spdlog/spdlog.h>
#include <filesystem>
#include <fstream>

#if defined(__APPLE__)
#include <mach-o/dyld.h>
#elif defined(_WIN32)
#include <windows.h>
#else
#include <unistd.h>
#include <limits.h>
#endif

namespace fs = std::filesystem;

std::string VFS::base_path_ = "./";
std::vector<std::string> VFS::search_roots_;

std::string VFS::NormalizeRoot(const std::string& path) {
    if (path.empty()) return "./";
    std::string out = path;
    if (out.back() != '/') out += '/';
    return out;
}

void VFS::Init(const std::string& base_path) {
    base_path_ = NormalizeRoot(base_path);
    if (search_roots_.empty()) {
        search_roots_.push_back(base_path_);
    }
}

void VFS::AddSearchRoot(const std::string& path) {
    std::string root = NormalizeRoot(path);
    for (const auto& existing : search_roots_) {
        if (existing == root) return;
    }
    search_roots_.push_back(root);
}

void VFS::InitFromExecutable() {
    search_roots_.clear();

    std::string exe_dir = "./";
#if defined(__APPLE__)
    char exe_path[4096];
    uint32_t size = sizeof(exe_path);
    if (_NSGetExecutablePath(exe_path, &size) == 0) {
        exe_dir = fs::path(exe_path).parent_path().string() + "/";
    }
#elif defined(_WIN32)
    char exe_path[MAX_PATH];
    DWORD len = GetModuleFileNameA(nullptr, exe_path, MAX_PATH);
    if (len > 0) {
        exe_dir = fs::path(std::string(exe_path, len)).parent_path().string() + "\\";
    }
#else
    char exe_path[PATH_MAX];
    ssize_t len = readlink("/proc/self/exe", exe_path, sizeof(exe_path) - 1);
    if (len > 0) {
        exe_path[len] = '\0';
        exe_dir = fs::path(exe_path).parent_path().string() + "/";
    }
#endif

    const std::string candidates[] = {
        exe_dir,
        exe_dir + "../",
        exe_dir + "../../",
        exe_dir + "../../../",
        "./",
        "../",
        "../../",
    };

    for (const auto& c : candidates) {
        AddSearchRoot(c);
    }

    base_path_ = search_roots_.front();
    spdlog::info("VFS: initialized with {} search root(s), primary={}",
                 search_roots_.size(), base_path_);
}

std::string VFS::Resolve(const std::string& path) {
    if (path.empty()) return path;
    if (path[0] == '/') return path;
    if (path.find(base_path_) == 0) return path;
    return base_path_ + path;
}

std::string VFS::Find(const std::string& relative_path) {
    if (relative_path.empty()) return relative_path;
    if (relative_path[0] == '/' && fs::exists(relative_path)) {
        return relative_path;
    }

    for (const auto& root : search_roots_) {
        std::string full = root + relative_path;
        if (fs::exists(full)) return full;
    }
    return Resolve(relative_path);
}

bool VFS::Exists(const std::string& path) {
    std::string resolved = Find(path);
    return fs::exists(resolved);
}
