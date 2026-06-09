# Agent FINAL-A — Verifikasi & Koreksi ADAPTASI_SPEC.md

Lokasi: /Users/macbookair/PRIBADI/luna-plus-master
Build: cmake --build Luna-Plus-Reborn/build/macos-debug -j$(sysctl -n hw.ncpu)

## Tugas

Baca ADAPTASI_SPEC.md, lalu verifikasi SETIAP klaim dengan membaca kode Reborn yang sesungguhnya. Update dokumen hanya jika klaim terbukti SALAH.

## Aturan Ketat (WAJIB)

1. **🚫 JANGAN PERCAYA dokumen**. Setiap klaim harus diverifikasi dengan membaca file asli.
2. **✅ Jika sudah benar**: skip. Jangan ubah apapun.
3. **🔧 Jika salah**: update dokumen dengan data yang benar. Sertakan bukti (file:line).
4. **🚫 JANGAN ubah kode**. Hanya update dokumen ADAPTASI_SPEC.md.
5. **🚫 JANGAN build**. Hanya baca file + update dokumen.

## Item yang harus diverifikasi

### Player Flow — Save ID Checkbox
- Baca client/ui/screens/LoginScreen.cpp — cari "Save ID" atau "save_id"
- Status sekarang: apakah sudah ada atau masih missing?

### UI System — Missing Dialogs
- 114 .bin files diklaim missing. Ambil sample 10 file:
  1. BattleGuage.bin → cari di client/ui/LegacyHudOverlay.cpp
  2. ShoutDlg.bin → cari di client/ui/dialogs/ atau ChatPanel
  3. QuickSlot.bin → cari di client/ui/LegacyHudOverlay.cpp
  4. Channel.bin → cari di client/ui/dialogs/ChannelDialog.cpp
  5. SystemMsg.bin → cari di client/ui/
  6. PartyCreate.bin → cari di client/ui/dialogs/PartyDialog.cpp
  7. ServerSelect.bin → cari di client/ui/screens/LoginScreen.cpp
  8. MonsterKill.bin → cari di client/ui/dialogs/MonsterKillDialog.cpp
  9. PetresDialog.bin → cari di client/ui/dialogs/PetDialog.cpp
  10. DissolveDialog.bin → cari di client/ui/dialogs/ atau GameScreen.cpp
- Untuk masing-masing: apakah ADA implementasi di Reborn?
- Update daftar di ADAPTASI_SPEC.md sesuai temuan.

### Gameplay — Aggro Penalty
- Baca game/ecs/systems/CombatSystem.cpp — cari "aggro" atau "ThreatTable"
- Apakah aggro penalty (aggro≥3: avoid ×= 1-(n-2)²×0.01) sudah diimplementasi?
- Baca game/ecs/systems/AISystem.cpp — cari aggro logic

### Network — Pseudocode Handler Pattern
- Baca server/map/MapServer.cpp — cari HandleNpcSpeech
- Apakah pseudocode di ADAPTASI_SPEC.md cocok dengan implementasi aktual?

### Server — FSM Engine
- Baca server/map/systems/ — cari file FSMEngine.* atau MapScriptRuntime.*
- Apakah FSMEngine sudah ada implementasi atau masih partial?

### Error Handling — NACK Codes
- Baca server/map/MapServer.cpp — cari pola "NACK" atau "nack"
- Berapa banyak NACK response yang sudah diimplementasi?

## Output

Update ADAPTASI_SPEC.md hanya item yang TERBUKTI SALAH. Untuk setiap perubahan:
```
[SEBELUM] <teks salah>
[SESUDAH] <teks benar>
[BUKTI]   <file>:<line> — <isi kode aktual>
```

## ✅ Kembalikan "Agent FINAL-A done: ADAPTASI_SPEC.md verified, X items corrected"
