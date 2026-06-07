#include "MapServer.h"
#include <spdlog/spdlog.h>
#include <csignal>
#include <cstdlib>
#include <chrono>
#include <thread>

static bool g_running = true;
void signal_handler(int) { g_running = false; }

int main(int argc, char** argv) {
    spdlog::info("NEXUS MapServer starting...");

    int map_id = (argc > 1) ? std::atoi(argv[1]) : 13;
    uint16_t port = static_cast<uint16_t>((argc > 2) ? std::atoi(argv[2]) : 8200 + map_id);

    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    MapServer server;
    if (!server.Initialize(map_id, port)) {
        spdlog::error("MapServer failed to initialize");
        return 1;
    }

    spdlog::info("MapServer: map {} running on port {}", map_id, port);

    const float dt = 1.0f / 60.0f;
    while (g_running) {
        server.Update(dt);
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }

    server.Shutdown();
    spdlog::info("MapServer stopped");
    return 0;
}
