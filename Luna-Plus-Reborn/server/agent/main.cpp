#include "AgentServer.h"
#include <spdlog/spdlog.h>
#include <csignal>
#include <chrono>
#include <thread>

static bool g_running = true;
void signal_handler(int) { g_running = false; }

int main() {
    spdlog::info("NEXUS AgentServer starting...");
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    AgentServer server;
    if (!server.Initialize(8100)) {
        spdlog::error("AgentServer failed to initialize");
        return 1;
    }

    spdlog::info("AgentServer running on port 8100");
    while (g_running) {
        server.Update();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    server.Shutdown();
    spdlog::info("AgentServer stopped");
    return 0;
}
