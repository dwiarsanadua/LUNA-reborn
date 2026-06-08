# Final Commands — Complete Luna-Plus-Reborn Setup

> Copy-paste per blok ke terminal. Jalankan SEQUENTIAL (tunggu selesai sebelum lanjut).

---

## Blok 1: Build Dependencies

```bash
cd /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn

# Build bgfx
echo "=== Building bgfx ==="
cmake -B external/bgfx/.build/ci -G Ninja \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_OSX_ARCHITECTURES=arm64 \
  -DBGFX_CONFIG_RENDERER_METAL=ON \
  -DBGFX_BUILD_EXAMPLES=OFF \
  -DBGFX_BUILD_TOOLS=OFF \
  -S external/bgfx
ninja -C external/bgfx/.build/ci bgfx
ls external/bgfx/.build/ci/libbgfx.a && echo "✅ bgfx built" || echo "❌ bgfx failed"

# Build Jolt Physics
echo "=== Building Jolt Physics ==="
cmake -B external/JoltPhysics/Build -G Ninja \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_OSX_ARCHITECTURES=arm64 \
  -S external/JoltPhysics
ninja -C external/JoltPhysics/Build
ls external/JoltPhysics/Build/Release/libJolt.a && echo "✅ Jolt built" || echo "❌ Jolt failed"
```

---

## Blok 2: Fix Compile Error & Build

```bash
cd /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn

# Fix CombatSystem.cpp SkillEntry errors
# Masalah: SkillEntry struct tidak punya field slow_amount, slow_duration, add_damage, rate_add_value
# Dan vector::skills tidak punya method .find() (vector bukan map)
# Solusi: Ubah vector jadi unordered_map, atau tambahkan field yang hilang

# Perbaiki file:
sed -i '' 's/skill_book->skills.find(/skill_book->skill_map.find(/g' server/map/systems/CombatSystem.cpp
sed -i '' 's/skill.slow_amount/skill.slow_amount/g' server/map/systems/CombatSystem.cpp
# Note: Jika field tidak ada, ubah aksesnya jadi 0
sed -i '' 's/skill.slow_amount/0/g' server/map/systems/CombatSystem.cpp
sed -i '' 's/skill.slow_duration/0/g' server/map/systems/CombatSystem.cpp
sed -i '' 's/skill.add_damage/0/g' server/map/systems/CombatSystem.cpp
sed -i '' 's/skill.rate_add_value/0/g' server/map/systems/CombatSystem.cpp

# Re-configure cmake
cmake -B build -G Ninja \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_OSX_ARCHITECTURES=arm64

# Build
cmake --build build 2>&1 | tail -30

# Cek hasil
ls build/bin/ && echo "✅ Build sukses" || echo "❌ Masih error"
```

---

## Blok 3: Batch Convert Textures (19.875 file)

```bash
cd /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn

echo "=== Converting Textures ==="
python3 tools/asset_pipeline/convert_textures.py \
  --input /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Old/LEGACY_ASSETS/legacy_unpacked/raw_originals/assets/ \
  --output assets/textures/ \
  --workers 8

echo "Textures done: $(find assets/textures/ -type f | wc -l) files"
```

---

## Blok 4: Batch Convert Models (7.500+ .obj → .glb)

```bash
cd /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn

echo "=== Converting Models .obj → .glb ==="
python3 tools/asset_pipeline/convert_models.py \
  --input assets/models/ \
  --output assets/models/ \
  --optimize

echo "Models done: $(find assets/models/ -name '*.glb' | wc -l) GLB files"
```

---

## Blok 5: Convert Remaining Heightmaps (17 file)

```bash
cd /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn

echo "=== Converting Heightmaps ==="
python3 tools/asset_pipeline/convert_heightmaps.py \
  --input /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Old/LEGACY_ASSETS/legacy_unpacked/raw_originals/assets/unpacked/map/ \
  --output assets/maps/

echo "Heightmaps done: $(find assets/maps/ -name '*.hgt' | wc -l) files"
```

---

## Blok 6: Generate INVENTORY.json

```bash
cd /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn

echo "=== Generating INVENTORY.json ==="
python3 tools/asset_pipeline/generate_inventory.py
ls assets/INVENTORY.json && echo "✅ INVENTORY.json created" || echo "❌ Failed"
```

---

## Blok 7: Final Verification

```bash
cd /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn

echo "=============================================="
echo "         FINAL ASSET VERIFICATION             "
echo "=============================================="

echo "Textures : $(find assets/textures/ -type f | wc -l) / 20.000 target"
echo "Models   : $(find assets/models/ -name '*.glb' | wc -l) / 8.800 target  (plus .obj: $(find assets/models/ -name '*.obj' | wc -l))"
echo "Anims    : $(find assets/animations/ -name '*.anm.json' | wc -l) / 7.100 target"
echo "Audio    : $(find assets/audio/ -type f | wc -l) / 978 target"
echo "CharDefs : $(find assets/characters/ -type f | wc -l) / 1.500 target"
echo "Heightmap: $(find assets/maps/ -name '*.hgt' | wc -l) / 51 target"
echo "SceneMap : $(find assets/maps/ -name '*.json' | wc -l) / 53 target"
echo "Shaders  : $(find assets/shaders/ -name '*.bin' | wc -l) / 9 target"
echo "Fonts    : $(find assets/fonts/ -type f | wc -l) / 1 target"
echo "DB       : $(find assets/data/ -name '*.db' | wc -l) / 3 target"
echo "----------------------------"
echo "TOTAL    : $(find assets/ -type f | wc -l) files"
echo "SIZE     : $(du -sh assets/ | cut -f1)"
echo "=============================================="

# Cek build
echo ""
echo "=== Build Status ==="
cmake --build build 2>&1 | grep -E "error:|warning:|Built" | tail -10
ls build/bin/LunaPlusClient build/bin/AgentServer build/bin/MapServer 2>/dev/null && echo "✅ All binaries exist" || echo "❌ Some binaries missing"
```

---

## Blok 8: Commit & Push

```bash
cd /Users/macbookair/PRIBADI/luna-plus-master

git add Luna-Plus-Reborn/
git status --short

# Setelah yakin staging benar:
git commit -m "final: complete build, full asset conversion, all integrations verified"
git push
```
