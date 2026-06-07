#include <cstdio>
#include <cstring>
#include <vector>
#include <string>
#include <thread>
#include <chrono>
#include <atomic>
#include <mutex>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <flatbuffers/flatbuffers.h>
#include <Login_generated.h>

#pragma pack(push, 1)
struct PktHdr { uint32_t magic; uint16_t length; uint16_t type; uint16_t seq; uint32_t cksum; };
#pragma pack(pop)

static std::atomic<int> g_ok{0}, g_fail{0}, g_done{0};
static std::mutex g_print;

static void bot(int id, const char* user, const char* pw) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) { g_fail++; g_done++; return; }
    struct timeval tv{5, 0};
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8100);
    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);
    if (connect(sock, (sockaddr*)&addr, sizeof(addr)) < 0) {
        std::lock_guard<std::mutex> lock(g_print);
        if (id < 10 || id % 100 == 0)
            printf("Bot %d: connect FAILED (errno=%d)\n", id, errno);
        g_fail++; close(sock); g_done++; return;
    }

    flatbuffers::FlatBufferBuilder fbb;
    std::vector<uint8_t> pv(pw, pw + strlen(pw));
    auto req = luna::protocol::CreateLoginRequestDirect(fbb, user, &pv);
    fbb.Finish(req);
    PktHdr hdr{0x4C4E50, (uint16_t)fbb.GetSize(), 0x0101, 0, 0};
    ::send(sock, &hdr, sizeof(hdr), 0);
    ::send(sock, fbb.GetBufferPointer(), fbb.GetSize(), 0);

    auto t0 = std::chrono::steady_clock::now();
    char buf[4096];
    auto n = recv(sock, buf, sizeof(buf), 0);
    if (n > 0) {
        auto* rh = (PktHdr*)buf;
        auto resp = flatbuffers::GetRoot<luna::protocol::LoginResponse>(buf + sizeof(PktHdr));
        auto dt = std::chrono::duration<float>(std::chrono::steady_clock::now() - t0).count();
        if (resp->result() == luna::protocol::LoginResult_Success) {
            g_ok++;
            if (id < 10 || id % 100 == 0) {
                std::lock_guard<std::mutex> lock(g_print);
                printf("Bot %d: LOGIN OK (%s) in %.1fms\n", id, user, dt * 1000);
            }
        } else {
            std::lock_guard<std::mutex> lock(g_print);
            printf("Bot %d: LOGIN FAILED (%s)\n", id, user);
            g_fail++;
        }
    } else {
        if (id < 10 || id % 100 == 0) {
            std::lock_guard<std::mutex> lock(g_print);
            printf("Bot %d: timeout\n", id);
        }
        g_fail++;
    }
    close(sock);
    g_done++;
}

int main(int argc, char** argv) {
    int count = 100;
    if (argc > 1) count = std::atoi(argv[1]);
    if (count < 1) count = 1;
    if (count > 2000) count = 2000;

    printf("=== Stress Test: %d concurrent logins ===\n\n", count);
    const char* users[] = {"admin", "test", "demo"};
    const char* pws[] = {
        "8c6976e5b5410415bde908bd4dee15dfb167a9c873fc4bb8a81f6f2ab448a918",
        "9f86d081884c7d659a2feaa0c55ad015a3bf4f1b2b0b822cd15d6c15b0f00a08",
        "2a97516c354b68848cdbd8f54a226a0a55b21f138b1d3665ae1f3c552d6c7def",
    };
    auto start = std::chrono::steady_clock::now();
    std::vector<std::thread> bots;
    for (int i = 0; i < count; i++) {
        bots.emplace_back(bot, i, users[i % 3], pws[i % 3]);
    }
    for (auto& t : bots) t.join();
    auto elapsed = std::chrono::duration<float>(std::chrono::steady_clock::now() - start).count();

    printf("\n=== Results ===");
    printf("\n  Total:  %d", g_ok.load() + g_fail.load());
    printf("\n  OK:     %d", g_ok.load());
    printf("\n  FAIL:   %d", g_fail.load());
    printf("\n  Time:   %.2fs", elapsed);
    printf("\n  Rate:   %.0f conn/s", (g_ok.load() + g_fail.load()) / elapsed);
    printf("\n  Avg:    %.1fms/conn\n", elapsed / std::max(1, g_ok.load() + g_fail.load()) * 1000);
    return (g_fail.load() == 0) ? 0 : 1;
}
