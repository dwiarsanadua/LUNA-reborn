#pragma once
#include <string>

// Runtime asset path resolution (after VFS::InitFromExecutable).
class Paths {
public:
    static void Init();
    static const std::string& Assets();
    static const std::string& GameDataDb();
    static std::string Asset(const std::string& relative);
    static std::string FindAsset(const std::string& relative);

private:
    static std::string assets_root_;
    static std::string game_data_db_;
    static bool initialized_;
};
