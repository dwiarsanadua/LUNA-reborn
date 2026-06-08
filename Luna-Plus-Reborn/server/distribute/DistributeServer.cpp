#include "DistributeServer.h"
#include "engine/network/NetworkLayer.h"
#include <MapChange_generated.h>
#include <Chat_generated.h>
#include <PacketType_generated.h>
#include <flatbuffers/flatbuffers.h>
#include <spdlog/spdlog.h>
#include <sstream>

DistributeServer::DistributeServer()
    : network_(std::make_unique<NetworkLayer>())
    , last_heartbeat_check_(std::chrono::steady_clock::now())
{}

DistributeServer::~DistributeServer() { Shutdown(); }

void DistributeServer::RegisterMap(uint16_t map_id, uint16_t port, const std::string& name) {
    ServerInfo info;
    info.server_id = next_server_id_++;
    info.map_id = map_id;
    info.port = port;
    info.name = name;
    info.host = "127.0.0.1";
    info.online = true;
    info.last_heartbeat = std::chrono::steady_clock::now();

    for (auto it = servers_.begin(); it != servers_.end(); ++it) {
        if (it->second.map_id == map_id) {
            it->second.port = port;
            it->second.name = name;
            it->second.online = true;
            it->second.last_heartbeat = info.last_heartbeat;
            spdlog::info("DistributeServer: updated map {} on port {}", map_id, port);
            return;
        }
    }
    servers_[info.server_id] = info;
    spdlog::info("DistributeServer: registered map {} ({}) on port {}", map_id, name, port);
}

const ServerInfo* DistributeServer::FindMapServer(uint16_t map_id) const {
    for (const auto& [id, sv] : servers_) {
        (void)id;
        if (sv.map_id == map_id && sv.online) return &sv;
    }
    return nullptr;
}

void DistributeServer::RegisterDefaultMaps() {
    RegisterMap(51, 8251, "Alker Harbor");
    RegisterMap(13, 8213, "Red Orc Outpost");
    RegisterMap(20, 8220, "Alker Harbor Town");
}

bool DistributeServer::Initialize(uint16_t port) {
    RegisterDefaultMaps();

    if (!network_->Initialize(port)) {
        spdlog::error("DistributeServer: network init failed on port {}", port);
        return false;
    }

    network_->SetReceiveCallback([this](uint16_t type, const uint8_t* data, size_t len) {
        HandlePacket(type, data, len);
    });

    spdlog::info("DistributeServer: initialized on port {} ({} maps)", port, servers_.size());
    return true;
}

void DistributeServer::HandlePacket(uint16_t type, const uint8_t* payload, size_t len) {
    using namespace luna::protocol;

    if (type == PacketType_MP_MAPSERVER_REGISTER_SYN) {
        auto req = flatbuffers::GetRoot<MapServerRegister>(payload);
        std::string name = req->name() ? req->name()->str() : ("Map_" + std::to_string(req->map_id()));
        RegisterMap(req->map_id(), req->port(), name);
        flatbuffers::FlatBufferBuilder fbb;
        auto ack = CreateMapServerRegister(fbb, req->map_id(), req->port(),
            fbb.CreateString(name));
        fbb.Finish(ack);
        network_->SendPacket(PacketType_MP_MAPSERVER_REGISTER_ACK,
            fbb.GetBufferPointer(), fbb.GetSize());
        return;
    }

    if (type == PacketType_MP_MAPSERVER_LIST_SYN) {
        flatbuffers::FlatBufferBuilder fbb;
        std::vector<flatbuffers::Offset<MapServerListEntry>> entries;
        for (const auto& [id, sv] : servers_) {
            (void)id;
            entries.push_back(CreateMapServerListEntry(fbb, sv.map_id, sv.port,
                fbb.CreateString(sv.name), static_cast<uint16_t>(sv.player_count), sv.online));
        }
        auto resp = CreateMapServerListResponse(fbb, fbb.CreateVector(entries));
        fbb.Finish(resp);
        network_->SendPacket(PacketType_MP_MAPSERVER_LIST_ACK,
            fbb.GetBufferPointer(), fbb.GetSize());
        return;
    }

    if (type == PacketType_MP_USERCONN_CHANGEMAP_SYN) {
        auto req = flatbuffers::GetRoot<ChangeMapRequest>(payload);
        uint16_t target = req->target_map_id();
        const ::ServerInfo* sv = FindMapServer(target);
        flatbuffers::FlatBufferBuilder fbb;
        if (!sv) {
            Vec3 pos{0, 0, 0};
            auto resp = CreateChangeMapResponse(fbb, 1, target, 0, &pos);
            fbb.Finish(resp);
            network_->SendPacket(PacketType_MP_USERCONN_CHANGEMAP_NACK,
                fbb.GetBufferPointer(), fbb.GetSize());
            spdlog::warn("DistributeServer: changemap to {} failed — map offline", target);
            return;
        }
        Vec3 pos{0, 0, 0};
        if (target == 13) { pos = Vec3(12.0f, 0.0f, 8.0f); }
        else if (target == 20) { pos = Vec3(6.0f, 0.0f, 4.0f); }
        auto resp = CreateChangeMapResponse(fbb, 0, sv->map_id, sv->port, &pos);
        fbb.Finish(resp);
        network_->SendPacket(PacketType_MP_USERCONN_CHANGEMAP_ACK,
            fbb.GetBufferPointer(), fbb.GetSize());
        spdlog::info("DistributeServer: changemap -> map {} port {}", sv->map_id, sv->port);
        (void)len;
        return;
    }

    if (type == PacketType_MP_CHAT_WHISPER_SYN) {
        auto req = flatbuffers::GetRoot<ChatMessage>(payload);
        std::string target = req->sender_name() ? req->sender_name()->str() : "";
        std::string text = req->message() ? req->message()->str() : "";
        spdlog::info("DistributeServer: whisper route to '{}' ({} bytes)", target, text.size());
        std::string reply = "To [" + target + "]: " + text + " (routed via Distribute)";
        flatbuffers::FlatBufferBuilder fbb;
        auto msg = CreateChatMessageDirect(fbb, 0, "System", reply.c_str(), ChatChannel_Whisper, 0);
        fbb.Finish(msg);
        network_->SendPacket(PacketType_MP_CHAT_WHISPER_ACK,
            fbb.GetBufferPointer(), fbb.GetSize());
        return;
    }

    spdlog::debug("DistributeServer: unhandled packet 0x{:04X} ({} bytes)", type, len);
}

void DistributeServer::Shutdown() {
    network_->Shutdown();
    servers_.clear();
    player_server_map_.clear();
}

void DistributeServer::Update() {
    network_->Update();

    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - last_heartbeat_check_);
    if (elapsed.count() >= 30) {
        last_heartbeat_check_ = now;
        for (auto it = servers_.begin(); it != servers_.end(); ) {
            auto age = std::chrono::duration_cast<std::chrono::seconds>(
                now - it->second.last_heartbeat);
            if (age.count() > 120) {
                it = servers_.erase(it);
                continue;
            }
            ++it;
        }
    }
}

void DistributeServer::HandleChat(const std::string& sender, int channel, const std::string& message) {
    spdlog::info("Chat [ch{}] {}: {}", channel, sender, message);
    (void)sender; (void)channel; (void)message;
}

void DistributeServer::HandleWhisper(const std::string& sender, const std::string& target, const std::string& message) {
    (void)sender; (void)target; (void)message;
}

void DistributeServer::BroadcastServerList(const std::vector<ServerInfo>& servers) {
    spdlog::info("DistributeServer: broadcasting {} server entries", servers.size());
}

void DistributeServer::RoutePacket(int target_server_id, const uint8_t* data, size_t size) {
    (void)target_server_id; (void)data; (void)size;
}
