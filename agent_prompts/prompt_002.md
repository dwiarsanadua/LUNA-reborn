# Agent #002 — Tugas Adaptasi Luna Old → Reborn

Lokasi: /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn

Build: cmake --build build/macos-debug -j$(sysctl -n hw.ncpu)

---

## File yang harus diubah


### server/agent/AgentServer.cpp [UPDATE]

- **🟡 AgentServer: char list handler** (Severity: H, Effort: 1 week)
  - Referensi Old: `AgentDBMsgParser`

- **🟡 Login failure NACK codes** (Severity: M, Effort: 1 day)
  - Referensi Old: `Detailed NACK codes`

- **🟡 Authentication: password hashing (bcrypt)** (Severity: C, Effort: 3 days)
  - Referensi Old: `AES-GCM token`

- **🔴 Rate limiting** (Severity: H, Effort: 1 week)
  - Referensi Old: `(basic)`

- **🔴 Brute-force protection** (Severity: M, Effort: 2 days)
  - Referensi Old: `(basic lockout)`

### game/network/protocol/Combat.fbs [UPDATE]

- **🟡 MP_COMBAT: add isBlocked field** (Severity: H, Effort: 1 week)
  - Referensi Old: `MP_COMBAT (10+ packets)`
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

