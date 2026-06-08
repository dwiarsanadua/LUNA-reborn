#pragma once
#include <string>
#include <vector>
#include <fstream>

// Virtual File System — resolves assets relative to executable and project roots.
class VFS {
public:
    static void Init(const std::string& base_path);
    static void InitFromExecutable();
    static void AddSearchRoot(const std::string& path);

    static std::string Resolve(const std::string& path);
    static std::string Find(const std::string& relative_path);
    static bool Exists(const std::string& path);

    static const std::string& GetBasePath() { return base_path_; }
    static const std::vector<std::string>& GetSearchRoots() { return search_roots_; }

private:
    static std::string NormalizeRoot(const std::string& path);
    static std::string base_path_;
    static std::vector<std::string> search_roots_;
};
