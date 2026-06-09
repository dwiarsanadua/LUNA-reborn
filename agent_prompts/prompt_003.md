# Agent #003 — Tugas Adaptasi Luna Old → Reborn

Lokasi: /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn

Build: cmake --build build/macos-debug -j$(sysctl -n hw.ncpu)

---

## File yang harus diubah


### database/schema_game_sqlite.sql [UPDATE]

- **🔴 TB_CHARACTER: add battle_style column** (Severity: L, Effort: 2 hrs)
  - Referensi Old: `TB_CHARACTER.battle_style`
  - Baca perbandingan kolom di DB_QUERY_MAPPING.md

- **🟡 USP_ITEM_LOAD: storage_type → StorageType** (Severity: M, Effort: 2 hrs)
  - Referensi Old: `TB_ITEM.storage_type`
  - Baca perbandingan kolom di DB_QUERY_MAPPING.md

- **🟡 USP_GUILD_LOAD: add emblem_data** (Severity: M, Effort: 1 day)
  - Referensi Old: `TB_GUILD.emblem_data`
  - Baca perbandingan kolom di DB_QUERY_MAPPING.md

- **🟡 USP_FRIEND_LIST: memo → Memo** (Severity: L, Effort: 2 hrs)
  - Referensi Old: `TB_FRIEND.memo`
  - Baca perbandingan kolom di DB_QUERY_MAPPING.md

- **🔴 ~30 remaining SP not mapped** (Severity: H, Effort: 3 weeks)
  - Referensi Old: `Various USP_*`
  - Baca perbandingan kolom di DB_QUERY_MAPPING.md

### game/network/protocol/Housing.fbs [BARU]

- **🔴 MP_HOUSE: new schema + handler** (Severity: M, Effort: 3 days)
  - Referensi Old: `MP_HOUSE (20+ packets)`
  - Baca schema `.fbs` dan update field-by-field sesuai mapping di PACKET_MAPPING.md

**📌 CATATAN: Tugas ini membuat file HEADER baru (.h/.hpp/.fbs).**
Agent lain mungkin menunggu file ini selesai. Prioritaskan pengerjaan.

---

## Aturan Keamanan Eksekusi Paralel

1. BACA file Reborn yang tercantum sebelum mengubah

2. BACA file Old reference untuk memahami behavior asli

3. JANGAN ubah file di luar daftar ini

4. KODE BARU harus mengikuti style yang sudah ada (PascalCase class, snake_case function)

5. **🚫 JANGAN build atau compile**. Cukup tulis kode. Build akan dilakukan 1x di akhir oleh koordinator.

6. **🚫 JANGAN commit atau push**. Cukup tulis kode.

7. **⚠️ Jika tugas Anda membuat file .h BARU (header)**: beri tahu koordinator karena agent lain mungkin depend pada header Anda.

8. **✅ Jika tugas selesai**: cukup return pesan "Agent #{agent_num+1:03d} done: files modified".

