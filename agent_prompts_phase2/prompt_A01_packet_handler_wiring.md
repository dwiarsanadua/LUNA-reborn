# Agent A01 — Packet Handler Wiring Completion

Lokasi: /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn
Build: cmake --build build/macos-debug -j$(sysctl -n hw.ncpu)

## Konteks

Dari 10 kategori packet utama, 7 sudah fully wired. Yang masih missing:
1. **NPC**: `NPC_SPEECH_SYN` — tidak ada handler di server maupun client
2. **MOVE**: `MOVE_STOP`, `MOVE_TELEPORT` — tidak ada handler di server
3. **VEHICLE**: ~20 packet types — server handler tidak ada sama sekali
4. **INVENTORY**: `ITEM_APPEARANCE_REMOVE` — 1 handler missing

## File yang harus diubah

### server/map/MapServer.cpp [UPDATE]

**🔴 NPC_SPEECH_SYN handler** (Severity: H, Effort: 2 days)
- Tambah handler untuk `PacketType_MP_NPC_SPEECH_SYN`
- Referensi: NPC.fbs (NpcRequest, NpcResponse schema sudah ada)
- Flow: Parse NpcRequest → load NPC data → reply NpcResponse
- Gunakan pola yang sama seperti handler lain di MapServer (if-else chain)

Pseudo-code:
```cpp
} else if (type == PacketType_MP_NPC_SPEECH_SYN) {
    auto req = flatbuffers::GetRoot<NpcRequest>(payload.data());
    uint32_t npc_id = req->npc_id();
    // Load NPC template from DB
    auto npc = db_.GetNpcTemplate(npc_id);
    if (!npc) { sendNack(conn_id, type); return; }
    // Build response based on action
    flatbuffers::FlatBufferBuilder fbb;
    auto dialog = fbb.CreateString(npc->dialog_text);
    auto resp = CreateNpcResponse(fbb, 0, npc_id, req->action(), dialog, 0, 0, 0, 0);
    fbb.Finish(resp);
    SendPacket(conn_id, PacketType_MP_NPC_SPEECH_ACK, fbb);
```

**🟡 MOVE_STOP handler** (Severity: M, Effort: 1 day)
- Tambah handler untuk `PacketType_MP_MOVE_STOP`
- Update posisi terakhir di grid, broadcast ENTITY_TRANSFORM dengan velocity=0

**🔴 MOVE_TELEPORT handler** (Severity: M, Effort: 1 day)
- Validasi: cek map_id valid, posisi valid
- Update entity transform, broadcast ke grid baru dan grid lama

**🔴 VEHICLE handler** (Severity: M, Effort: 3 days)
- Tambah handler untuk SEMUA MP_VEHICLE_*_SYN types:
  - MP_VEHICLE_SUMMON_SYN → summon vehicle
  - MP_VEHICLE_UNSUMMON_SYN → unsummon
  - MP_VEHICLE_MOUNT_REQUEST_SYN → mount request
  - MP_VEHICLE_MOUNT_ALLOW_SYN → allow mount
  - MP_VEHICLE_DISMOUNT_SYN → dismount
  - MP_VEHICLE_GET_OPTION_SYN → get toll options
- Vehicle.fbs schema sudah tersedia

**🟡 ITEM_APPEARANCE_REMOVE handler** (Severity: L, Effort: 4 hrs)
- Tambah handler untuk `PacketType_MP_ITEM_APPEARANCE_REMOVE`

### client/ui/screens/GameScreen.cpp [UPDATE]

**🔴 NPC_SPEECH_ACK/NACK** (Severity: H, Effort: 1 day)
- Tambah case untuk `PacketType_MP_NPC_SPEECH_ACK` dan `_NACK`
- Parse NpcResponse → tampilkan dialog di NPCDialog

## Aturan

1. BACA file Reborn yang tercantum sebelum mengubah
2. IKUTI pattern yang sudah ada (if-else chain di MapServer, switch-case di GameScreen)
3. JANGAN build atau compile
4. ✅ Kembalikan pesan "Agent A01 done: packet handlers wired"
