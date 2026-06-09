# Agent #013 — Tugas Adaptasi Luna Old → Reborn

Lokasi: /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn

Build: cmake --build build/macos-debug -j$(sysctl -n hw.ncpu)

---

## File yang harus diubah


### client/audio/AudioManager.cpp [UPDATE]

- **🟡 BGM crossfade implementation** (Severity: M, Effort: 2 days)
  - Referensi Old: `(basic crossfade)`

- **🔴 3D positional audio** (Severity: M, Effort: 3 days)
  - Referensi Old: `(Miles 3D)`

### server/map/systems/ItemSystem.cpp [UPDATE]

- **🟡 MapServer: item management** (Severity: H, Effort: 1 week)
  - Referensi Old: `ItemManager`

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

