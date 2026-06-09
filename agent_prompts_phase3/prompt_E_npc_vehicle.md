# Agent E — NPC & VEHICLE Server Handler Wiring 🟡 HIGH

Lokasi: /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn
Build: cmake --build build/macos-debug -j$(sysctl -n hw.ncpu)

## Masalah

Dari scan packet handler wiring:
1. **NPC_SPEECH_SYN**: Defined di protocol, schema NPC.fbs sudah ada, tapi 0 handler di MapServer dan GameScreen
2. **VEHICLE**: Schema Vehicle.fbs + 20 packet types sudah di PacketType.fbs, client GameScreen sudah handle sebagian besar ACK, tapi MapServer punya 0 handler

## File yang harus diubah

### server/map/MapServer.cpp [UPDATE]

**🔴 NPC_SPEECH_SYN handler:**

Cari section if-else chain untuk packet handling. Tambah:

```cpp
} else if (type == PacketType_MP_NPC_SPEECH_SYN) {
    auto req = flatbuffers::GetRoot<NpcRequest>(payload.data());
    uint32_t npc_id = req->npc_id();
    auto action = req->action();
    
    // Cari NPC template dari DB
    auto npc = db_.GetNpcTemplate(npc_id);
    if (!npc) {
        // Send NACK
        flatbuffers::FlatBufferBuilder fbb;
        auto resp = CreateNpcResponse(fbb, 1, npc_id, action, 0, 0, 0, 0, 0);
        fbb.Finish(resp);
        SendPacket(conn_id, PacketType_MP_NPC_SPEECH_NACK, fbb);
        return;
    }
    
    flatbuffers::FlatBufferBuilder fbb;
    auto dialog = fbb.CreateString(npc->dialog_text);
    
    // Handle berdasarkan action type
    flatbuffers::Offset<NpcResponse> resp;
    switch (action) {
        case NpcAction::Shop: {
            // Load shop items dari DB
            std::vector<flatbuffers::Offset<NpcShopItem>> items;
            auto shop_items = db_.GetNpcShopItems(npc_id);
            for (auto& item : shop_items) {
                items.push_back(CreateNpcShopItem(fbb, item.item_id, item.price, item.stock, false));
            }
            auto items_vec = fbb.CreateVector(items);
            resp = CreateNpcResponse(fbb, 0, npc_id, action, dialog, items_vec, 0, 0, 0);
            break;
        }
        case NpcAction::Quest: {
            // Load quest IDs dari NPC
            auto quests = db_.GetNpcQuestIds(npc_id);
            auto qvec = fbb.CreateVector(quests);
            resp = CreateNpcResponse(fbb, 0, npc_id, action, dialog, 0, 0, qvec, 0);
            break;
        }
        case NpcAction::ChangeMap: {
            // Load warp destinations
            std::vector<flatbuffers::Offset<NpcChangeMapDestination>> dests;
            // ... populate from DB
            auto dvec = fbb.CreateVector(dests);
            resp = CreateNpcResponse(fbb, 0, npc_id, action, dialog, 0, dvec, 0, 0);
            break;
        }
        case NpcAction::Recall: {
            // Load recall destinations
            std::vector<flatbuffers::Offset<NpcRecallDestination>> recalls;
            auto rvec = fbb.CreateVector(recalls);
            resp = CreateNpcResponse(fbb, 0, npc_id, action, dialog, 0, 0, 0, rvec);
            break;
        }
        default: {
            resp = CreateNpcResponse(fbb, 0, npc_id, action, dialog, 0, 0, 0, 0);
            break;
        }
    }
    fbb.Finish(resp);
    SendPacket(conn_id, PacketType_MP_NPC_SPEECH_ACK, fbb);
```

**🔴 VEHICLE handlers:**

Tambah handler untuk masing-masing VEHICLE SYN type di MapServer:

```cpp
} else if (type >= PacketType_MP_VEHICLE_SUMMON_SYN && 
           type <= PacketType_MP_VEHICLE_PASSENGER_ACK) {
    HandleVehiclePacket(conn_id, type, payload);
}
```

Buat fungsi baru `HandleVehiclePacket()` di MapServer.cpp:

```cpp
void MapServer::HandleVehiclePacket(uint32_t conn_id, PacketType type, 
                                     const std::vector<uint8_t>& payload) {
    auto entity = GetEntityByConnection(conn_id);
    if (!registry_.valid(entity)) return;
    
    switch (type) {
        case PacketType_MP_VEHICLE_SUMMON_SYN: {
            // Cek inventory untuk item vehicle
            // Buat entity vehicle di posisi player
            // Broadcast VEHICLE_SUMMON_ACK
            break;
        }
        case PacketType_MP_VEHICLE_MOUNT_REQUEST_SYN: {
            auto req = flatbuffers::GetRoot<VehicleMountRequest>(payload.data());
            auto vehicle_id = req->vehicle_id();
            auto seat = req->seat_index();
            // Cek apakah seat tersedia
            // Broadcast mount ACK ke owner + passenger
            break;
        }
        case PacketType_MP_VEHICLE_MOUNT_ALLOW_SYN: {
            // Owner mengizinkan mount
            // Broadcast VEHICLE_MOUNT_ALLOW_ACK
            break;
        }
        case PacketType_MP_VEHICLE_DISMOUNT_SYN: {
            // Hapus passenger dari vehicle
            // Broadcast VEHICLE_DISMOUNT_ACK
            break;
        }
        // ... dan seterusnya untuk semua VEHICLE type
    }
}
```

### client/ui/screens/GameScreen.cpp [UPDATE]

NPC speech ACK/NACK (tambah switch case):

```cpp
case luna::protocol::PacketType_MP_NPC_SPEECH_ACK: {
    auto resp = flatbuffers::GetRoot<NpcResponse>(payload.data());
    if (npcdialog_) {
        npcdialog_->ShowNpcResponse(resp);
    }
    return true;
}
case luna::protocol::PacketType_MP_NPC_SPEECH_NACK: {
    state_->chat_messages.push_back("NPC interaction failed");
    return true;
}
```

## Aturan

1. LOAD file dulu sebelum edit — baca MapServer.cpp, GameScreen.cpp
2. IKUTI pattern if-else chain yang sudah ada di MapServer
3. Vehicle schema sudah di `/game/network/protocol/Vehicle.fbs`
4. NPC schema sudah di `/game/network/protocol/NPC.fbs`
5. JANGAN build atau compile
6. ✅ Kembalikan "Agent E done: NPC & VEHICLE handlers wired"
