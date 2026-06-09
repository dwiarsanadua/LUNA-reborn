# Agent-08 — Minor Dialogs: Batch Implementasi 15 Sub-Dialog

Lokasi: /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn
Build: cmake --build build/macos-debug -j$(sysctl -n hw.ncpu)

## Tugas

Dari 106 .bin files yang diklaim missing, ~70 adalah sub-dialogs minor. Batch implementasi 15 yang paling sering digunakan:

1. ApplyOptionDialog.bin — Option apply confirmation
2. AutoAnswerDlg.bin — Auto-reply system
3. ChallengeZoneClearNo1Dlg.bin — Challenge clear popup
4. ChatRoomCreateDlg.bin — Create chat room
5. ChatRoomJoinDlg.bin — Join chat room
6. ChatRoomOptionDlg.bin — Chat room options
7. FarmAnimalCageDlg.bin — Farm animal cage management
8. GuildLevelUp.bin — Guild level up notification
9. GuildRank.bin — Guild rank display
10. HouseNameDlg.bin — House naming dialog
11. ItemPopupDlg.bin — Item detail popup
12. PartyMatchingSetting.bin — Party matching settings
13. PartySeekMemberDlg.bin — Seek party member
14. ReinforceGuideDialog.bin — Reinforce guide
15. TutorialBtnDlg.bin — Tutorial button overlay

## Aturan Ketat

1. CEK setiap dialog dengan `ls client/ui/dialogs/<Nama>* 2>/dev/null`
2. CEK di GameScreen.cpp: `rg "<Nama>" client/ui/screens/GameScreen.cpp`
3. ✅ Jika SUDAH ADA — skip
4. 🔧 Jika BELUM ADA — buat dialog minimal dengan pattern yang SAMA

## Pattern Dialog Minimal

```cpp
// NamaDialog.hpp
#pragma once
class GameState;
class WindowManager;
class Window;
class NamaDialog {
public:
    void Open(WindowManager* wm);
    void Close();
private:
    Window* window_ = nullptr;
};
```

Implementasi .cpp:
- Buka window dengan `wm->CreateWindow()` atau `new Window()`
- Tambah widget sederhana (Label, Button)
- JANGAN buat layout kompleks — cukup functional

## Output

✅ Kembalikan: "Agent-08 done: created [list], skipped [list yang sudah ada]"
