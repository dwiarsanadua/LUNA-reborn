// Phase 6 integration test — SecondarySystem + MapServer init
#include "map/MapServer.h"
#include "map/systems/SecondarySystem.hpp"
#include <cstdio>
#include <filesystem>

namespace fs = std::filesystem;

int main() {
    printf("=== Phase 6 MapServer Integration Test ===\n");

    if (!fs::exists("assets/data/luna_map.db")) {
        printf("INFO: running bootstrap_phase6.py first...\n");
        if (std::system("python3 tools/asset_pipeline/bootstrap_phase6.py") != 0) {
            printf("FAIL: bootstrap_phase6.py\n");
            return 1;
        }
    }

    MapServer server;
    if (!server.Initialize(51, 8251)) {
        printf("FAIL: MapServer::Initialize(51, 8251)\n");
        return 1;
    }
    printf("OK: MapServer initialized with SecondarySystem\n");

    server.Shutdown();
    printf("=== Phase 6 test: PASS ===\n");
    return 0;
}
