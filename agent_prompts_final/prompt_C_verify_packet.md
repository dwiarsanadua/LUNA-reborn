# Agent FINAL-C — Verifikasi & Koreksi PACKET_MAPPING.md

Lokasi: /Users/macbookair/PRIBADI/luna-plus-master
Build: cmake --build Luna-Plus-Reborn/build/macos-debug -j$(sysctl -n hw.ncpu)

## Tugas

Baca PACKET_MAPPING.md, lalu verifikasi SETIAP field mapping dengan membaca actual FlatBuffers schema (.fbs) dan Old Protocol.h.

## Aturan Ketat (WAJIB)

1. **🚫 JANGAN PERCAYA tabel**. Setiap field harus diverifikasi.
2. **✅ Jika field mapping sudah benar**: skip.
3. **🔧 Jika field mapping salah**: koreksi di PACKET_MAPPING.md.
4. **🚫 JANGAN ubah schema .fbs atau kode apapun**. Hanya update dokumen.
5. **🚫 JANGAN build**.

## Item yang harus diverifikasi

### 1. LOGIN → Login.fbs
- Baca game/network/protocol/Login.fbs — catat semua field
- Baca build/macos-debug/game/fbs/Login_generated.h — cari CreateLoginRequest signature
- Bandingkan dengan tabel di PACKET_MAPPING.md:
  - Apakah field order sesuai?
  - Apakah tipe data sesuai?
  - Apakah ada field Old yang sebenarnya sudah di-port tapi tidak tercatat?
  - Apakah ada field yang diklaim missing tapi sebenarnya ada?

### 2. MOVE → Movement.fbs
- Baca game/network/protocol/Movement.fbs
- Baca build/macos-debug/game/fbs/Movement_generated.h
- Verifikasi: kyung_gong_idx, added_move_speed, direction, target_position, move_mode

### 3. COMBAT → Combat.fbs
- Baca game/network/protocol/Combat.fbs
- Baca Combat_generated.h
- Verifikasi: target_id, skill_id, position, is_blocked

### 4. SKILL → Skill.fbs
- Baca game/network/protocol/Skill.fbs
- Verifikasi field mapping

### 5. INVENTORY → Inventory.fbs
- Baca game/network/protocol/Inventory.fbs
- Verifikasi: src_slot, dst_slot, src_storage_type, dst_storage_type, durability

### 6. QUEST → Quest.fbs
- Baca game/network/protocol/Quest.fbs

### 7. PARTY → Party.fbs
- Baca game/network/protocol/Party.fbs

### 8. GUILD → Guild.fbs
- Baca game/network/protocol/Guild.fbs
- Verifikasi: guild_mark, guild_point

### 9. CHAT → Chat.fbs
- Baca game/network/protocol/Chat.fbs
- Verifikasi: WhisperMessage, PartyChatMessage, GuildChatMessage, FamilyChatMessage, ShoutMessage

### 10. NPC → NPC.fbs
- Baca game/network/protocol/NPC.fbs
- Verifikasi: NpcAction enum, NpcRequest, NpcResponse

## Output

Update PACKET_MAPPING.md hanya untuk field yang TERBUKTI SALAH mapping-nya.
Untuk setiap koreksi:
```
[FIELD] <nama field>
[OLD]   <klaim salah>
[NEW]   <data benar>
[BUKTI] <file>:<line>
```

## ✅ Kembalikan "Agent FINAL-C done: PACKET_MAPPING.md verified, X corrections"
