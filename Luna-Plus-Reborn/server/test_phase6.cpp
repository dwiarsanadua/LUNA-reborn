// Phase 6 integration test — SecondarySystem + MapServer + asset gates
#include "map/MapServer.h"
#include <cstdio>
#include <cstdlib>
#include <filesystem>

namespace fs = std::filesystem;

static int RunCmd(const char* cmd) {
    printf("  > %s\n", cmd);
    return std::system(cmd);
}

int main() {
    printf("=== Phase 6 MapServer Integration Test ===\n");

    if (RunCmd("python3 tools/asset_pipeline/bootstrap_phase6.py") != 0) {
        printf("FAIL: bootstrap_phase6.py\n");
        return 1;
    }
    printf("OK: bootstrap_phase6.py\n");

    if (RunCmd("python3 tools/asset_pipeline/validate_assets.py --phase6") != 0) {
        printf("FAIL: validate_assets.py --phase6\n");
        return 1;
    }
    printf("OK: validate_assets --phase6\n");

    if (!fs::exists("assets/data/luna_map.db")) {
        printf("FAIL: missing luna_map.db after bootstrap\n");
        return 1;
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
