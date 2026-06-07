#include "DistributeServer.h"
#include <spdlog/spdlog.h>
#include <csignal>
#include <chrono>
#include <thread>

static bool g_running = true;
void signal_handler(int) { g_running = false; }

int main(int argc, char* argv[]) {
    spdlog::info("NEXUS DistributeServer starting...");
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    int port = (argc > 1) ? std::atoi(argv[1]) : 8300;

    DistributeServer server;
    if (!server.Initialize(static_cast<uint16_t>(port))) {
        spdlog::error("DistributeServer failed to initialize");
        return 1;
    }

    spdlog::info("DistributeServer running on port {}", port);
    while (g_running) {
        server.Update();
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    server.Shutdown();
    spdlog::info("DistributeServer stopped");
    return 0;
}
