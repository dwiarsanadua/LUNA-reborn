# Agent-01 — Database Schema: Fix Missing Columns

Lokasi: /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn
Build: cmake --build build/macos-debug -j$(sysctl -n hw.ncpu)

## Tugas

Dari verifikasi agents sebelumnya, ditemukan 2 kolom database yang MISSING:
1. `TB_GUILD` — kolom `GP` (INTEGER) tidak ada, tapi diklaim ada di DB_QUERY_MAPPING.md
2. `TB_PARTY` — kolom `Option` (INTEGER) tidak ada, tapi diklaim ada di DB_QUERY_MAPPING.md

## Aturan Ketat

1. BACA database/schema_game_sqlite.sql — cari CREATE TABLE TB_GUILD dan TB_PARTY
2. CEK apakah kolom GP dan Option SUDAH ADA:
   - Jika SUDAH ADA ✅ → skip, jangan ubah apapun
   - Jika BELUM ADA 🔧 → tambah ALTER TABLE di bagian migrasi
3. JANGAN ubah CREATE TABLE — tambah ALTER TABLE di bagian "_migration_version"
4. Build verify setelah selesai

## File yang harus dicek

database/schema_game_sqlite.sql — cari:
- `CREATE TABLE TB_GUILD` — catat semua kolom yang ada
- `CREATE TABLE TB_PARTY` — catat semua kolom yang ada

Jika kolom GP tidak ada di TB_GUILD, tambah di bagian migrasi:
```sql
-- Migration v2: Add missing columns
ALTER TABLE TB_GUILD ADD COLUMN GP INTEGER DEFAULT 0;
```

Jika kolom Option tidak ada di TB_PARTY:
```sql
ALTER TABLE TB_PARTY ADD COLUMN Option INTEGER DEFAULT 0;
```

## Output

✅ Kembalikan: "Agent-01 done: [GP: sudah ada/ditambah] [Option: sudah ada/ditambah]"
