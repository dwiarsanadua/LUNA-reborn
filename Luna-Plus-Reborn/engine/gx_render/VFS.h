#pragma once
#include <string>
#include <vector>
#include <fstream>

// Virtual File System — resolves assets relative to executable and project roots.
// On macOS, automatically handles app bundle layout (app/Contents/Resources/).
// Override root at runtime with the LUNA_ASSETS_PATH environment variable.
class VFS {
public:
    // Set base path explicitly (also adds it as first search root)
    static void Init(const std::string& base_path);

    // Auto-detect root from executable location at startup
    static void InitFromExecutable();

    // Register an additional directory to search (lower priority than existing roots)
    static void AddSearchRoot(const std::string& path);

    // Prepend base_path to a relative path (does NOT check existence)
    static std::string Resolve(const std::string& path);

    // Search all roots and return the first path where the file exists.
    // Returns Resolve(relative_path) as fallback if not found.
    static std::string Find(const std::string& relative_path);

    // Returns true if the file exists at any registered root
    static bool Exists(const std::string& path);

    static const std::string& GetBasePath() { return base_path_; }
    static const std::vector<std::string>& GetSearchRoots() { return search_roots_; }

private:
    static std::string NormalizeRoot(const std::string& path);
    static std::string base_path_;
    static std::vector<std::string> search_roots_;
};
