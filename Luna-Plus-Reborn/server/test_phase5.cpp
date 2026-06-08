// Phase 5 integration test — MapServer quest depth
#include "map/MapServer.h"
#include "map/systems/QuestSystem.h"
#include <cstdio>
#include <filesystem>

namespace fs = std::filesystem;

int main() {
    printf("=== Phase 5 MapServer Integration Test ===\n");

    if (!fs::exists("assets/data/luna_map.db")) {
        printf("INFO: running bootstrap_phase5.py first...\n");
        if (std::system("python3 tools/asset_pipeline/bootstrap_phase5.py") != 0) {
            printf("FAIL: bootstrap_phase5.py\n");
            return 1;
        }
    }

    MapServer server;
    if (!server.Initialize(51, 8251)) {
        printf("FAIL: MapServer::Initialize(51, 8251)\n");
        return 1;
    }
    printf("OK: MapServer initialized\n");

    size_t quest_count = server.GetQuestSystem().GetTemplates().size();
    printf("OK: Quest templates loaded (%zu)\n", quest_count);
    if (quest_count < 3) {
        printf("FAIL: expected at least 3 quest templates\n");
        server.Shutdown();
        return 1;
    }

    server.Shutdown();
    printf("=== Phase 5 test: PASS ===\n");
    return 0;
}
