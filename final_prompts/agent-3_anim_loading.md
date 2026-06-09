# AGENT-3 — Animation Blending + Loading Flow (merger FINAL-03 + FINAL-04)

Lokasi: /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn
Build: cmake --build build/macos-debug -j$(sysctl -n hw.ncpu)

## Tugas

1. Fine-tune animation transition times agar gerakan karakter tidak kaku
2. Upgrade loading screen dengan progress bar + loading tips + fade transition

## Aturan Ketat

1. ✅ Jika animasi transition sudah sesuai → skip
2. ✅ Jika loading screen sudah ada progress bar + tips → skip
3. 🔧 Jika belum → implementasi
4. Build verify

## File target (tidak ada conflict dengan agent lain)

- `engine/gx_geom/AnimationSystem.hpp` + `.cpp` — transition times
- `client/ui/screens/LoadingScreen.hpp` + `.cpp` — progress bar + tips
- `client/ui/dialogs/FadeDlg.hpp` + `.cpp` — fade transition

### Part A: Animation Transition Times (AnimationSystem.cpp)

Cari method `PlayAnimation()` atau `Update()`. Jika ada konstanta blend default (misal `blend_duration_ = 0.2f`), ganti dengan table-based:

```cpp
// Tambah struct atau map untuk transition time per-animasi
struct AnimTransitionConfig {
    float idle_to_walk = 0.10f;    // 100ms
    float walk_to_run = 0.20f;      // 200ms
    float to_attack = 0.05f;        // 50ms (instant)
    float attack_to_idle = 0.15f;   // 150ms
    float run_to_idle = 0.10f;      // 100ms
    float any_to_die = 0.30f;       // 300ms
    float default_blend = 0.20f;    // 200ms
};
// Set blend duration berdasarkan pair animasi (from → to)
```

### Part B: Loading Screen (LoadingScreen.hpp + .cpp)

```cpp
// Di LoadingScreen.hpp — tambah member:
class LoadingScreen {
    // ... existing ...
private:
    ProgressBar* progress_bar_ = nullptr;
    Label* tip_label_ = nullptr;
    std::vector<std::string> tips_;
    float load_timer_ = 0.0f;
    int current_tip_ = 0;
    
    void LoadTips();  // parse dari assets/interface/LoadingMsg.bin.txt
    void ShowRandomTip();
};
```

```cpp
// Di LoadingScreen::Enter():
if (!progress_bar_) {
    progress_bar_ = new ProgressBar(w/2 - 100, h - 60, 200, 16);
    progress_bar_->SetColors({60, 200, 60, 255}, {60, 0, 0, 180});
}
LoadTips();
ShowRandomTip();
```

```cpp
// Di LoadingScreen::Update():
load_timer_ += dt;
if (load_timer_ > 3.0f) {
    current_tip_ = (current_tip_ + 1) % tips_.size();
    ShowRandomTip();
    load_timer_ = 0.0f;
}
```

### Part C: Fade Transition (FadeDlg.cpp)

```cpp
// Cek apakah FadeDlg sudah punya StartFadeIn() / StartFadeOut()
// Jika belum — implement:
void FadeDlg::StartFadeIn() {
    alpha_ = 0.0f; fading_in_ = true; fading_out_ = false;
}
void FadeDlg::StartFadeOut() {
    alpha_ = 1.0f; fading_out_ = true; fading_in_ = false;
}
void FadeDlg::Update(float dt) {
    float speed = 1.0f / 0.5f; // 500ms fade
    if (fading_in_) { alpha_ += dt * speed; if (alpha_ >= 1.0f) { alpha_ = 1.0f; fading_in_ = false; } }
    if (fading_out_) { alpha_ -= dt * speed; if (alpha_ <= 0.0f) { alpha_ = 0.0f; fading_out_ = false; } }
}
```

Di `LoadingScreen::OnComplete()` atau `GameScreen::Enter()`:
```cpp
FadeDlg::StartFadeIn();  // fade in setelah loading selesai
```

## ✅ Kembalikan: "AGENT-3 done: animation transitions + loading screen + fade, 0 build errors"
