#pragma once
#include <string>
#include <vector>
#include <fstream>

// Virtual File System — base path + relative path resolver
class VFS {
public:
    static void Init(const std::string& base_path) {
        base_path_ = base_path;
        if (base_path_.empty() || base_path_.back() != '/')
            base_path_ += '/';
    }
    
    static std::string Resolve(const std::string& path) {
        if (path.empty()) return path;
        if (path[0] == '/') return path;  // absolute already
        if (path.find(base_path_) == 0) return path;  // already resolved
        return base_path_ + path;
    }
    
    static bool Exists(const std::string& path) {
        std::ifstream f(Resolve(path));
        return f.good();
    }
    
    static const std::string& GetBasePath() { return base_path_; }

private:
    static std::string base_path_;
};
