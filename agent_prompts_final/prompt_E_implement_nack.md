# Agent FINAL-E — Implementasi NACK Codes & Error Handling

Lokasi: /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn
Build: cmake --build build/macos-debug -j$(sysctl -n hw.ncpu)

## Tugas

Implementasi NACK response codes untuk server packet handlers. Saat ini banyak handler yang return void tanpa mengirim NACK saat operasi gagal.

## Aturan Ketat (WAJIB)

1. **📖 BACA dulu file yang akan diubah** — pahami struktur kode yang sudah ada.
2. **✅ Jika error handling SUDAH ADA** — skip. Jangan duplikasi.
3. **🔧 Jika BELUM ADA** — tambah NACK response. Gunakan pattern yang SUDAH ADA.
4. **🚫 JANGAN ubah logic bisnis** — hanya tambah error response path.
5. **🚫 JANGAN refactor** — tambah kode baru saja.
6. **🚫 JANGAN hapus kode existing** — hanya tambah.
7. **✅ Build setelah selesai** — pastikan 0 error.

## Pattern NACK yang sudah ada (contoh):

```cpp
// Pattern dari MapServer.cpp (NPC handler):
flatbuffers::FlatBufferBuilder fbb;
auto resp = CreateNpcResponse(fbb, 1 /*error*/, npc_id, action, 0, 0, 0, 0, 0);
fbb.Finish(resp);
SendPacket(conn_id, PacketType_MP_NPC_SPEECH_NACK, fbb);
```

Gunakan pattern yang SAMA PERSIS untuk semua NACK.

## File yang harus diubah

### server/map/MapServer.cpp [UPDATE]

Cari fungsi-fungsi berikut dan tambah NACK path jika belum ada:

**HandleMoveWalk/Run** — jika validasi gagal:
```cpp
// Jika sudah ada ValidationSystem::ValidateMovement gagal:
// Cek: apakah sudah kirim MoveCorrection atau NACK?
// Jika belum: tambah:
//   SendMoveCorrection(conn_id, connected_player_.pos_x, connected_player_.pos_z);
```

**HandleCombatAttack** — jika target tidak valid:
```cpp
// Cek: apakah ada NACK ketika target tidak ditemukan?
// Jika belum: tambah flatbuffers NACK
```

**HandleNpcSpeech** — jika NPC tidak ditemukan:
```cpp
// Cek: apakah NACK sudah dikirim?
// Pattern sudah ada di agent E — verifikasi dulu.
```

### client/ui/screens/GameScreen.cpp [UPDATE]

**HandlePacket switch cases** — cek apakah NACK cases ditangani:
```cpp
// Cari pattern "case PacketType_MP_*_NACK"
// Hitung berapa banyak yang sudah ada vs missing
// Untuk masing-masing NACK yang belum: tambah case dengan:
//   state_->chat_messages.push_back("Operation failed");
```

### server/map/systems/ItemSystem.cpp [UPDATE]

**UseItem, MoveItem, etc.** — tambah return false + log untuk setiap error path:
- Item not found → log error
- Invalid slot → log error
- Not enough gold → log error (khusus BuyItem)

### server/map/systems/TradingSystem.cpp [UPDATE]

**CanApplyExchange** — sudah return false? ✅ skip jika sudah.
**ConfirmExchange** — sudah kirim error? 🔧 tambah jika belum.

## Verifikasi Sebelum-Sesudah

```
SEBELUM: 20+ error cases identified — ~3 weeks estimated
SESUDAH: Hitung aktual berapa NACK yang ditambah
LAPORKAN: "Added X NACK responses across Y files"
```

## ✅ Kembalikan "Agent FINAL-E done: added X NACK codes, 0 build errors"
