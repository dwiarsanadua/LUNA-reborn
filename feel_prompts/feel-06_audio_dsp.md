# FEEL-06 — Audio DSP & SFX Feel Alignment

Lokasi: /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn
Build: cmake --build build/macos-debug -j$(sysctl -n hw.ncpu)

## Tugas

Old pakai Miles Sound System yang punya DSP effects (reverb, EQ, 3D positioning). Reborn pakai miniaudio yang raw. SFX tidak "terasa" sama.

## Aturan Ketat

1. BACA Old: cari Miles Sound System config (mss.h, MilesSoundLib.cpp)
2. BACA Reborn: `lib/soundlib/SoundLib.cpp` — catat method
3. ✅ Jika sudah setara — skip
4. 🔧 Jika beda — tambah DSP processing

## Yang harus dicek

### Old Miles Sound System:
```bash
rg "A3D\|DSP\|Reverb\|EQ\|Streaming\|VolumeRamp" /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Old/lib/soundlib/ --type cpp --type h | head -20
```

### Reborn miniaudio:
```bash
cat lib/soundlib/SoundLib.cpp
cat lib/soundlib/SoundLib.h
```

### Parameter yang harus diverifikasi:

| Parameter | Old (Miles) | Reborn (miniaudio) | Status |
|-----------|-------------|-------------------|--------|
| SFX volume ramp | Gradual fade | Instant | 🔧 |
| BGM crossfade duration | 500ms | ? | 🔧 |
| 3D distance model | Linear (1.0 falloff) | ? | 🔧 |
| Sample rate | 44100 Hz | 44100 Hz | ✅ |
| Compression | ADPCM? | Raw? | 🔧 |

### Implementasi (jika perlu):

```cpp
// Tambah volume ramp di CSoundLib::PlaySFX():
void CSoundLib::PlaySFX(int id, float volume, float pan) {
    // Old punya volume ramp 50ms
    // Reborn: instant — perlu gradual
    float target_volume = volume * master_volume_;
    float current = 0.0f;
    float ramp_ms = 50.0f;
    // ... implementasi volume ramp
    ma_sound_set_volume(slot.sound, target_volume);
}

// Tambah BGM crossfade:
void CSoundLib::CrossfadeBGM(int from_id, int to_id, float duration_ms) {
    // Fade out old, fade in new simultaneously
}
```

## Output

✅ Kembalikan: "FEEL-06 done: audio DSP aligned — X effects ported"
