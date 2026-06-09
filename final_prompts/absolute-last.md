# ABSOLUTE-LAST — Fix All Broken Asset References

Lokasi: /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn
Build: cmake --build build/macos-debug -j$(sysctl -n hw.ncpu) && /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/build/macos-debug/bin/test_runner

## Tugas

Dari audit asset relationship, ditemukan broken references di 3 area. Fix SEMUA.

## Aturan Ketat

1. ✅ Cek dulu apakah file sudah ada — jangan buat ulang
2. 🔧 Jika memang missing — baru fix
3. Build + test setelah selesai — 0 error, 0 test failure

---

## PART 1: Fix Animasi Missing & Typo (17 file)

### 1a. Typo: `bose_` → `boss_`

```bash
cd /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/assets/animations

# Cari file dengan prefix "bose_" (saat ini typo, harusnya "boss_")
ls bose_*.anm.json 2>/dev/null
# Contoh: bose_leostein_01.anm.json, bose_leostein_02.anm.json

# RENAME ke boss_:
for f in bose_*.anm.json; do
  newname=$(echo "$f" | sed 's/^bose_/boss_/')
  if [ ! -f "$newname" ]; then
    mv "$f" "$newname"
    echo "RENAMED: $f → $newname"
  fi
done
```

### 1b. Typo: `lihgt` → `light`

```bash
ls *lihgt*.anm.json 2>/dev/null
# Contoh: 92_candlelihgt01.anm.json
# Skenario: file .chr adalah "candlelight", animasi seharusnya "candlelight"
# Cek apakah file candlelight sudah ada:
ls 92_candlelight*.anm.json 2>/dev/null
# Jika candlelight belum ada, rename candlelihgt → candlelight
```

### 1c. Animasi missing — 2 boss tanpa animasi

```bash
# boss_arach.chx — butuh minimal 1 animasi
# boss_kierra.chx — butuh minimal 1 animasi
# Buat animasi placeholder (idle stand):
for boss in boss_arach boss_kierra; do
  if [ ! -f "${boss}_01.anm.json" ]; then
    # Copy dari animasi boss terdekat yang sudah ada
    # atau buat minimal JSON:
    echo '{
  "header": { "frame_speed": 1.0, "last_frame": 1, "ticks_per_frame": 1 },
  "objects": []
}' > "${boss}_01.anm.json"
    echo "CREATED: ${boss}_01.anm.json (placeholder)"
  fi
done
```

### 1d. Missing anim files (12 lainnya)

Untuk setiap model .chr di scene.json yang punya expected .anm.json tapi tidak ada:
```bash
# Cari .chr refs di scene.json
# Cek apakah .anm.json dengan nama yang sama ada
# Jika tidak ada — buat placeholder kecil
```

---

## PART 2: Fix NPC Shop — 83 Item ID

### 2a. Identifikasi item_id yang valid

```bash
cd /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn

# Cek range item_id yang ada
sqlite3 assets/data/game_data.db "SELECT MIN(id), MAX(id), COUNT(*) FROM item_templates;"
# Contoh hasil: min=1, max=28000, count=27475
```

### 2b. Cari mapping item_id lama → baru

```bash
# Untuk setiap item_id broken di game_dealitem
# Cari di item_templates dengan id terdekat
# Update reference
sqlite3 assets/data/game_data.db "
  UPDATE game_dealitem SET item_id = (
    SELECT id FROM item_templates WHERE id = game_dealitem.item_id
  )
  WHERE item_id NOT IN (SELECT id FROM item_templates);
"
```

Jika item_id lama tidak ada sama sekali di item_templates, cari berdasarkan nama:
```bash
# Cek nama item dari game_itemlist (Old DB)
sqlite3 assets/data/game_data.db "
  SELECT i.col_0001 FROM game_itemlist i
  JOIN game_dealitem d ON i.col_0000 = d.item_id
  WHERE d.item_id NOT IN (SELECT id FROM item_templates)
  LIMIT 10;
"
# Lalu cari item dengan nama mirip di item_templates
# Update dengan item_id yang valid
```

---

## PART 3: Fix Buff Skills — 5.593 Orphaned

### 3a. Cek struktur tabel

```bash
sqlite3 assets/data/game_data.db ".schema game_skill_buff_list"
# Cari kolom yang menjadi foreign key ke skill
# Biasanya col_0004 = skill_ref_id
```

### 3b. Update atau hapus orphaned

```bash
# Opsi A: Hapus orphaned entries (jika tidak dibutuhkan)
sqlite3 assets/data/game_data.db "
  DELETE FROM game_skill_buff_list
  WHERE col_0004 NOT IN (SELECT col_0000 FROM game_skilllist);
"
echo "Deleted orphaned buff entries"

# Opsi B: Update ke skill_id valid (jika dibutuhkan)
# Cari mapping skill_name → skill_id
```

---

## PART 3 ALT (Safe): Jika DB read-only

Jika game_data.db dianggap read-only (tidak boleh diubah karena berasal dari Old yang sudah fix), cukup buat **view atau tabel mapping**:

```sql
-- Buat tabel mapping di Reborn DB
CREATE TABLE IF NOT EXISTS item_id_mapping (
    old_id INTEGER PRIMARY KEY,
    new_id INTEGER NOT NULL
);
INSERT OR IGNORE INTO item_id_mapping
SELECT d.item_id, COALESCE(t.id, 1)
FROM game_dealitem d
LEFT JOIN item_templates t ON d.item_id = t.id;
```

---

## VERIFIKASI

Setelah semua fix, jalankan ulang audit:

```bash
# 1. Animasi
echo "=== Animasi ===" && find assets/animations/ -name "boss_arach*" -o -name "boss_kierra*" | wc -l

# 2. NPC Shop
echo "=== NPC Shop broken ===" && sqlite3 assets/data/game_data.db "SELECT COUNT(*) FROM game_dealitem d LEFT JOIN item_templates i ON d.item_id = i.id WHERE i.id IS NULL;"

# 3. Buff
echo "=== Buff orphaned ===" && sqlite3 assets/data/game_data.db "SELECT COUNT(*) FROM game_skill_buff_list b LEFT JOIN game_skilllist s ON b.col_0004 = s.col_0000 WHERE s.col_0000 IS NULL;"

# 4. Build
echo "=== Build ===" && cmake --build build/macos-debug -j$(sysctl -n hw.ncpu) 2>&1 | tail -3

# 5. Test
echo "=== Test ===" && ./build/macos-debug/bin/test_runner 2>&1 | grep "STATUS"
```

## ✅ Kembalikan: "ABSOLUTE-LAST done: fixed X anim typos, Y shop refs, Z buff orphans — 0 errors"
