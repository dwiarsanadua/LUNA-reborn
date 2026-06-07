#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <network/TcpServer.hpp>
#include <network/LoginHandler.hpp>
#include <network/CharacterDB.hpp>
#include <flatbuffers/flatbuffers.h>
#include <Login_generated.h>
#include <Character_generated.h>
#include <Chat_generated.h>
#include <Inventory_generated.h>
#include <PacketType_generated.h>
#include <cstdlib>
#include <csignal>
#include <memory>
#include <thread>
#include <chrono>
#include <unordered_map>
#include <vector>
#include <mutex>
#include <game/Log.h>
#include <sys/stat.h>

static std::unique_ptr<TcpServer> g_server;
static std::unique_ptr<LoginHandler> g_login;
static std::unique_ptr<CharacterDB> g_char_db;
static std::unordered_map<uint32_t, std::string> g_user_map;
static std::unordered_map<uint32_t, std::shared_ptr<TcpConnection>> g_conns;
static std::mutex g_conn_mutex;

void signal_handler(int) {
    spdlog::info("Shutting down...");
    if (g_server) g_server->Stop();
}

static void SendResponse(std::shared_ptr<TcpConnection> conn, uint16_t type,
                          const uint8_t* payload, size_t len) {
    PacketHeader hdr;
    hdr.magic = 0x4C4E50;
    hdr.length = static_cast<uint16_t>(len);
    hdr.type = type;
    hdr.sequence = 0;
    hdr.checksum = 0;
    auto& buf = conn->write_buffer;
    buf.clear();
    buf.insert(buf.end(), reinterpret_cast<uint8_t*>(&hdr),
               reinterpret_cast<uint8_t*>(&hdr) + sizeof(hdr));
    buf.insert(buf.end(), payload, payload + len);
    g_server->Send(conn, buf.data(), buf.size());
}

static void BroadcastChat(const std::string& sender, const std::string& msg, uint16_t channel) {
    flatbuffers::FlatBufferBuilder fbb;
    auto chatMsg = luna::protocol::CreateChatMessageDirect(fbb, 0, sender.c_str(), msg.c_str(), static_cast<luna::protocol::ChatChannel>(channel), 0);
    fbb.Finish(chatMsg);
    PacketHeader hdr;
    hdr.magic = 0x4C4E50;
    hdr.length = static_cast<uint16_t>(fbb.GetSize());
    hdr.type = luna::protocol::PacketType_MP_CHAT_ALL_SYN; // 0x0501 legacy
    hdr.sequence = 0;
    hdr.checksum = 0;
    std::lock_guard<std::mutex> lock(g_conn_mutex);
    for (auto& [cid, conn] : g_conns) {
        auto& buf = conn->write_buffer;
        buf.clear();
        buf.insert(buf.end(), reinterpret_cast<uint8_t*>(&hdr), reinterpret_cast<uint8_t*>(&hdr) + sizeof(hdr));
        buf.insert(buf.end(), fbb.GetBufferPointer(), fbb.GetBufferPointer() + fbb.GetSize());
        g_server->Send(conn, buf.data(), buf.size());
    }
}

static bool ValidateSession(std::shared_ptr<TcpConnection> conn) {
    return g_user_map.find(conn->id) != g_user_map.end();
}

int main(int argc, char* argv[]) {
    mkdir("logs", 0777);
    Luna::InitLog("agent", "agent.log");
    spdlog::info("LUNA Plus Reborn — AgentServer v{}", "1.0.0");

    int port = 8100;
    if (argc > 1) port = std::atoi(argv[1]);

    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    g_login = std::make_unique<LoginHandler>();
    if (!g_login->Init("data/luna_member.db")) {
        spdlog::error("Failed to initialize login handler");
        return 1;
    }

    g_char_db = std::make_unique<CharacterDB>();
    if (!g_char_db->Init("data/luna_char.db")) {
        spdlog::error("Failed to initialize character DB");
        return 1;
    }

    g_server = std::make_unique<TcpServer>();
    g_server->SetNewConnectionCallback([](std::shared_ptr<TcpConnection> conn) {
        spdlog::info("New connection: id={}", conn->id);
        std::lock_guard<std::mutex> lock(g_conn_mutex);
        g_conns[conn->id] = conn;
    });

    g_server->SetDataCallback([](std::shared_ptr<TcpConnection> conn, uint8_t* data, size_t len, uint16_t type) {
        using namespace luna::protocol;
        switch (type) {
        case PacketType_MP_USERCONN_LOGIN_SYN: { // 1537
            std::string token;
            if (g_login->HandleLogin(conn, data, len, &token)) {
                auto req = flatbuffers::GetRoot<LoginRequest>(data);
                std::string uname = req->username() ? req->username()->str() : "";
                g_user_map[conn->id] = uname;
                auto& buf = conn->write_buffer;
                g_server->Send(conn, buf.data(), buf.size());
                spdlog::info("Login OK: user={} conn={}", uname, conn->id);
            } else {
                g_user_map.erase(conn->id);
            }
            break;
        }
        case PacketType_MP_USERCONN_CHARACTERLIST_SYN: { // 1546
            if (!ValidateSession(conn)) break;
            auto it = g_user_map.find(conn->id);
            std::string account_id = (it != g_user_map.end()) ? it->second : "admin";
            auto chars = g_char_db->GetCharacters(account_id);
            flatbuffers::FlatBufferBuilder fbb;
            std::vector<flatbuffers::Offset<CharacterInfo>> charVec;
            for (auto& c : chars) {
                auto pos = Vec3(c.pos_x, c.pos_y, c.pos_z);
                charVec.push_back(CreateCharacterInfoDirect(
                    fbb, c.id, c.name.c_str(), c.level, 0, 0, c.map_id, &pos));
            }
            auto resp = CreateCharacterListResponseDirect(fbb, &charVec, 4);
            fbb.Finish(resp);
            SendResponse(conn, PacketType_MP_USERCONN_CHARACTERLIST_ACK, fbb.GetBufferPointer(), fbb.GetSize());
            break;
        }
        case PacketType_MP_USERCONN_CHARACTER_MAKE_SYN: { // Character creation mapping
            if (!ValidateSession(conn)) break;
            auto req = flatbuffers::GetRoot<CreateCharacterRequest>(data);
            CharInfo info;
            info.account_id = g_user_map[conn->id];
            info.name = req->name() ? req->name()->str() : "NewChar";
            info.char_class = req->class_();
            info.gender = req->gender();
            info.level = 1;
            info.map_id = 51;
            info.pos_x = 0; info.pos_y = 0; info.pos_z = 0;
            info.hp = 500; info.max_hp = 500;
            uint32_t new_id = g_char_db->CreateCharacter(info);
            flatbuffers::FlatBufferBuilder fbb;
            auto pos = Vec3(0, 0, 0);
            auto ci = CreateCharacterInfoDirect(fbb, new_id, info.name.c_str(), 1, info.char_class, info.gender, 51, &pos);
            auto resp = CreateCreateCharacterResponse(fbb, 0, ci);
            fbb.Finish(resp);
            SendResponse(conn, PacketType_MP_USERCONN_CHARACTER_MAKE_ACK, fbb.GetBufferPointer(), fbb.GetSize());
            spdlog::info("Character created: {} (id={})", info.name, new_id);
            break;
        }
        case PacketType_MP_USERCONN_GAMEIN_SYN: { // 1558
            if (!ValidateSession(conn)) break;
            auto req = flatbuffers::GetRoot<EnterWorldRequest>(data);
            uint32_t char_id = req->character_id();
            auto c = g_char_db->GetCharacter(char_id);
            auto items = g_char_db->LoadItems(char_id);
            c.items = items;

            flatbuffers::FlatBufferBuilder fbb;
            auto pos = Vec3(c.pos_x, c.pos_y, c.pos_z);
            auto resp = CreateEnterWorldResponse(fbb, 0, c.map_id, &pos, 0);
            fbb.Finish(resp);
            SendResponse(conn, PacketType_MP_USERCONN_GAMEIN_ACK, fbb.GetBufferPointer(), fbb.GetSize());

            // Send inventory data
            {
                flatbuffers::FlatBufferBuilder invFbb;
                std::vector<flatbuffers::Offset<InventorySlot>> slotVec;
                for (auto& item : items) {
                    slotVec.push_back(CreateInventorySlot(invFbb, item.slot, item.item_id, item.count, item.enchant));
                }
                auto invData = CreateInventoryDataDirect(invFbb, c.gold, &slotVec);
                invFbb.Finish(invData);
                SendResponse(conn, PacketType_MP_ITEM_STORAGEITEM_INFO, invFbb.GetBufferPointer(), invFbb.GetSize());
            }
            spdlog::info("EnterWorld: char={} map={} items={}", char_id, c.map_id, items.size());
            break;
        }
        case PacketType_MP_CHAT_ALL_SYN: { // 0x0501
            auto it = g_user_map.find(conn->id);
            std::string sender = (it != g_user_map.end()) ? it->second : "Unknown";
            auto msg = flatbuffers::GetRoot<ChatMessage>(data);
            std::string text = msg->message() ? msg->message()->str() : "";
            uint16_t channel = static_cast<uint16_t>(msg->channel());
            spdlog::info("Chat [{}] {}: {}", channel, sender, text);
            BroadcastChat(sender, text, channel);
            break;
        }
        default:
            spdlog::info("Packet from {}: type=0x{:04X} len={}", conn->id, type, len);
        }
    });

    g_server->SetCloseCallback([](std::shared_ptr<TcpConnection> conn) {
        if (g_login) g_login->HandleLogout(conn->id);
        g_user_map.erase(conn->id);
        {
            std::lock_guard<std::mutex> lock(g_conn_mutex);
            g_conns.erase(conn->id);
        }
        spdlog::info("Connection closed: id={}", conn->id);
    });

    if (!g_server->Start(static_cast<uint16_t>(port))) {
        spdlog::error("Failed to start server on port {}", port);
        return 1;
    }

    spdlog::info("AgentServer running on port {}. Press Ctrl+C to stop.", port);
    while (g_server && g_server->IsRunning()) {
        g_server->Poll();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    return 0;
}
