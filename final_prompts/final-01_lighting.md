# FINAL-01 — Render & Lighting Alignment (DX9 fixed-function look)

Lokasi: /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn
Build: cmake --build build/macos-debug -j$(sysctl -n hw.ncpu)

## Tugas

Approximate DX9 fixed-function lighting look using bgfx shader parameters. Old DX9 used: directional light + ambient + diffuse + specular with Gouraud shading. bgfx Metal uses PBR. Kita tidak bisa 100% sama, tapi bisa didekati.

## Aturan Ketat

1. BACA dulu file shader dan renderer sebelum ubah
2. JANGAN ubah format asset — hanya parameter rendering
3. Build verify

## File yang harus dicek

```bash
ls shaders/*.sc shaders/*.bin 2>/dev/null
cat engine/gx_render/RenderDevice.h | head -50
rg "light\|Light\|ambient\|Ambient\|diffuse\|Diffuse\|fog\|Fog" engine/gx_render/ --type cpp -l
```

## Yang harus diubah

### 1. Ambient Light Setting
Old DX9 punya ambient light color default (sRGB: ~0.2, 0.2, 0.3).
```cpp
// Cari di engine/gx_render/ — set ambient light
// Jika belum ada, tambah uniform:
bgfx::setUniform(u_ambient, &ambient_color);
```

### 2. Directional Light
Old pakai 1 directional light dari atas-kiri.
```cpp
struct LightUniforms { float dir[4]; float ambient[4]; float diffuse[4]; };
// Set light direction mirip DX9 default (45° ke kiri-atas)
float light_dir[] = { 0.5f, -0.8f, 0.3f, 0.0f };
```

### 3. Fog
Old punya fog (linear, warna hitam, jarak tertentu).
```cpp
// Cek apakah shader sudah punya fog uniform
// Jika belum, tambah: fog_color, fog_start, fog_end
```

### 4. Cari reference Old
```bash
# Cari DX9 shader file atau pipeline state
rg "D3DRS_AMBIENT\|D3DLIGHT9\|SetRenderState\|LightEnable" /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Old/4DYUCHIGX_RENDER/ --type cpp -l
```

## Output

✅ Kembalikan: "FINAL-01 done: lighting aligned — ambient, directional, fog set"
