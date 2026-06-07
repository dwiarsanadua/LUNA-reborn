#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <network/TcpServer.hpp>
#include <cstdlib>
#include <csignal>
#include <memory>
#include <thread>
#include <chrono>
#include <vector>
#include <mutex>
#include <map>

static std::unique_ptr<TcpServer> g_server;
static bool g_running = true;
void signal_handler(int) { g_running = false; }

struct MapServerInfo {
    uint32_t id;
    uint16_t map_id;
    std::string name;
    std::string host;
    uint16_t port;
    int player_count = 0;
    int max_players = 200;
    bool online = false;
};

struct ChannelInfo {
    uint16_t channel_id;
    std::string name;
    std::vector<MapServerInfo> maps;
    int total_players = 0;
};

static std::mutex g_mutex;
static std::vector<ChannelInfo> g_channels;
static std::map<uint32_t, std::shared_ptr<TcpConnection>> g_map_conns;

#include <game/Log.h>
#include <sys/stat.h>

int main(int argc, char* argv[]) {
    mkdir("logs", 0777);
    Luna::InitLog("distribute", "distribute.log");
    spdlog::info("LUNA Plus Reborn — DistributeServer v{}", "1.0.0");

    int port = 8200;
    if (argc > 1) port = std::atoi(argv[1]);

    signal(SIGINT, signal_handler); signal(SIGTERM, signal_handler);

    // Initialize default channels
    for (int i = 1; i <= 3; i++) {
        ChannelInfo ch;
        ch.channel_id = i;
        ch.name = fmt::format("Channel {}", i);
        g_channels.push_back(ch);
    }

    g_server = std::make_unique<TcpServer>();
    g_server->SetNewConnectionCallback([](std::shared_ptr<TcpConnection> conn) {
        spdlog::info("New connection: id={}", conn->id);
    });
    g_server->SetDataCallback([](std::shared_ptr<TcpConnection> conn, uint8_t* data, size_t len, uint16_t type) {
        std::lock_guard<std::mutex> lock(g_mutex);
        if (type == 0x0001) {
            // MapServer registration
            spdlog::info("MapServer registered: conn={}", conn->id);
            g_map_conns[conn->id] = conn;
            for (auto& ch : g_channels) {
                if (ch.maps.empty()) {
                    MapServerInfo msi;
                    msi.id = conn->id;
                    msi.map_id = 51;
                    msi.name = fmt::format("MapServer-51-{}", ch.channel_id);
                    msi.host = "127.0.0.1";
                    msi.port = 8300 + ch.channel_id - 1;
                    msi.online = true;
                    ch.maps.push_back(msi);
                    ch.total_players = 0;
                    spdlog::info("Assigned to {}", ch.name);
                    break;
                }
            }
        } else if (type == 0x0104) {
            // Server list request — respond with channel info
            std::lock_guard<std::mutex> lock(g_mutex);
            spdlog::info("Server list request from conn={}", conn->id);
            std::string response;
            for (auto& ch : g_channels) {
                response += fmt::format("{}:{} ", ch.channel_id, ch.total_players);
            }
            if (!response.empty()) response.pop_back();
            spdlog::info("Channels: [{}]", response);
        } else {
            spdlog::info("Packet type=0x{:04X} from conn={}", type, conn->id);
        }
    });
    g_server->SetCloseCallback([](std::shared_ptr<TcpConnection> conn) {
        std::lock_guard<std::mutex> lock(g_mutex);
        g_map_conns.erase(conn->id);
        for (auto& ch : g_channels) {
            for (auto it = ch.maps.begin(); it != ch.maps.end(); ) {
                if (it->id == conn->id) { it = ch.maps.erase(it); }
                else { ++it; }
            }
        }
        spdlog::info("Connection closed: id={}", conn->id);
    });

    if (!g_server->Start(static_cast<uint16_t>(port))) {
        spdlog::error("Failed to start server on port {}", port);
        return 1;
    }

    spdlog::info("DistributeServer running on port {}", port);

    auto last_status = std::chrono::steady_clock::now();
    while (g_running && g_server->IsRunning()) {
        g_server->Poll();
        auto now = std::chrono::steady_clock::now();
        if (std::chrono::duration<float>(now - last_status).count() > 10.0f) {
            last_status = now;
            std::lock_guard<std::mutex> lock(g_mutex);
            spdlog::info("--- Server Status ---");
            for (auto& ch : g_channels) {
                spdlog::info("  {}: {} maps, {} players", ch.name, ch.maps.size(), ch.total_players);
                for (auto& m : ch.maps) {
                    spdlog::info("    Map {}: {} ({} players)", m.map_id, m.name, m.player_count);
                }
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    spdlog::info("DistributeServer shutdown");
    return 0;
}
