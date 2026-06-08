# Solution Plan: Closing the Visual & Technical Gaps

> **Berdasarkan analisis:** 6 gap utama antara Luna-Plus-Old dan Luna-Plus-Reborn.
> **Target:** Implementasi modern menggunakan C++23 + bgfx + ECS — bukan porting kode Old.

---

## Gap 1: Asset Pipeline — Metadata Hilang

### Masalah
Converter `.mod → .glb` membuang bones, animation weights, material properties (specular, ambient, emissive, transparency mode).

### Solusi — Legacy C++ Parser Upgrade

**Tools/legacy_converters/mod2obj.cpp — tambahkan ekstraksi:**

```cpp
// Material properties yang hilang (dari OT_MATERIAL block di .mod)
struct ModMaterial {
    std::string name;
    std::string diffuse_tex, reflect_tex, bump_tex;
    uint32_t diffuse_color, ambient_color, specular_color;
    float transparency;      // → bgfx alpha blend
    float shine;             // → specular power
    float shine_strength;    // → specular intensity
    uint32_t flag;           // → 2-sided, additive, dll
};
```

**Output:** `.glb` dengan material PBR (metallic-roughness) atau specular-glossiness:
```
glTF Material:
  pbrMetallicRoughness:
    baseColorFactor: diffuse_color
    metallicFactor: 0.0
    roughnessFactor: 1.0 - shine
  emissiveFactor: 0.0
  alphaMode: transparency > 0 ? "BLEND" : "OPAQUE"
  doubleSided: flag & 0x100
```

**Untuk terrain (.hfl parser):**
```cpp
// Header HFL — jangan di-skip
struct HflHeader {
    float version;           // 0x0001
    uint32_t face_size;      // fFaceSize — scale penting
    uint32_t obj_num_x;      // grid X
    uint32_t obj_num_z;      // grid Z
    float left, top;         // world position offset
};
```

---

## Gap 2: Atmosphere & Environment — Fog, Ambient, Skybox

### Masalah
Fog dan ambient light dari JSON map diabaikan. Skybox statis. Air effect sederhana.

### Solusi — Parse Environment Data + Dynamic Sky

**EngineMap.cpp — Parse FOG_DESC dari scene JSON:**

```cpp
// Tambah parsing di LoadSceneObjects():
struct FogDesc {
    float start = 30.0f, end = 150.0f, density = 0.8f;
    uint32_t color = 0x888899FF;
    bool enabled = true;
};

// Parse dari scene JSON:
// "fog": {"start": 50, "end": 300, "density": 0.5, "color": "0xAABBCCFF"}
```

**EngineSky.cpp — Skybox dinamis:**
```cpp
// Implementasi multi-layer cloud:
// Layer 1: Sky dome texture (existing) ✅
// Layer 2: Cloud layer — rotating transparent cloud texture
// Layer 3: Fog layer — distance-based

void EngineSky::Update(float dt) {
    time_of_day_ += dt * time_speed_;
    cloud_offset_ += dt * 0.01f;  // slow cloud drift
    
    // Update sky colors based on time
    sky_top_color_ = CalculateSkyColor(time_of_day_);
    sun_color_ = CalculateSunColor(time_of_day_);
}
```

**Air Effect — Simple Water:**
```cpp
// Di terrain atau sebagai entity terpisah
class WaterRenderer {
    // Plane dengan UV animation untuk caustics
    // Reflection via render-to-texture (bgfx FBO)
    // Transparency with depth-based color
};
```

---

## Gap 3: EFT Effect System — Missing Pipeline

### Masalah
4.138 file `.beff` tidak terkonversi. EftParser hanya basic.

### Solusi — Binary Parser + Implementation

**Tool baru: tools/asset_pipeline/convert_effects.py**

```python
# Parse binary .beff format:
# Header: version, unit_count, total_duration
# Per unit: type (0=Particle,1=Billboard,2=Light,3=Sound,4=CameraShake,5=Model)
#   Particle: emit_rate, life, velocity, gravity, color_over_life, size_over_life
#   Billboard: texture, size, billboard_type (screen/camera/axis)
#   Light: color, radius, intensity, type (point/spot)
#   CameraShake: amplitude, frequency, decay
#   Model: attach bone, scale
```

**Output:** JSON format per effect → `assets/effects/*.json`

**C++ Loader:** `EffectManager.cpp` — parse JSON + spawn ECS particles:
```cpp
void EffectManager::LoadEffect(const std::string& json_path) {
    // Parse units
    for (auto& unit : parsed.units) {
        switch (unit.type) {
            case UNIT_PARTICLE: SpawnParticleEmitter(unit); break;
            case UNIT_BILLBOARD: SpawnBillboard(unit); break;
            case UNIT_LIGHT: SpawnDynamicLight(unit); break;
            case UNIT_CAMERA_SHAKE: ApplyCameraShake(unit); break;
            case UNIT_MODEL: AttachModelEffect(unit); break;
        }
    }
}
```

---

## Gap 4: UI Polish — Color Palette + Scripting

### Masalah
Warna hardcoded, UI layout kaku (C++), tidak ada animasi.

### Solusi — Color Table + Hybrid UI

**Color Palette (dari Old cWindowDef.h):**

```cpp
// client/ui/ColorPalette.hpp
namespace ColorPalette {
    // General
    constexpr uint32_t WINDOW_BG      = 0xff0a0a1e;
    constexpr uint32_t WINDOW_TITLE   = 0xff2244aa;
    constexpr uint32_t WINDOW_BORDER  = 0xff6666aa;
    
    // Item rarity
    constexpr uint32_t NORMAL_ITEM    = 0xffffffff;
    constexpr uint32_t MAGIC_ITEM     = 0xff4444ff;
    constexpr uint32_t RARE_ITEM      = 0xffffff00;
    constexpr uint32_t UNIQUE_ITEM    = 0xffff8800;
    constexpr uint32_t LEGENDARY_ITEM = 0xffff4444;
    constexpr uint32_t SET_ITEM       = 0xff00ff00;
    constexpr uint32_t QUEST_ITEM     = 0xff88ff88;
    
    // UI States
    constexpr uint32_t BTN_NORMAL     = 0xffcccccc;
    constexpr uint32_t BTN_HOVER      = 0xffffffff;
    constexpr uint32_t BTN_DISABLED   = 0xff666666;
    constexpr uint32_t TEXT_NORMAL    = 0xffffffff;
    constexpr uint32_t TEXT_DISABLED  = 0xff888888;
    constexpr uint32_t TEXT_HIGHLIGHT = 0xffffff00;
};
```

**UI Script Loading:** Gunakan file `.bin.txt` yang sudah di-decrypt untuk layout:
```cpp
// WindowManager::LoadFromScript() — sudah ada, tinggal aktifkan
// Setiap dialog akan load layout dari .bin.txt
// C++ code hanya untuk logic (event handlers)
```

**UI Animation:** Fade in/out, slide, scale — via `FadeDlg` yang sudah ada.

---

## Gap 5: Movement — Waypoint Buffer

### Masalah
Reborn menggunakan sync point-to-point sederhana. Old punya buffer waypoint.

### Solusi — Waypoint Buffer di ServerAuthMovement

```cpp
// client/engine/ServerAuthMovement.hpp — tambah:
struct Waypoint {
    glm::vec3 position;
    float timestamp;
};

class ServerAuthMovement {
    static constexpr size_t MAX_WAYPOINTS = 16;
    std::deque<Waypoint> waypoints_;
    
    void PushWaypoint(const glm::vec3& pos) {
        waypoints_.push_back({pos, current_time_});
        if (waypoints_.size() > MAX_WAYPOINTS)
            waypoints_.pop_front();
    }
    
    glm::vec3 GetInterpolatedPosition(float time) const {
        if (waypoints_.empty()) return position_;
        if (waypoints_.size() == 1) return waypoints_[0].position;
        // Linear interpolation between nearest waypoints
        for (size_t i = 0; i < waypoints_.size() - 1; i++) {
            if (time >= waypoints_[i].timestamp && time <= waypoints_[i+1].timestamp) {
                float t = (time - waypoints_[i].timestamp) / 
                         (waypoints_[i+1].timestamp - waypoints_[i].timestamp);
                return glm::mix(waypoints_[i].position, waypoints_[i+1].position, t);
            }
        }
        return waypoints_.back().position;
    }
};
```

**Dash/Leap Movement (KyungGongIdx):**
```cpp
// Tambah state di Hero:
struct DashState {
    bool is_dashing = false;
    float dash_time = 0.0f;
    float dash_duration = 0.3f;
    glm::vec3 dash_direction;
    float dash_speed = 30.0f;
    
    void StartDash(const glm::vec3& dir) {
        is_dashing = true;
        dash_time = 0;
        dash_direction = dir;
    }
    
    void Update(float dt) {
        if (!is_dashing) return;
        dash_time += dt;
        if (dash_time >= dash_duration) {
            is_dashing = false;
            return;
        }
        // Apply movement + visual effect (speed lines, blur)
    }
};
```

---

## Status Implementasi

| # | Gap | Status | Commit |
|---|-----|--------|--------|
| 1 | **HGT limit 4096→65536** | ✅ **SELESAI** | `9cc539a6` |
| 2 | **Fog values** (80-600 range) | ✅ **SELESAI** | `cbbfd725` |
| 3 | **Light direction** (warmer) | ✅ **SELESAI** | `cbbfd725` |
| 4 | **Color Palette** (35 constants) | ✅ **SELESAI** | `cbbfd725` |
| 5 | **Terrain world_scale** (0.1→0.16) | ✅ **SELESAI** | `23beac89` |
| 6 | **Login screen textures** (direct load) | ✅ **SELESAI** | `f0a517ce` |
| 7 | **Interface files decrypt** (213/213) | ✅ **SELESAI** | (local) |
| 8 | **BGM filename mapping** (48 tracks) | ✅ **SELESAI** | `88826def` |
| 9 | **Texture search paths** (9 paths) | ✅ **SELESAI** | `cbce2e1f` |
| 10 | **Engine path standardization** | ✅ **SELESAI** | `28c233a4` |
| 11 | **.chr handler in EngineMap** | ✅ **SELESAI** | `87fe5b96` |
| — | | | |
| ⬜ | **HFL header parsing** (face_size, world pos) | 🔶 Partial — scale fixed | — |
| ⬜ | **Model material pipeline** (.mod → PBR glTF) | ❌ Belum | — |
| ⬜ | **EFT effect parser** (4.138 .beff) | ❌ Belum | — |
| ⬜ | **Movement waypoint buffer** | ❌ Belum | — |
| ⬜ | **Water effect** (caustics, reflection) | ❌ Belum | — |
| ⬜ | **Dynamic skybox clouds** | ❌ Belum | — |
| ⬜ | **UI script animation** (fade/slide) | ❌ Belum | — |

### Fix 1: HFL Header Parsing
```cpp
// tools/legacy_converters/hfl2hgt.cpp — tambah:
// struct HflHeader {
//     float fFaceSize;    // scale: 10.0 (default)
//     int32_t dwObjNumX;  // grid width
//     int32_t dwObjNumZ;  // grid height
//     float fLeft, fTop;  // world position offset
// };
// Output: .hgt dengan world_scale yang benar
```

### Fix 2: Fog + Ambient dari Scene JSON
```cpp
// client/engine/EngineMap.cpp — parsing:
// "fog": {"start": 50, "end": 300, "density": 0.5, "color": "0xAABBCCFF"}
// "ambient": "0x202020FF"
// → set ke EnvData untuk TerrainRenderer
```

### Fix 3: Color Palette
```cpp
// client/ui/ColorPalette.hpp — konstanta warna
// Ganti semua hardcoded color di dialog dengan ColorPalette::*
```
