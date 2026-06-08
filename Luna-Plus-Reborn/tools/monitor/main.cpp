/// Server Monitor Tool — connects to AgentServer via TCP and displays metrics

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <thread>
#include <chrono>
#include <atomic>
#include <mutex>
#include <vector>
#include <algorithm>
#include <ctime>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
    using SOCKET_T = SOCKET;
    #define CLOSE_SOCKET(s) closesocket(s)
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <netdb.h>
    #include <unistd.h>
    #include <arpa/inet.h>
    using SOCKET_T = int;
    #define CLOSE_SOCKET(s) close(s)
    #define SOCKET_ERROR -1
    #define INVALID_SOCKET -1
#endif

#include <spdlog/spdlog.h>

// ── Configuration ──────────────────────────────────────────────────────────

struct MonitorConfig {
    std::string server_host = "127.0.0.1";
    int server_port = 10800;         // AgentServer default port
    int refresh_interval_ms = 2000;  // Poll interval
    bool verbose = false;
    bool auto_reconnect = true;
};

// ── Server metrics ─────────────────────────────────────────────────────────

struct ServerMetrics {
    // Connection
    bool connected = false;
    std::string server_name;
    std::string server_version;
    int uptime_seconds = 0;

    // Players
    int player_count = 0;
    int max_players = 0;
    int unique_accounts = 0;

    // Performance
    float cpu_usage = 0;
    float memory_usage_mb = 0;
    float tick_rate = 0;
    int active_threads = 0;

    // Network
    int active_connections = 0;
    int packets_per_sec = 0;
    int bandwidth_in_kbps = 0;
    int bandwidth_out_kbps = 0;

    // Channels
    int channel_count = 0;
    std::vector<std::string> channel_names;

    // Errors / warnings
    int error_count = 0;
    int warning_count = 0;
    std::string last_error;
    std::string last_warning;

    // Timestamp
    std::time_t last_update = 0;
};

// ── Global state ───────────────────────────────────────────────────────────

static MonitorConfig s_config;
static ServerMetrics s_metrics;
static std::mutex s_metrics_mutex;
static std::atomic<bool> s_running{true};
static SOCKET_T s_sock = INVALID_SOCKET;
static bool s_alerts_enabled = true;

// ── Socket helpers ─────────────────────────────────────────────────────────

static bool InitSockets() {
#ifdef _WIN32
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        spdlog::error("Monitor: WSAStartup failed");
        return false;
    }
#endif
    return true;
}

static void CleanupSockets() {
#ifdef _WIN32
    WSACleanup();
#endif
}

static bool ConnectToServer() {
    if (s_sock != INVALID_SOCKET) {
        CLOSE_SOCKET(s_sock);
        s_sock = INVALID_SOCKET;
    }

    s_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (s_sock == INVALID_SOCKET) {
        spdlog::error("Monitor: socket creation failed");
        return false;
    }

    struct hostent* server = gethostbyname(s_config.server_host.c_str());
    if (!server) {
        spdlog::error("Monitor: cannot resolve host '{}'", s_config.server_host);
        CLOSE_SOCKET(s_sock);
        s_sock = INVALID_SOCKET;
        return false;
    }

    struct sockaddr_in addr{};
    addr.sin_family = AF_INET;
    std::memcpy(&addr.sin_addr.s_addr, server->h_addr, server->h_length);
    addr.sin_port = htons((uint16_t)s_config.server_port);

    if (connect(s_sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        spdlog::warn("Monitor: connection to {}:{} failed",
                     s_config.server_host, s_config.server_port);
        CLOSE_SOCKET(s_sock);
        s_sock = INVALID_SOCKET;
        return false;
    }

    // Set non-blocking for recv
#ifdef _WIN32
    u_long mode = 1;
    ioctlsocket(s_sock, FIONBIO, &mode);
#else
    int flags = fcntl(s_sock, F_GETFL, 0);
    fcntl(s_sock, F_SETFL, flags | O_NONBLOCK);
#endif

    spdlog::info("Monitor: connected to {}:{}", s_config.server_host, s_config.server_port);
    return true;
}

static void Disconnect() {
    if (s_sock != INVALID_SOCKET) {
        CLOSE_SOCKET(s_sock);
        s_sock = INVALID_SOCKET;
    }
    std::lock_guard<std::mutex> lock(s_metrics_mutex);
    s_metrics.connected = false;
    spdlog::info("Monitor: disconnected");
}

static bool SendRequest(const std::string& request) {
    if (s_sock == INVALID_SOCKET) return false;
    int sent = (int)send(s_sock, request.c_str(), (int)request.size(), 0);
    if (sent < 0) {
        spdlog::warn("Monitor: send failed, disconnecting");
        Disconnect();
        return false;
    }
    return true;
}

static std::string RecvResponse(int timeout_ms = 2000) {
    if (s_sock == INVALID_SOCKET) return "";

    std::string response;
    char buf[4096];

    auto start = std::chrono::steady_clock::now();
    while (true) {
        int n = (int)recv(s_sock, buf, sizeof(buf) - 1, 0);
        if (n > 0) {
            buf[n] = 0;
            response += buf;
            if (response.find('\n') != std::string::npos) break;
        } else if (n == 0) {
            spdlog::warn("Monitor: connection closed by server");
            Disconnect();
            return "";
        } else {
            // Would block (non-blocking socket)
            auto now = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count();
            if (elapsed >= timeout_ms) break;
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
    return response;
}

// ── Protocol: send STATUS command, parse response ─────────────────────────

static void ParseStatusResponse(const std::string& resp) {
    std::lock_guard<std::mutex> lock(s_metrics_mutex);

    s_metrics.connected = true;
    s_metrics.last_update = std::time(nullptr);

    // Simple key:value parsing
    std::stringstream ss(resp);
    std::string line;
    while (std::getline(ss, line)) {
        if (line.empty()) continue;
        auto colon = line.find(':');
        if (colon == std::string::npos) continue;

        std::string key = line.substr(0, colon);
        std::string val = line.substr(colon + 1);
        // Trim
        key.erase(0, key.find_first_not_of(" \t\r\n"));
        key.erase(key.find_last_not_of(" \t\r\n") + 1);
        val.erase(0, val.find_first_not_of(" \t\r\n"));
        val.erase(val.find_last_not_of(" \t\r\n") + 1);

        if (key == "server_name") s_metrics.server_name = val;
        else if (key == "version") s_metrics.server_version = val;
        else if (key == "uptime") s_metrics.uptime_seconds = atoi(val.c_str());
        else if (key == "players") s_metrics.player_count = atoi(val.c_str());
        else if (key == "max_players") s_metrics.max_players = atoi(val.c_str());
        else if (key == "unique_accounts") s_metrics.unique_accounts = atoi(val.c_str());
        else if (key == "cpu") s_metrics.cpu_usage = (float)atof(val.c_str());
        else if (key == "memory_mb") s_metrics.memory_usage_mb = (float)atof(val.c_str());
        else if (key == "tick_rate") s_metrics.tick_rate = (float)atof(val.c_str());
        else if (key == "threads") s_metrics.active_threads = atoi(val.c_str());
        else if (key == "connections") s_metrics.active_connections = atoi(val.c_str());
        else if (key == "packets_sec") s_metrics.packets_per_sec = atoi(val.c_str());
        else if (key == "bandwidth_in") s_metrics.bandwidth_in_kbps = atoi(val.c_str());
        else if (key == "bandwidth_out") s_metrics.bandwidth_out_kbps = atoi(val.c_str());
        else if (key == "channels") s_metrics.channel_count = atoi(val.c_str());
        else if (key == "errors") s_metrics.error_count = atoi(val.c_str());
        else if (key == "warnings") s_metrics.warning_count = atoi(val.c_str());
        else if (key == "last_error") s_metrics.last_error = val;
        else if (key == "last_warning") s_metrics.last_warning = val;
    }
}

static bool PollServer() {
    if (s_sock == INVALID_SOCKET) {
        if (s_config.auto_reconnect) {
            if (!ConnectToServer()) return false;
        } else {
            return false;
        }
    }

    if (!SendRequest("STATUS\n")) return false;
    std::string resp = RecvResponse();
    if (resp.empty()) return false;

    ParseStatusResponse(resp);
    return true;
}

// ── Monitor thread ─────────────────────────────────────────────────────────

static void MonitorThreadFunc() {
    spdlog::info("Monitor: polling thread started (interval={}ms)", s_config.refresh_interval_ms);

    while (s_running) {
        if (!PollServer()) {
            std::lock_guard<std::mutex> lock(s_metrics_mutex);
            s_metrics.connected = false;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(s_config.refresh_interval_ms));
    }

    Disconnect();
    spdlog::info("Monitor: polling thread stopped");
}

// ── Console display ────────────────────────────────────────────────────────

#ifdef _WIN32
    #define COLOR_RESET ""
    #define COLOR_RED ""
    #define COLOR_GREEN ""
    #define COLOR_YELLOW ""
    #define COLOR_CYAN ""
    #define COLOR_GRAY ""
#else
    #define COLOR_RESET "\033[0m"
    #define COLOR_RED "\033[31m"
    #define COLOR_GREEN "\033[32m"
    #define COLOR_YELLOW "\033[33m"
    #define COLOR_CYAN "\033[36m"
    #define COLOR_GRAY "\033[90m"
#endif

static void ClearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

static void DrawProgressBar(float pct, int width) {
    int filled = (int)(pct * width);
    printf("[");
    for (int i = 0; i < width; i++) {
        if (i < filled) printf("=");
        else if (i == filled) printf(">");
        else printf(" ");
    }
    printf("]");
}

static void DisplayMetrics() {
    std::lock_guard<std::mutex> lock(s_metrics_mutex);

    ClearScreen();
    printf("\n");
    printf(COLOR_CYAN "  ╔══════════════════════════════════════════════════════════╗\n" COLOR_RESET);
    printf(COLOR_CYAN "  ║         LUNA PLUS REBORN — SERVER MONITOR v1.0        ║\n" COLOR_RESET);
    printf(COLOR_CYAN "  ╚══════════════════════════════════════════════════════════╝\n" COLOR_RESET);
    printf("\n");

    // Connection status
    if (s_metrics.connected) {
        printf(COLOR_GREEN "  ● Connected" COLOR_RESET "  ─  %s:%d\n",
               s_config.server_host.c_str(), s_config.server_port);
    } else {
        printf(COLOR_RED "  ○ Disconnected" COLOR_RESET "  ─  %s:%d  (reconnecting...)\n",
               s_config.server_host.c_str(), s_config.server_port);
    }

    printf("  %s", std::string(50, '─').c_str());
    printf("\n");

    // Server info
    printf(COLOR_CYAN "  Server:" COLOR_RESET " %-20s  " COLOR_CYAN "Version:" COLOR_RESET " %s\n",
           s_metrics.server_name.c_str(), s_metrics.server_version.c_str());

    int up_h = s_metrics.uptime_seconds / 3600;
    int up_m = (s_metrics.uptime_seconds % 3600) / 60;
    int up_s = s_metrics.uptime_seconds % 60;
    printf(COLOR_CYAN "  Uptime:" COLOR_RESET " %02d:%02d:%02d\n", up_h, up_m, up_s);

    printf("  %s\n", std::string(50, '─').c_str());

    // Players
    printf(COLOR_CYAN "  Players:" COLOR_RESET " %d / %d  (unique accounts: %d)\n",
           s_metrics.player_count, s_metrics.max_players, s_metrics.unique_accounts);

    float player_pct = s_metrics.max_players > 0 ? (float)s_metrics.player_count / s_metrics.max_players : 0;
    printf("  ");
    DrawProgressBar(player_pct, 40);
    printf(" %d%%\n", (int)(player_pct * 100));

    printf("  %s\n", std::string(50, '─').c_str());

    // Performance
    printf(COLOR_CYAN "  Performance:\n" COLOR_RESET);
    printf("    CPU:        %6.1f%%\n", s_metrics.cpu_usage);
    printf("    Memory:     %6.1f MB\n", s_metrics.memory_usage_mb);
    printf("    Tick Rate:  %6.1f /s\n", s_metrics.tick_rate);
    printf("    Threads:    %6d\n", s_metrics.active_threads);

    printf("  %s\n", std::string(50, '─').c_str());

    // Network
    printf(COLOR_CYAN "  Network:\n" COLOR_RESET);
    printf("    Connections: %6d\n", s_metrics.active_connections);
    printf("    Packets/s:   %6d\n", s_metrics.packets_per_sec);
    printf("    Bandwidth In:  %6d KB/s\n", s_metrics.bandwidth_in_kbps);
    printf("    Bandwidth Out: %6d KB/s\n", s_metrics.bandwidth_out_kbps);
    printf("    Channels:    %6d\n", s_metrics.channel_count);

    printf("  %s\n", std::string(50, '─').c_str());

    // Errors / Warnings
    if (s_metrics.error_count > 0 || s_metrics.warning_count > 0) {
        if (s_metrics.error_count > 0) {
            printf(COLOR_RED "  Errors:   %d\n" COLOR_RESET, s_metrics.error_count);
            if (!s_metrics.last_error.empty())
                printf("    Last: %s\n", s_metrics.last_error.c_str());
        }
        if (s_metrics.warning_count > 0) {
            printf(COLOR_YELLOW "  Warnings: %d\n" COLOR_RESET, s_metrics.warning_count);
            if (!s_metrics.last_warning.empty())
                printf("    Last: %s\n", s_metrics.last_warning.c_str());
        }
        printf("  %s\n", std::string(50, '─').c_str());
    }

    // Alerts
    if (s_alerts_enabled && s_metrics.connected) {
        bool alert = false;
        if (s_metrics.cpu_usage > 90.0f) {
            printf(COLOR_RED "  ⚠ ALERT: CPU usage above 90%%!\n" COLOR_RESET);
            alert = true;
        }
        if (s_metrics.memory_usage_mb > 2000.0f) {
            printf(COLOR_RED "  ⚠ ALERT: Memory usage above 2 GB!\n" COLOR_RESET);
            alert = true;
        }
        if (s_metrics.error_count > 10) {
            printf(COLOR_RED "  ⚠ ALERT: Error count above 10!\n" COLOR_RESET);
            alert = true;
        }
    }

    printf("\n");
    printf(COLOR_GRAY "  Last update: %s", std::ctime(&s_metrics.last_update));
    printf(COLOR_GRAY "  Press 'q' to quit  |  'r' to reconnect  |  'a' toggle alerts" COLOR_RESET);
    printf("\n");
}

// ── Main ───────────────────────────────────────────────────────────────────

int main(int argc, char** argv) {
    spdlog::set_level(spdlog::level::warn);

    // Parse arguments
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "-h" || arg == "--host") {
            if (i + 1 < argc) s_config.server_host = argv[++i];
        } else if (arg == "-p" || arg == "--port") {
            if (i + 1 < argc) s_config.server_port = atoi(argv[++i]);
        } else if (arg == "-i" || arg == "--interval") {
            if (i + 1 < argc) s_config.refresh_interval_ms = atoi(argv[++i]);
        } else if (arg == "-v" || arg == "--verbose") {
            s_config.verbose = true;
        } else if (arg == "--no-reconnect") {
            s_config.auto_reconnect = false;
        } else {
            fprintf(stderr, "Usage: %s [-h host] [-p port] [-i interval_ms] [-v]\n", argv[0]);
            return 1;
        }
    }

    if (!InitSockets()) return 1;

    // Start monitor thread
    std::thread monitor_thread(MonitorThreadFunc);

    // Console display loop
    printf("\n");
    printf(COLOR_CYAN "  Connecting to %s:%d..." COLOR_RESET "\n",
           s_config.server_host.c_str(), s_config.server_port);

    while (s_running) {
        DisplayMetrics();

        // Non-blocking key check
        int c = getchar();
        if (c == 'q' || c == 'Q') {
            s_running = false;
            break;
        } else if (c == 'r' || c == 'R') {
            spdlog::info("Monitor: manual reconnect requested");
            Disconnect();
        } else if (c == 'a' || c == 'A') {
            s_alerts_enabled = !s_alerts_enabled;
        }

        // Small sleep to avoid busy-waiting on getchar
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    s_running = false;
    monitor_thread.join();
    CleanupSockets();

    printf("\n");
    printf(COLOR_GREEN "  Monitor stopped.\n" COLOR_RESET);
    return 0;
}
