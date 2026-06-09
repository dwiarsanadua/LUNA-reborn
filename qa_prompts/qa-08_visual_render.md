# QA-08 — Visual & Render Test: Bgfx Initialization, Resource Loading

Lokasi: /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn
Build: cmake --build build/macos-debug -j$(sysctl -n hw.ncpu) --target test_runner

## Tugas

Buat `tools/test_runner/visual.cpp` — test untuk render pipeline, resource loading, dan validasi visual asset.

## Aturan Ketat

1. ✅ Panggil method/API yang SUDAH ADA — jangan buat implementasi baru
2. 🔧 Jika butuh loading asset — path relative ke `assets/` dari project root
3. JANGAN buka GLFW window — test cukup sampai validasi resource handle
4. Build + run — 0 failure

## Test Scenarios

### Test 1: Texture Loading (bgfx)
```
TEST_STEP("Texture: load PNG from disk → valid bgfx handle");
```
- Baca client/rendering/ — cari Texture class atau loadTexture function
- Load file PNG dari assets/textures/ (cari file yang ada)
- Test: texture handle != BGFX_INVALID_HANDLE
- Test: width/height > 0
- Log: "Loaded texture: assets/textures/<file> = 512x512"

Cari method dengan:
```bash
rg "loadTexture\|LoadTexture\|createTexture\|TextureHandle" client/rendering/ --type cpp
```

### Test 2: Model Loading (Assimp → GLB)
```
TEST_STEP("Model: load GLB file → valid Assimp scene");
```
- Cari di tools/ atau client/ — apakah ada model loader
- Coba load file GLB dari assets/models/
- Test: scene != nullptr
- Test: scene->mNumMeshes > 0
- Log: "Loaded model: <file> — X meshes, Y vertices"

### Test 3: Animation Loading (.anm.json)
```
TEST_STEP("Animation: load .anm.json → valid animation data");
```
- Cari AnmParser atau AnimationSystem
- Load file .anm.json dari assets/animations/
- Test: parser sukses
- Test: frame count > 0, duration > 0

### Test 4: Shader Compilation (bgfx)
```
TEST_STEP("Shader: load .bin shader → valid bgfx program handle");
```
- Cari shader loader di engine/gx_render/
- Load vertex + fragment shader dari assets/shaders/ atau shaders/
- Test: program handle valid
- Log vertex/fragment shader names

### Test 5: Font Loading (FreeType)
```
TEST_STEP("Font: load .ttf → valid font face");
```
- Cari font loader — cek FontManager atau UIRenderer
- Load 2002_EYA.ttf dari assets/fonts/
- Test: face != nullptr
- Test: glyph_count > 0, font_size > 0

### Test 6: UI Skin/Atlas Loading
```
TEST_STEP("UI Atlas: load skin texture → valid atlas entries");
```
- Cek UiSkinManager atau UiAtlasRegistry
- Load texture atlas
- Test: atlas entries > 0
- Test: entry memiliki valid UV coordinates

### Test 7: Heightmap Loading (.hgt)
```
TEST_STEP("Heightmap: load .hgt → valid terrain data");
```
- Cari HflParser atau heightmap loader
- Load file .hgt dari assets/maps/
- Test: width/height > 0
- Test: height data array tidak kosong

### Test 8: Scene JSON Loading
```
TEST_STEP("Scene: load scene.json → valid scene data");
```
- Cari map/scene loader
- Load file scene.json dari assets/maps/
- Test: parsed valid
- Test: object/entity list > 0

## Output

✅ Kembalikan: "QA-08 done: visual/render resource loading tests — X passed"
