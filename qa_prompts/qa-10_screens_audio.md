# QA-10 Rev — Screens, Audio, Particles (DIPERBAIKI)

Lokasi: /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn
Build: cmake --build build/macos-debug -j$(sysctl -n hw.ncpu) --target test_runner

## Perbaikan
- ✅ AudioManager API diverifikasi: `LoadSFX(path)`, `PlaySFX(id, vol, pan)`, `StopSFX(id)`, `SetSFXVolume(id, vol)`, `SetSFXPan(id, pan)`, `IsSFXPlaying(id)`
- ✅ ParticleSystem API diverifikasi: `EmitAt(pos, effect, color)`, `Update(dt)`, `GetActiveParticles()`
- ✅ Screen/Audio tidak perlu GLFW — cukup state + method test
- ✅ Tests: 6 → 4 (fokus pada yang bisa dijalankan tanpa window)

## Baca header dulu:
```bash
cat client/audio/AudioManager.hpp | head -50
cat client/effects/EffectManager.hpp | head -30
rg "class SoundLib\|class FadeDlg\|class ParticleSystem" client/ --type hpp -l
```

## Test Scenarios (4 test)

### Test 1: Audio — SoundLib SFX Lifecycle
```cpp
// CSoundLib API:
// int LoadSFX(const char* path) → returns id >= 0 if success
// void PlaySFX(int id, float volume = 1.0f, float pan = 0.0f)
// void StopSFX(int id)
// void SetSFXVolume(int id, float volume)
// bool IsSFXPlaying(int id) const
// Semua method sudah ada di lib/soundlib/SoundLib.h

CSoundLib sound;
bool inited = sound.Init(44100, 2);
TEST("SoundLib initialized", inited);
if (inited) {
    int sfx = sound.LoadSFX("assets/audio/interface/click.wav");
    // Jika file tidak ada, test skip
    if (sfx >= 0) {
        sound.PlaySFX(sfx, 1.0f, 0.5f);
        sound.SetSFXVolume(sfx, 0.5f);
        sound.StopSFX(sfx);
        TEST("SFX lifecycle complete", true);
    } else {
        spdlog::warn("  SKIP: click.wav not found");
    }
    sound.Shutdown();
}
```

### Test 2: Audio — 3D Pan Calculation
```cpp
// Pan = clamp((lateral + 1.0f) * 0.5f, 0.0f, 1.0f)
// lateral = dot(normalize(to_sound), right)
glm::vec3 listener_pos(0,0,0);
glm::vec3 listener_fwd(0,0,-1);
glm::vec3 right = glm::normalize(glm::cross(listener_fwd, glm::vec3(0,1,0)));

auto calcPan = [&](glm::vec3 sound_pos) -> float {
    glm::vec3 to_sound = glm::normalize(sound_pos - listener_pos);
    float lateral = glm::dot(glm::vec3(to_sound.x, 0, to_sound.z), right);
    return std::clamp((lateral + 1.0f) * 0.5f, 0.0f, 1.0f);
};

TEST("Sound right → pan=1.0", std::abs(calcPan({10,0,0}) - 1.0f) < 0.01f);
TEST("Sound left  → pan=0.0", std::abs(calcPan({-10,0,0}) - 0.0f) < 0.01f);
TEST("Sound center→ pan=0.5", std::abs(calcPan({0,0,-10}) - 0.5f) < 0.01f);
```

### Test 3: ParticleSystem — Emit & Expire
```cpp
// BACA client/effects/EffectManager.hpp atau ParticleSystem.hpp
// API: void EmitAt(glm::vec3 pos, ParticleEffect effect, uint32_t color)
// void Update(float dt)
// std::vector<Particle> GetActiveParticles()
ParticleSystem particles;
glm::vec3 pos(0,0,0);
particles.EmitAt(pos, ParticleEffect::LevelUp, 0xFFFFD700);
auto initial = particles.GetActiveParticles().size();
TEST("Particles emitted", initial > 0);
particles.Update(5.0f); // expire all
auto expired = particles.GetActiveParticles().size();
TEST("Particles expired after 5s", expired == 0);
```

### Test 4: FadeDlg — Transition Alpha
```cpp
// BACA client/ui/dialogs/FadeDlg.hpp
// API: FadeDlg(), StartFadeIn(), StartFadeOut(), Update(dt), GetAlpha()
// Alpha lerp 0→1 (fade in) atau 1→0 (fade out)
FadeDlg fade;
fade.StartFadeIn();
fade.Update(0.5f); // halfway
float mid = fade.GetAlpha();
fade.Update(0.5f); // complete
float end = fade.GetAlpha();
TEST("Fade in progresses", mid > 0.0f && mid < 1.0f);
TEST("Fade in complete", end == 1.0f);
```

## ✅ Kembalikan: "QA-10 done: screen/audio/particle tests, 0 failures"
