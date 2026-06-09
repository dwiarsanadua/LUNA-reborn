# Agent FINAL-D — Verifikasi & Koreksi DB_QUERY_MAPPING.md

Lokasi: /Users/macbookair/PRIBADI/luna-plus-master
Build: cmake --build Luna-Plus-Reborn/build/macos-debug -j$(sysctl -n hw.ncpu)

## Tugas

Baca DB_QUERY_MAPPING.md, lalu verifikasi SETIAP kolom mapping dengan membaca actual database schema.

## Aturan Ketat (WAJIB)

1. **🚫 JANGAN PERCAYA tabel**. Setiap kolom harus diverifikasi.
2. **✅ Jika mapping sudah benar**: skip.
3. **🔧 Jika mapping salah**: koreksi di DB_QUERY_MAPPING.md.
4. **🚫 JANGAN ubah schema SQL**. Hanya update dokumen.
5. **🚫 JANGAN build**.

## Item yang harus diverifikasi

### 1. TB_CHARACTER — semua kolom
- Baca database/schema_game_sqlite.sql — cari CREATE TABLE TB_CHARACTER
- Catat SEMUA kolom beserta tipe data SQLite
- Bandingkan dengan tabel di DB_QUERY_MAPPING.md:
  - Apakah ada kolom yang disebut "missing" tapi sebenarnya SUDAH ADA?
  - Apakah ada kolom yang disebut "ada" tapi sebenarnya TIDAK ADA?
  - Apakah tipe data SQLite sesuai?

### 2. TB_ITEM — semua kolom
- Baca CREATE TABLE TB_ITEM dari schema
- Verifikasi: Durability, StorageType, Grade, Socket, Element, dll.

### 3. TB_GUILD — MarkData & EmblemData
- Apakah kolom MarkData, MarkLen, EmblemData, EmblemLen benar-benar ada?
- Baca CREATE TABLE TB_GUILD

### 4. TB_FRIEND — Memo column
- Apakah kolom Memo benar-benar ada di TB_FRIEND?
- Baca CREATE TABLE TB_FRIEND

### 5. Total Tables Count
- Hitung aktual:
  ```
  rg "^CREATE TABLE" database/schema_game_sqlite.sql | wc -l
  ```

### 6. Query Templates
- Baca bagian "-- SQL QUERY TEMPLATES" di schema_game_sqlite.sql
- Berapa banyak query template yang tercatat?
- Apakah sesuai dengan klaim "50+ query templates" di DB_QUERY_MAPPING.md?

## Output

Update DB_QUERY_MAPPING.md hanya untuk kolom yang TERBUKTI SALAH.
Untuk setiap koreksi:
```
[TABEL]   <nama tabel>
[KOLOM]   <nama kolom>
[OLD]     Status salah: <klaim>
[NEW]     Status benar: <data aktual>
[BUKTI]   database/schema_game_sqlite.sql:<line>
```

## ✅ Kembalikan "Agent FINAL-D done: DB_QUERY_MAPPING.md verified, X corrections"
