# FEEL-05 — Animation Timing Verification

Lokasi: /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn
Build: cmake --build build/macos-debug -j$(sysctl -n hw.ncpu)

## Tugas

Old pakai .anm format proprietary. Reborn convert ke .anm.json. Frame rate, blending, transition timing harus match agar animasi karakter terlihat sama.

## Aturan Ketat

1. BACA Old reference: cari format .anm di scripts_legacy/anm_to_json.py
2. BACA Reborn: `game/ecs/systems/AnimationSystem.cpp`
3. ✅ Jika animasi timing sudah match — skip
4. 🔧 Jika beda — update AnimationSystem

## Yang harus dicek

### Old Animation Format (.anm):
```bash
# Cari parser atau converter
cat /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Old/scripts_legacy/anm_to_json.py | head -50
```

### Reborn Animation System:
```bash
cat game/ecs/systems/AnimationSystem.* | head -100
```

### Parameter yang harus diverifikasi:

| Parameter | Old | Reborn | Status |
|-----------|-----|--------|--------|
| Frame rate | .anm header fps | .anm.json "fps" field | ✅/🔧 |
| Blending weight | Linear interp | ? | 🔧 Cek |
| Transition time | Crossfade N ms | ? | 🔧 Cek |
| Loop mode | Loop flag | .anm.json "loop" | ✅/🔧 |
| Speed modifier | AttackSpeedRate | ? | 🔧 Cek |

### Test:

```cpp
// Load Old .anm.json, cek field:
// - "fps" → harus match dengan render rate
// - "frames"[] → jumlah frame
// - "loop" → boolean
// - "duration" → total detik

// Bandingkan dengan playback di AnimationSystem:
// Apakah AnimationSystem::Update() pakai duration dari JSON?
// Apakah blending weight benar?
```

## Output

✅ Kembalikan: "FEEL-05 done: animation timing verified — X parameters aligned"
