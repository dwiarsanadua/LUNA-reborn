# Agent-05 — Error Handling: NACK Codes untuk Semua Subsystem

Lokasi: /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn
Build: cmake --build build/macos-debug -j$(sysctl -n hw.ncpu)

## Tugas

Dari analisa Old source, setiap subsystem punya NACK codes spesifik untuk berbagai error case. Reborn sudah punya beberapa NACK (NPC handler, Vehicle handler). Perlu tambah NACK untuk subsystem yang masih missing.

## Aturan Ketat

1. BACA dulu file sebelum ubah — pahami pattern NACK yang sudah ada
2. ✅ Jika NACK SUDAH ADA → skip
3. 🔧 Jika BELUM ADA → tambah dengan pattern yang SAMA

## Pattern NACK yang sudah ada (contoh dari MapServer):

```cpp
// Pattern: FlatBuffers response dengan result code != 0
flatbuffers::FlatBufferBuilder fbb;
auto resp = CreateXxxResponse(fbb, 1 /*error*/, ...);
fbb.Finish(resp);
SendPacket(conn_id, PacketType_MP_XXX_NACK, fbb);
```

## File yang harus dicek & diupdate

### client/ui/screens/GameScreen.cpp
Cari switch cases untuk NACK:
```bash
grep -c "case.*NACK" client/ui/screens/GameScreen.cpp
```
Yang mungkin masih MISSING:
- ITEM_USE_NACK, ITEM_MOVE_NACK, ITEM_DISCARD_NACK
- SKILL_START_NACK, SKILL_CANCEL_NACK
- PARTY_CREATE_NACK, PARTY_ADD_NACK, PARTY_INVITE_NACK
- GUILD_CREATE_NACK, GUILD_ADDMEMBER_NACK
- QUEST_START_NACK, QUEST_END_NACK
- EXCHANGE_*_NACK
- STORAGE_*_NACK
- NPC_SPEECH_NACK (✅ mungkin sudah ada)

Untuk setiap NACK yang belum ada, tambah case:
```cpp
case luna::protocol::PacketType_MP_XXX_NACK: {
    state_->chat_messages.push_back("Operation failed");
    return true;
}
```

### server/map/MapServer.cpp
Cek handler untuk NACK response:
```bash
grep -c "NACK" server/map/MapServer.cpp
```
Untuk handler SYN yang gagal, kirim NACK:
```cpp
// Contoh untuk item use:
if (!ItemSystem::UseItem(...)) {
    flatbuffers::FlatBufferBuilder fbb;
    auto resp = CreateInventoryUseResponse(fbb, false, slot, 0);
    fbb.Finish(resp);
    SendPacket(conn_id, PacketType_MP_ITEM_USE_NACK, fbb);
    return;
}
```

## Output

✅ Kembalikan: "Agent-05 done: added X NACK cases in GameScreen, Y NACK sends in MapServer"
