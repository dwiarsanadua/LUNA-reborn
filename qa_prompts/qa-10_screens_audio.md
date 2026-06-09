# QA-10 — Visual & Render Test: Screen Flow, Transition, Audio Pipeline

Lokasi: /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn
Build: cmake --build build/macos-debug -j$(sysctl -n hw.ncpu) --target test_runner

## Tugas

Buat `tools/test_runner/screens_audio.cpp` — test screen flow, transition, dan audio pipeline.

## Aturan Ketat

1. ✅ Panggil method yang SUDAH ADA
2. JANGAN buka GLFW window — cukup state transition test
3. Build + run — 0 failure

## Test Scenarios

### Test 1: Screen Stack / State Machine
```
TEST_STEP("Screens: Launcher → Login → CharSelect → Game flow");
```
- Baca ScreenManager atau GameState
- Set state = Launcher → current = Launcher
- Transition Login → current = Login
- Transition CharSelect → current = CharSelect
- Transition Game → current = Game
- Test: state machine berjalan sesuai urutan
- Test: invalid transition di-reject (jika ada guard)

### Test 2: Fade Transition
```
TEST_STEP("Transition: FadeDlg — alpha lerp 0→1→0");
```
- Buat FadeDlg
- Mulai fade in → alpha dari 0 ke 1
- Update 0.5 dt → alpha antara 0 dan 1
- Update sampai selesai → alpha = 1
- Fade out → alpha kembali ke 0

### Test 3: AudioManager — Load & Play SFX
```
TEST_STEP("Audio: SFX load, play, volume, stop");
```
- Baca client/audio/AudioManager.hpp
- LoadSFX(path) → id valid (≥ 0)
- PlaySFX(id) → playing = true
- SetSFXVolume(id, 0.5f) → volume berubah
- StopSFX(id) → playing = false
- (Gunakan file audio yang ada di assets/audio/ jika ada)
- Jika tidak ada file audio: test dengan miniaudio langsung

### Test 4: Audio — 3D Positional
```
TEST_STEP("Audio: 3D position → pan calculation");
```
- Set listener position (0,0,0), forward (0,0,-1)
- Set sound position (10,0,0) → pan should be right
- Set sound position (-10,0,0) → pan should be left
- Set sound position (0,0,-10) → pan center
- Test: pan value in correct range [0,1]

### Test 5: ParticleSystem — Emit & Update
```
TEST_STEP("Particles: emit, update lifetime, expire");
```
- Buat ParticleSystem
- EmitAt(position, effect_type, color)
- GetActiveParticles() > 0
- Update(5.0f) → semua expired
- GetActiveParticles() = 0

### Test 6: Minimap Rendering Data
```
TEST_STEP("Minimap: load tile data → valid grid");
```
- Cek MiniMapDlg — cari method loadMap / setMap
- Load data minimap untuk map tertentu
- Test: grid width/height > 0
- Test: tile data array tidak kosong

## Output

✅ Kembalikan: "QA-10 done: screen/audio/particle tests — X passed"
