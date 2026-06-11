#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <filesystem>
#include <cstdint>

namespace fs {

// ─── File I/O ──────────────────────────────────────────────
inline bool ReadFile(const std::string& path, std::vector<uint8_t>& data) {
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file) return false;
    size_t size = (size_t)file.tellg();
    file.seekg(0);
    data.resize(size);
    file.read(reinterpret_cast<char*>(data.data()), (std::streamsize)size);
    return true;
}

inline bool WriteFile(const std::string& path, const void* data, size_t size) {
    std::ofstream file(path, std::ios::binary);
    if (!file) return false;
    file.write(static_cast<const char*>(data), (std::streamsize)size);
    return true;
}

inline bool FileExists(const std::string& path) {
    return std::filesystem::exists(path);
}

inline uint64_t GetFileSize(const std::string& path) {
    std::error_code ec;
    auto size = std::filesystem::file_size(path, ec);
    return ec ? 0 : size;
}

inline bool DeleteFile(const std::string& path) {
    std::error_code ec;
    return std::filesystem::remove(path, ec);
}

inline bool CreateDirectory(const std::string& path) {
    std::error_code ec;
    return std::filesystem::create_directories(path, ec);
}

// ─── Path Utilities ─────────────────────────────────────────
inline std::string GetExecutableDirectory() {
    return std::filesystem::current_path().string();
}

inline std::string GetDataDirectory() {
    return GetExecutableDirectory() + "/assets";
}

inline std::string GetConfigDirectory() {
    return GetExecutableDirectory() + "/config";
}

inline std::string CombinePath(const std::string& a, const std::string& b) {
    return (std::filesystem::path(a) / b).string();
}

// ─── Directory Enumeration ──────────────────────────────────
inline std::vector<std::string> ListFiles(const std::string& dir, const std::string& ext = "") {
    std::vector<std::string> result;
    if (!std::filesystem::exists(dir)) return result;
    for (const auto& entry : std::filesystem::directory_iterator(dir)) {
        if (entry.is_regular_file()) {
            if (ext.empty() || entry.path().extension() == ext)
                result.push_back(entry.path().filename().string());
        }
    }
    return result;
}

inline std::vector<std::string> ListDirectories(const std::string& dir) {
    std::vector<std::string> result;
    if (!std::filesystem::exists(dir)) return result;
    for (const auto& entry : std::filesystem::directory_iterator(dir)) {
        if (entry.is_directory())
            result.push_back(entry.path().filename().string());
    }
    return result;
}

} // namespace fs
