#include <cstdio>
#include <cstring>
#include <vector>
#include <string>
#include <thread>
#include <chrono>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <flatbuffers/flatbuffers.h>
#include <Login_generated.h>
#include <Types_generated.h>

#pragma pack(push, 1)
struct PktHdr { uint32_t magic; uint16_t length; uint16_t type; uint16_t seq; uint32_t cksum; };
#pragma pack(pop)

int main() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8100);
    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);
    if (connect(sock, (sockaddr*)&addr, sizeof(addr)) < 0) {
        printf("Connect failed\n"); return 1;
    }
    printf("Connected\n");

    flatbuffers::FlatBufferBuilder fbb;
    std::string pw = "8c6976e5b5410415bde908bd4dee15dfb167a9c873fc4bb8a81f6f2ab448a918";
    std::vector<uint8_t> pw_vec(pw.begin(), pw.end());
    auto req = luna::protocol::CreateLoginRequestDirect(fbb, "admin", &pw_vec);
    fbb.Finish(req);

    PktHdr hdr{0x4C4E50, (uint16_t)fbb.GetSize(), 0x0101, 0, 0};
    ::send(sock, &hdr, sizeof(hdr), 0);
    ::send(sock, fbb.GetBufferPointer(), fbb.GetSize(), 0);
    printf("Sent %zu bytes\n", sizeof(hdr) + fbb.GetSize());

    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    char buf[4096];
    auto n = recv(sock, buf, sizeof(buf), MSG_DONTWAIT);
    if (n > 0) {
        auto* rhdr = (PktHdr*)buf;
        printf("Response: type=0x%04x len=%u\n", rhdr->type, rhdr->length);
        auto resp = flatbuffers::GetRoot<luna::protocol::LoginResponse>(buf + sizeof(PktHdr));
        printf("Result: %d\n", (int)resp->result());
        if (resp->session_token()) printf("Token: %s\n", resp->session_token()->c_str());
    } else {
        printf("No response (n=%ld)\n", n);
    }

    close(sock);
    return 0;
}
