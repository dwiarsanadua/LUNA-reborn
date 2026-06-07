#include <spdlog/spdlog.h>
#include <network/TcpServer.hpp>
#include <network/LoginHandler.hpp>
#include <thread>
#include <chrono>

int main() {
    spdlog::info("Login Handler Test");

    LoginHandler login;
    if (!login.Init("data/luna_member.db")) {
        spdlog::error("Failed to open account DB");
        return 1;
    }

    spdlog::info("Login handler ready. Start AgentServer and connect on port 8100");
    spdlog::info("Test accounts: admin:admin, test:test, demo:demo");
    return 0;
}
