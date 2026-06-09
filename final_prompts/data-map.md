# DATA-MAP — Mapping Korean Names & Old IDs ke Modern Item ID

Lokasi: /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn
Build: cmake --build build/macos-debug -j$(sysctl -n hw.ncpu)

## Tugas

Monster_drops, npc_shop_entries, dan game_skill_buff_list berisi Korean item names dan Old numeric IDs yang tidak match dengan item_templates modern. Buat mapping script untuk fix ini.

---

## PART 1: Korean Name → Modern ID Mapping

### Problem
993 monster_drops dan 2.472 npc_shop_entries punya item_id = Korean name (text), bukan integer.

### Fix Strategy
Gunakan `game_itemlist` (Old DB — punya 27.475 item dengan Korean names + Old IDs) sebagai jembatan:

```
game_itemlist.col_0000 = Old ID (integer)
game_itemlist.col_0001 = Korean name (text)
```

Mapping: Korean name → game_itemlist.col_0000 → item_templates.id

### Script

```bash
cd /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn

python3 << 'PYEOF'
import sqlite3, sys, re

db = sqlite3.connect("assets/data/game_data.db")
db.row_factory = sqlite3.Row

# Step 1: Build mapping dari game_itemlist
# game_itemlist: col_0000=OldID, col_0001=KoreanName
old_to_modern = {}  # OldID → ModernID
korean_to_modern = {}  # KoreanName → ModernID
korean_to_old = {}  # KoreanName → OldID

# Cari kolom game_itemlist
cols = [c[1] for c in db.execute("PRAGMA table_info(game_itemlist)")]
print(f"game_itemlist columns: {cols[:5]}...")

rows = db.execute("SELECT * FROM game_itemlist").fetchall()
print(f"Total game_itemlist rows: {len(rows)}")

for row in rows:
    old_id = row[0]
    korean_name = row[1] if row[1] else ""
    # Clean Korean name: remove special chars
    clean_name = korean_name.replace("^_", " ").replace("^", "").strip()
    
    # Cari modern ID dengan offset +11000000
    modern_id = None
    if isinstance(old_id, int) and old_id > 0:
        candidate = old_id + 11000000
        exists = db.execute("SELECT id FROM item_templates WHERE id = ?", (candidate,)).fetchone()
        if exists:
            modern_id = candidate
            old_to_modern[old_id] = modern_id
    
    if clean_name:
        korean_to_old[clean_name] = old_id
        if modern_id:
            korean_to_modern[clean_name] = modern_id

print(f"Old→Modern mapped: {len(old_to_modern)}")
print(f"Korean→Modern mapped: {len(korean_to_modern)}")

# Step 2: Fix monster_drops
print("\n=== Fixing monster_drops ===")
fixed_korean = 0
fixed_oldid = 0

# 2a: Fix Korean names
rows = db.execute("SELECT rowid, item_id, item_name FROM monster_drops WHERE typeof(item_id) = 'text'").fetchall()
for row in rows:
    korean = row[1].replace("^_", " ").replace("^", "").strip()
    if korean in korean_to_modern:
        modern_id = korean_to_modern[korean]
        db.execute("UPDATE monster_drops SET item_id = ? WHERE rowid = ?", (modern_id, row[0]))
        fixed_korean += 1
    elif korean in korean_to_old:
        old_id = korean_to_old[korean]
        if old_id in old_to_modern:
            db.execute("UPDATE monster_drops SET item_id = ? WHERE rowid = ?", (old_to_modern[old_id], row[0]))
            fixed_korean += 1

# 2b: Fix Old integer IDs (offset +11000000)
rows = db.execute("""
    SELECT rowid, item_id FROM monster_drops 
    WHERE typeof(item_id) = 'integer' AND item_id > 0 
    AND item_id NOT IN (SELECT id FROM item_templates)
""").fetchall()
for row in rows:
    old_id = row[1]
    if old_id in old_to_modern:
        db.execute("UPDATE monster_drops SET item_id = ? WHERE rowid = ?", (old_to_modern[old_id], row[0]))
        fixed_oldid += 1
    else:
        # Try direct offset
        candidate = old_id + 11000000
        exists = db.execute("SELECT id FROM item_templates WHERE id = ?", (candidate,)).fetchone()
        if exists:
            db.execute("UPDATE monster_drops SET item_id = ? WHERE rowid = ?", (candidate, row[0]))
            fixed_oldid += 1

db.commit()
print(f"Fixed Korean names: {fixed_korean}")
print(f"Fixed Old IDs: {fixed_oldid}")

# Step 3: Fix npc_shop_entries
print("\n=== Fixing npc_shop_entries ===")
fixed_shop_korean = 0
fixed_shop_oldid = 0

# Cari tabel shop yang relevan
tables = [r[0] for r in db.execute("SELECT name FROM sqlite_master WHERE type='table'")]
shop_tables = [t for t in tables if 'shop' in t.lower() or 'deal' in t.lower()]

for tbl in shop_tables:
    try:
        cols = [c[1] for c in db.execute(f"PRAGMA table_info({tbl})")]
        # Cari kolom yang berisi item_id
        id_cols = [c for c in cols if 'item' in c.lower() or 'id' in c.lower() or 'col_0000' in c.lower()]
        if not id_cols:
            continue
            
        id_col = id_cols[0]
        
        # Fix Korean names
        rows = db.execute(f"SELECT rowid, {id_col} FROM \"{tbl}\" WHERE typeof({id_col}) = 'text'").fetchall()
        for row in rows:
            korean = row[1].replace("^_", " ").replace("^", "").strip()
            if korean in korean_to_modern:
                db.execute(f"UPDATE \"{tbl}\" SET {id_col} = ? WHERE rowid = ?", (korean_to_modern[korean], row[0]))
                fixed_shop_korean += 1
        
        # Fix Old IDs
        rows = db.execute(f"""
            SELECT rowid, {id_col} FROM \"{tbl}\" 
            WHERE typeof({id_col}) = 'integer' AND {id_col} > 0 
            AND {id_col} NOT IN (SELECT id FROM item_templates)
        """).fetchall()
        for row in rows:
            old_id = row[1]
            candidate = old_id + 11000000
            exists = db.execute("SELECT id FROM item_templates WHERE id = ?", (candidate,)).fetchone()
            if exists:
                db.execute(f"UPDATE \"{tbl}\" SET {id_col} = ? WHERE rowid = ?", (candidate, row[0]))
                fixed_shop_oldid += 1
                
    except Exception as e:
        print(f"  Error on {tbl}: {e}")

db.commit()
print(f"Fixed shop Korean names: {fixed_shop_korean}")
print(f"Fixed shop Old IDs: {fixed_shop_oldid}")

# Step 4: Fix buff_skills
print("\n=== Fixing buff_skills ===")
try:
    # Cek struktur
    cols = [c[1] for c in db.execute("PRAGMA table_info(buff_skills)")]
    print(f"  buff_skills columns: {cols}")
    
    # Cari kolom reference skill
    ref_cols = [c for c in cols if 'skill' in c.lower() or 'ref' in c.lower()]
    if ref_cols:
        ref_col = ref_cols[0]
        broken = db.execute(f"SELECT COUNT(*) FROM buff_skills b LEFT JOIN skill_data s ON b.{ref_col} = s.id WHERE s.id IS NULL").fetchone()[0]
        print(f"  Broken before: {broken}")
        
        # Coba offset mapping untuk skill
        rows = db.execute(f"SELECT rowid, {ref_col} FROM buff_skills WHERE {ref_col} NOT IN (SELECT id FROM skill_data)").fetchall()
        for row in rows:
            old_id = row[1]
            # Skill ID kemungkinan perlu offset berbeda
            # Cek range skill_data.id
        print(f"  SKIP: need manual skill ID mapping")
except Exception as e:
    print(f"  Error: {e}")

db.close()

# Final verification
print("\n=== FINAL VERIFICATION ===")
db = sqlite3.connect("assets/data/game_data.db")
broken_drops = db.execute("SELECT COUNT(*) FROM monster_drops d LEFT JOIN item_templates i ON d.item_id = i.id WHERE i.id IS NULL AND typeof(d.item_id) = 'integer' AND d.item_id > 0").fetchone()[0]
print(f"Monster drops still broken: {broken_drops}")
broken_shops = 0
for tbl in shop_tables:
    try:
        cols = [c[1] for c in db.execute(f"PRAGMA table_info({tbl})")]
        id_cols = [c for c in cols if 'item' in c.lower() or 'id' in c.lower() or 'col_0000' in c.lower()]
        if id_cols:
            b = db.execute(f"SELECT COUNT(*) FROM \"{tbl}\" s LEFT JOIN item_templates i ON s.{id_cols[0]} = i.id WHERE i.id IS NULL AND typeof(s.{id_cols[0]}) = 'integer' AND s.{id_cols[0]} > 0").fetchone()[0]
            broken_shops += b
    except: pass
print(f"NPC shop still broken: {broken_shops}")
db.close()
PYEOF
```

### Verifikasi

```bash
cd /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn

echo "=== Monster Drops ==="
sqlite3 assets/data/game_data.db "
SELECT COUNT(*) FROM monster_drops d 
LEFT JOIN item_templates i ON d.item_id = i.id 
WHERE i.id IS NULL AND typeof(d.item_id) = 'integer' AND d.item_id > 0;
"

echo "=== NPC Shop ==="
sqlite3 assets/data/game_data.db "
SELECT COUNT(*) FROM npc_shop_entries s 
LEFT JOIN item_templates i ON s.item_id = i.id 
WHERE i.id IS NULL AND typeof(s.item_id) = 'integer' AND s.item_id > 0;
"

echo "=== Korean names remaining ==="
sqlite3 assets/data/game_data.db "
SELECT COUNT(*) FROM monster_drops WHERE typeof(item_id) = 'text';
"
```

---

## PART 2: Items that REFUSE mapping

If after the script there are still items that can't be mapped:

```bash
# List unmappable items for manual review
sqlite3 assets/data/game_data.db "
SELECT DISTINCT item_id FROM monster_drops 
WHERE typeof(item_id) = 'text'
LIMIT 20;
"
```

For each, search in game_itemlist for the Korean name, then manually map to the closest modern item_templates.id. If impossible, set as item_id = 0 (no drop).

---

## ✅ Kembalikan: "DATA-MAP done: mapped X Korean names, Y Old IDs — Z remaining unmappable (manual)"
