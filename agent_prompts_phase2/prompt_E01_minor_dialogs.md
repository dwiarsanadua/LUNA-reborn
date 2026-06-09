# Agent E01 — Minor Dialogs (Phase 2)

Lokasi: /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn
Build: cmake --build build/macos-debug -j$(sysctl -n hw.ncpu)

## File yang harus dibuat

### client/ui/dialogs/ChannelDialog.cpp [BARU] — 🟡 Channel selection
Old .bin: Channel.bin — Pilih channel server. Grid + list channel.

### client/ui/dialogs/CharDelDialog.cpp [BARU] — 🟡 Character delete confirm
Old .bin: CharDel.bin — Konfirmasi hapus character. Password input + confirm button.
Catatan: Basic delete logic sudah ada di CharSelectScreen, tinggal UI dialog.

### client/ui/dialogs/PartyMatchingDialog.cpp [BARU] — 🟡 Party matching
Old .bin: PartyMatchingDlg.bin, PartyMatchingSetting.bin, PartySeekMemberDlg.bin
Deskripsi: Party matching/finding system.

### client/ui/dialogs/FriendInviteDialog.cpp [BARU] — 🟡 Friend invite
Old .bin: FriendInvite.bin — Dialog invite friend.

### client/ui/dialogs/GuildCreateDialog.cpp [BARU] — 🟡 Guild create form
Old .bin: GuildCreate.bin — Form untuk membuat guild (name, mark, description).

### client/ui/dialogs/GuildInviteDialog.cpp [BARU] — 🟡 Guild invite
Old .bin: GuildInvite.bin — Invite player ke guild.

### client/ui/dialogs/QuestQuickViewDialog.cpp [BARU] — 🟡 Quest quick view
Old .bin: QuestQuickView.bin — Quick quest tracker HUD.

### client/ui/dialogs/MonsterKillDialog.cpp [BARU] — 🟡 Monster kill tracker
Old .bin: MonsterKill.bin — Menampilkan kill count untuk quest.

## Aturan

1. IKUTI pattern dialog yang sudah ada (lihat FishingDialog.cpp atau WeatherDialog.cpp)
2. Setiap dialog: .hpp + .cpp, minimal constructor + Open() + Render/Update
3. Load dari .bin.txt jika ada, fallback ke C++ layout
4. JANGAN build atau compile
5. ✅ Kembalikan "Agent E01 done: 8 minor dialogs created"
