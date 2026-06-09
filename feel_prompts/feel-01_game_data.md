# FEEL-01 — Load Game Data: Items, Monsters, Skills

Lokasi: /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn
Build: cmake --build build/macos-debug -j$(sysctl -n hw.ncpu)

## Tugas

Old punya data game di `Luna-Plus-Old/LunaPlus/Data/game_data.db` (SQLite). Reborn punya schema tabel di `database/schema_game_sqlite.sql` (75 tabel) dan loader di `game/ecs/systems/GameDataDB.hpp`. TANPA data ini, angka di game (damage, EXP, gold, item stats) TIDAK AKAN SAMA dengan Old.

## Aturan Ketat

1. BACA `game/ecs/systems/GameDataDB.hpp` — pahami method yang sudah ada
2. BACA `database/schema_game_sqlite.sql` — cari tabel content (item_templates, monster_templates, skill_data, dll.)
3. ✅ Jika data SUDAH ADA di Reborn — skip, log ukuran data
4. 🔧 Jika BELUM ADA — copy data dari Old game_data.db ke Reborn assets/data/game_data.db
5. Pastikan format SQLite compatible (Old pakai SQLite version apa? Reborn pakai 3.51.0)
6. Build verify setelah selesai

## Data yang harus di-load

Cek dengan:
```bash
# Old database
sqlite3 /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Old/LunaPlus/Data/game_data.db ".tables"

# Reborn database  
sqlite3 assets/data/game_data.db ".tables"
```

### Tabel yang harus di-copy:

| Old Table | Reborn Schema Table | Kritis? |
|-----------|-------------------|---------|
| `Item` | `item_templates` | ✅ P0 — Damage, defense, price |
| `Monster` | `monster_templates` | ✅ P0 — HP, ATK, DEF, EXP |
| `Skill` | `skill_data` | ✅ P0 — Damage mult, cooldown |
| `Drop` | `monster_drops` | ✅ P0 — Loot tables |
| `Spawn` | `monster_spawns` | ✅ P0 — Monster positions |
| `NPC` | `npc_templates` + `npc_positions` | ✅ P1 — Shop items |
| `Quest` | `quest_templates` + `quest_conditions` | ✅ P1 |
| `Map` | `map_data` + `map_warps` | ✅ P1 |

### Proses:

```bash
# 1. Cek apakah Reborn sudah punya data
sqlite3 assets/data/game_data.db "SELECT count(*) FROM item_templates;"

# 2. Jika 0 row → copy dari Old
sqlite3 /path/to/Old/game_data.db ".dump Item" | sqlite3 assets/data/game_data.db

# 3. Verifikasi
sqlite3 assets/data/game_data.db "SELECT count(*) FROM item_templates;"
```

Jika format kolom berbeda (Old pakai nama kolom lama, Reborn pakai nama baru), buat SQL migration:
```sql
INSERT INTO item_templates (id, name, item_type, attack, defense, price_buy)
SELECT item_id, item_name, item_type, attack, defense, buy_price FROM Old.Item;
```

## Output

✅ Kembalikan: "FEEL-01 done: loaded X items, Y monsters, Z skills, W drops"
