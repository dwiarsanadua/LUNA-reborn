#include <cstdio>
#include <cstring>
#include <string>
#include <vector>
#include <fstream>
#include <filesystem>
#include <iomanip>
#include <sstream>
#include <cstdint>

namespace fs = std::filesystem;

static std::string Sha256Of(const std::string& path) {
    std::string cmd = "shasum -a 256 \"" + path + "\" 2>/dev/null | cut -d' ' -f1";
    FILE* fp = popen(cmd.c_str(), "r");
    if (!fp) return "0000000000000000000000000000000000000000000000000000000000000000";
    char buf[65] = {};
    if (fgets(buf, sizeof(buf), fp)) buf[64] = 0;
    pclose(fp);
    return buf;
}

static std::string FileSizeStr(const std::string& path) {
    std::ifstream f(path, std::ios::binary | std::ios::ate);
    if (!f) return "0";
    return std::to_string(f.tellg());
}

int main(int argc, char** argv) {
    std::string build_dir = "build";
    std::string version = "1.0.5";
    std::string out = "update/manifest.json";
    if (argc > 1) build_dir = argv[1];
    if (argc > 2) version = argv[2];
    if (argc > 3) out = argv[3];

    fs::create_directories(fs::path(out).parent_path());

    struct FileEntry {
        std::string path;
        std::string hash;
        std::string size;
    };

    std::vector<FileEntry> files;
    std::vector<std::string> targets = {
        "client/LunaPlusClient",
        "server/AgentServer/AgentServer",
        "server/DistributeServer/DistributeServer",
        "server/MapServer/MapServer",
    };

    for (auto& t : targets) {
        std::string full = build_dir + "/" + t;
        if (fs::exists(full)) {
            files.push_back({t, Sha256Of(full), FileSizeStr(full)});
        }
    }

    std::ofstream f(out);
    f << "{\n";
    f << "  \"version\": \"" << version << "\",\n";
    f << "  \"files\": [\n";
    for (size_t i = 0; i < files.size(); i++) {
        f << "    {\"path\": \"" << files[i].path << "\", "
          << "\"hash\": \"" << files[i].hash << "\", "
          << "\"size\": " << files[i].size << "}";
        if (i < files.size() - 1) f << ",";
        f << "\n";
    }
    f << "  ]\n";
    f << "}\n";
    f.close();

    printf("Update manifest generated: %s (version %s, %zu files)\n",
           out.c_str(), version.c_str(), files.size());
    return 0;
}
