# FINAL-03 — Animation Blending: Transition & Crossfade Fine-Tune

Lokasi: /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn
Build: cmake --build build/macos-debug -j$(sysctl -n hw.ncpu)

## Tugas

Old punya sistem transisi antar animasi yang specifik (idle→walk, walk→run, run→attack, dll.) dengan crossfade time berbeda-beda. Reborn punya AnimationSystem dengan basic blend. Perlu fine-tune agar transisi terasa natural.

## Aturan Ketat

1. BACA `engine/gx_geom/AnimationSystem.cpp` — catat parameter blend
2. ✅ Jika sudah sesuai — skip
3. 🔧 Jika belum — update

## File yang harus dicek

```bash
cat engine/gx_geom/AnimationSystem.* | head -100
rg "blend\|crossfade\|transition\|Blend\|lerp\|slerp" engine/gx_geom/AnimationSystem.* --type cpp
```

## Transition Time Table (dari Old — estimasi)

| Transition | Old (ms) | Reborn (ms) | Action |
|-----------|---------|-------------|--------|
| Idle → Walk | 100 | ? | 🔧 Set ke 100 |
| Walk → Run | 200 | ? | 🔧 Set ke 200 |
| Walk/Run → Attack | 50 | ? | 🔧 Set ke 50 (instant) |
| Attack → Idle | 150 | ? | 🔧 Set ke 150 |
| Run → Idle (stop) | 100 | ? | 🔧 Set ke 100 |
| Fall → Idle | 200 | ? | 🔧 Set ke 200 |
| Any → Die | 300 | ? | 🔧 Set ke 300 |

### Implementasi

```cpp
// Cari struct atau map untuk transition times.
// Jika belum ada, tambah:
struct AnimTransition {
    std::string from_anim;
    std::string to_anim;
    float blend_duration; // detik
};

// Atau lebih sederhana — update konstanta blend_duration_
// Di AnimationSystem::PlayAnimation():
// - Jika dari idle → walk: blend = 0.1f
// - Jika ke attack: blend = 0.05f
// - Default: blend = 0.2f
```

## Output

✅ Kembalikan: "FINAL-03 done: animation transitions tuned — X transitions aligned"
