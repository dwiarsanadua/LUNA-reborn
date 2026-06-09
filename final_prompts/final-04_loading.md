# FINAL-04 — Loading Flow: Progress Bar + Fade Transition

Lokasi: /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn
Build: cmake --build build/macos-debug -j$(sysctl -n hw.ncpu)

## Tugas

Old punya loading flow: launcher → download patch → NewLoadDlg.bin (loading screen dengan progress bar, random tips, background image) → fade in ke game. Reborn punya LoadingScreen basic. Perbaiki agar flow terasa sama.

## Aturan Ketat

1. BACA `client/ui/screens/LoadingScreen.cpp` — pahami flow
2. BACA `client/ui/dialogs/FadeDlg.cpp` — pahami fade transition
3. ✅ Jika sudah mirip — skip
4. 🔧 Jika belum — update

## File yang harus dicek

```bash
cat client/ui/screens/LoadingScreen.* | head -60
cat client/ui/dialogs/FadeDlg.* | head -40
```

## Yang harus diupdate

### 1. Loading Progress Bar
Old: progress bar di NewLoadDlg.bin dengan persentase.
```cpp
// Di LoadingScreen.hpp atau .cpp:
// Cek apakah sudah ada ProgressBar widget
// Jika sudah → pastikan progress update dari asset loading
// Jika belum → tambah ProgressBar di tengah screen
```

### 2. Random Loading Tips
Old: LoadingMsg.bin berisi array tips, ditampilkan random.
```cpp
// Cek apakah ada file assets/interface/LoadingMsg.bin.txt
// Jika ada → parse, tampilkan random tip saat loading
```

### 3. Background Image
Old: background image khas (scene, artwork).
```cpp
// Cek apakah sudah ada background texture loading
```

### 4. Fade Transition
Old: fade in setelah loading selesai.
```cpp
// Di GameScreen::Enter() atau LoadingScreen::OnComplete():
// Panggil FadeDlg::StartFadeIn() → tunggu selesai → mulai game
```

## Output

✅ Kembalikan: "FINAL-04 done: loading flow aligned — progress bar, tips, fade"
