// AGENT Integrator — Integration test
#include "shared/Database.h"
#include "agent/AgentServer.h"
#include <cstdio>
#include <thread>
#include <chrono>
#include <csignal>

static bool g_running = true;

int main() {
    printf("=== NEXUS Server Integration Test ===\n");

    // 1. Initialize Database
    Database db;
    if (!db.Initialize("assets/data/luna_member.db")) {
        printf("FAIL: Database::Initialize()\n");
        return 1;
    }
    printf("OK: Database initialized\n");

    // 2. Verify DB is operational
    auto result = db.Query("SELECT COUNT(*) FROM sqlite_master WHERE type='table'");
    if (!result.empty()) {
        printf("OK: Database query works (%s tables found)\n", result[0][0].c_str());
    }

    // 3. Initialize AgentServer
    AgentServer server;
    if (!server.Initialize(8100)) {
        printf("FAIL: AgentServer::Initialize(8100)\n");
        db.Shutdown();
        return 1;
    }
    printf("OK: AgentServer listening on port 8100\n");

    // 4. Run for 5 seconds
    printf("INFO: Server running for 5 seconds...\n");
    auto start = std::chrono::steady_clock::now();
    int updates = 0;
    while (g_running) {
        server.Update();
        db.Execute("SELECT 1");
        updates++;
        auto elapsed = std::chrono::steady_clock::now() - start;
        if (elapsed >= std::chrono::seconds(5)) break;
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    // 5. Shutdown
    server.Shutdown();
    db.Shutdown();

    printf("OK: Server shutdown cleanly (%d updates)\n", updates);
    printf("=== Server test: PASS ===\n");
    return 0;
}
