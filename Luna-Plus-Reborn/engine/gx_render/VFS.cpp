#include "VFS.h"
#include <spdlog/spdlog.h>
#include <algorithm>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <unordered_set>

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

// Cache of known missing files to avoid repeated warnings
static std::unordered_set<std::string> s_missing_file_cache;

std::string VFS::NormalizeRoot(const std::string& path) {
    if (path.empty()) return "./";
    std::string out = path;

#if defined(_WIN32)
    // Normalize Windows backslashes to forward slashes
    for (auto& c : out) if (c == '\\') c = '/';
#endif

    if (out.back() != '/') out += '/';
    return out;
}

void VFS::Init(const std::string& base_path) {
    base_path_ = NormalizeRoot(base_path);
    // Re-add as the first search root so it's checked first
    bool found = false;
    for (const auto& r : search_roots_)
        if (r == base_path_) { found = true; break; }
    if (!found)
        search_roots_.insert(search_roots_.begin(), base_path_);
}

void VFS::AddSearchRoot(const std::string& path) {
    std::string root = NormalizeRoot(path);
    for (const auto& existing : search_roots_) {
        if (existing == root) return;
    }
    search_roots_.push_back(root);
}

void VFS::InitFromExecutable() {
    static bool initialized = false;
    if (initialized) return;

    search_roots_.clear();
    s_missing_file_cache.clear();

    // 1. Environment variable override — highest priority
    const char* env_path = getenv("LUNA_ASSETS_PATH");
    if (env_path && env_path[0] != '\0') {
        std::string ep = NormalizeRoot(env_path);
        // LUNA_ASSETS_PATH may point to the assets/ sub-dir itself or its parent
        if (fs::exists(ep + "data/game_data.db")) {
            // Pointed directly at assets/ — parent is the root
            AddSearchRoot(fs::path(ep).parent_path().string());
        } else {
            AddSearchRoot(ep);
        }
        base_path_ = search_roots_.front();
        spdlog::info("VFS: initialized from LUNA_ASSETS_PATH={}", env_path);
        initialized = true;
        return;
    }

    // 2. Resolve the executable's directory
    std::string exe_dir = "./";
#if defined(__APPLE__)
    // Use _NSGetExecutablePath which resolves symlinks
    char exe_buf[4096];
    uint32_t exe_size = sizeof(exe_buf);
    if (_NSGetExecutablePath(exe_buf, &exe_size) == 0) {
        // Canonicalize to resolve symlinks (important for app bundles)
        std::error_code ec;
        auto canonical = fs::canonical(exe_buf, ec);
        if (!ec) exe_dir = canonical.parent_path().string() + "/";
        else     exe_dir = fs::path(exe_buf).parent_path().string() + "/";
    }

    // macOS app bundle: executable is at MyApp.app/Contents/MacOS/Game
    // Assets are at MyApp.app/Contents/Resources/ or at the project root.
    // Walk up to detect bundle and add Resources/ as a search root.
    {
        fs::path p = exe_dir;
        // Traverse upward looking for *.app/Contents
        for (int i = 0; i < 6; ++i) {
            fs::path parent = p.parent_path();
            if (parent == p) break;   // filesystem root
            if (parent.filename().string() == "Contents") {
                fs::path app_root  = parent.parent_path(); // *.app/
                fs::path resources = parent / "Resources";
                if (fs::is_directory(resources))
                    AddSearchRoot(resources.string());
                // Also add the directory containing the .app bundle
                AddSearchRoot(app_root.parent_path().string());
                break;
            }
            p = parent;
        }
    }
#elif defined(_WIN32)
    {
        char win_buf[MAX_PATH];
        DWORD len = GetModuleFileNameA(nullptr, win_buf, MAX_PATH);
        if (len > 0)
            exe_dir = fs::path(std::string(win_buf, len)).parent_path().string() + "\\";
    }
#else
    {
        char linux_buf[PATH_MAX];
        ssize_t len = readlink("/proc/self/exe", linux_buf, sizeof(linux_buf) - 1);
        if (len > 0) {
            linux_buf[len] = '\0';
            exe_dir = fs::path(linux_buf).parent_path().string() + "/";
        }
    }
#endif

    // Standard search locations: binary dir, one level up, cwd, parent of cwd
    const std::vector<std::string> candidates = {
        exe_dir,
        exe_dir + "../",
        exe_dir + "../../",
        "./",
        "../",
    };

    // Probe each candidate for a game data marker file
    static const char* MARKERS[] = {
        "assets/data/game_data.db",
        "assets/maps/51.hgt",
        "assets/fonts/2002_EYA.ttf",
    };

    for (const auto& c : candidates) {
        std::error_code ec;
        std::string norm = NormalizeRoot(fs::weakly_canonical(c, ec).string());
        if (ec) norm = NormalizeRoot(c);

        AddSearchRoot(norm);

        for (const char* marker : MARKERS) {
            if (fs::exists(norm + marker)) {
                base_path_ = norm;
                // Move the winning root to the front for fast resolution
                search_roots_.erase(
                    std::remove(search_roots_.begin(), search_roots_.end(), norm),
                    search_roots_.end());
                search_roots_.insert(search_roots_.begin(), norm);
                spdlog::info("VFS: root detected at '{}' (marker: {})", norm, marker);
                initialized = true;
                return;
            }
        }
    }

    // Fallback — no marker found, use executable directory and hope for the best
    base_path_ = NormalizeRoot(exe_dir);
    spdlog::warn("VFS: could not locate asset root; using exe dir '{}'. "
                 "Set LUNA_ASSETS_PATH env var if assets are elsewhere.", exe_dir);
    initialized = true;
}

std::string VFS::Resolve(const std::string& path) {
    if (path.empty()) return path;
    // Absolute paths pass straight through
#if defined(_WIN32)
    if (path.size() >= 2 && path[1] == ':') return path;
    if (path[0] == '/' || path[0] == '\\') return path;
#else
    if (path[0] == '/') return path;
#endif
    // Already has the base path prepended
    if (path.rfind(base_path_, 0) == 0) return path;
    return base_path_ + path;
}

std::string VFS::Find(const std::string& relative_path) {
    if (relative_path.empty()) return relative_path;

    // Absolute paths: just check existence
#if defined(_WIN32)
    bool is_abs = (relative_path.size() >= 2 && relative_path[1] == ':') ||
                  (relative_path[0] == '/' || relative_path[0] == '\\');
#else
    bool is_abs = (relative_path[0] == '/');
#endif
    if (is_abs) {
        if (fs::exists(relative_path)) return relative_path;
        return relative_path; // return as-is; caller handles missing
    }

    // Search all registered roots
    for (const auto& root : search_roots_) {
        std::string full = root + relative_path;
        if (fs::exists(full)) return full;
    }

    // Suppress log spam for repeatedly-queried missing files
    if (s_missing_file_cache.find(relative_path) == s_missing_file_cache.end()) {
        s_missing_file_cache.insert(relative_path);
        spdlog::debug("VFS: '{}' not found in {} root(s)", relative_path, search_roots_.size());
    }

    // Fallback: caller must check existence
    return Resolve(relative_path);
}

bool VFS::Exists(const std::string& path) {
    if (path.empty()) return false;
    std::string resolved = Find(path);
    return fs::exists(resolved);
}
