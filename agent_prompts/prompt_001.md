# Agent #001 — Tugas Adaptasi Luna Old → Reborn

Lokasi: /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn

Build: cmake --build build/macos-debug -j$(sysctl -n hw.ncpu)

---

## File yang harus diubah


### game/ecs/systems/CombatSystem.cpp [UPDATE]

- **🟡 Combat input system** (Severity: H, Effort: 5 days)
  - Referensi Old: `GameIn.cpp`

- **🟡 CalcDamage formula refactor** (Severity: C, Effort: 5 days)
  - Referensi Old: `CHero::CalcDamage()`
  - Baca file Old `CHero::CalcDamage()` di Luna-Plus-Old, update formula constants di Reborn

- **🟡 Crit chance formula (Old: DEX/1000)** (Severity: M, Effort: 4 hrs)
  - Referensi Old: `CHero::CalcDamage()`

- **🟡 Block chance formula (Old: CON/2000)** (Severity: M, Effort: 4 hrs)
  - Referensi Old: `CHero::CalcDamage()`

- **🟡 Miss chance formula (Old: 1% base)** (Severity: L, Effort: 2 hrs)
  - Referensi Old: `CHero::CalcDamage()`

- **🟡 Skill damage modifier (add_type)** (Severity: H, Effort: 2 days)
  - Referensi Old: `CHero::CalcDamage()`

- **🔴 Hate/threat system** (Severity: H, Effort: 5 days)
  - Referensi Old: `Hero.cpp`

### game/network/protocol/Movement.fbs [UPDATE]

- **🟡 MP_MOVE: add KyungGongIdx field** (Severity: H, Effort: 1 week)
  - Referensi Old: `MP_MOVE (20+ packets)`
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

