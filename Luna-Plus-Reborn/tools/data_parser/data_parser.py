#!/usr/bin/env python3
"""
LUNA Plus Reborn — Data Parser (Phase 0.3)
Parses 282 game data files (.bin.txt) → SQLite database.

Input:  NEW_LUNA/data/*.bin.txt (TSV, EUC-KR, ^s sub-separator)
Output: LUNA-Plus-Reborn/assets/data/game_data.db
"""

import os
import sys
import sqlite3
import csv
import re
from pathlib import Path

DATA_SRC = Path("/Users/macbookair/PRIBADI/luna-plus-master/NEW_LUNA/data")
DB_OUT = Path("/Users/macbookair/PRIBADI/luna-plus-master/LUNA-Plus-Reborn/assets/data/game_data.db")


def decode_euckr(bytes_data: bytes) -> str:
    try:
        return bytes_data.decode("euc-kr")
    except UnicodeDecodeError:
        return bytes_data.decode("euc-kr", errors="replace")


def parse_fields(line: str) -> list[str]:
    stripped = line.strip("\r\n")
    cells = stripped.split("\t")
    return [cell.replace("^s", "\x1f") for cell in cells]


def table_name_from_file(filename: str) -> str:
    name = filename.removesuffix(".bin.txt")
    name = re.sub(r"^(\d+)_", "", name)
    name = re.sub(r"_\d+$", "", name)
    name = re.sub(r"[^a-zA-Z0-9_]", "_", name).lower()
    name = re.sub(r"_+", "_", name).strip("_")
    if not name:
        name = f"table_{hash(filename) & 0xFFFFFFF}"
    return f"game_{name}"


def process_file(filepath: Path, db: sqlite3.Connection) -> tuple[str, int, int]:
    """Process a single .bin.txt file. Returns (table_name, row_count, col_count)."""
    filename = filepath.name
    table = table_name_from_file(filename)

    with open(filepath, "rb") as f:
        raw = f.read()

    text = decode_euckr(raw)
    lines = text.strip().split("\n")
    rows = [parse_fields(l) for l in lines if l.strip()]

    if not rows:
        return table, 0, 0

    col_count = max(len(r) for r in rows)

    # Normalize all rows to same column count
    normalized = []
    for row in rows:
        while len(row) < col_count:
            row.append("")
        normalized.append(row)

    # Escape column names
    cols = [f"col_{i:04d}" for i in range(col_count)]

    # Try to use first data row to detect types (all numeric → INTEGER)
    col_types = []
    for i in range(col_count):
        vals = [r[i] for r in normalized[:5] if r[i]]
        is_int = all(v.lstrip("-").isdigit() for v in vals if v)
        col_types.append("INTEGER" if is_int else "TEXT")

    col_defs = ", ".join(f'"{c}" {t}' for c, t in zip(cols, col_types))
    placeholders = ", ".join("?" for _ in cols)

    # Drop and recreate table
    db.execute(f"DROP TABLE IF EXISTS [{table}]")
    db.execute(f"CREATE TABLE [{table}] ({col_defs})")

    # Insert rows
    insert_sql = f"INSERT INTO [{table}] ({', '.join(f'\"{c}\"' for c in cols)}) VALUES ({placeholders})"
    db.executemany(insert_sql, normalized)

    # Create index on first column (usually ID)
    try:
        db.execute(f"CREATE INDEX IF NOT EXISTS idx_{table}_col0 ON [{table}]({cols[0]})")
    except sqlite3.Error:
        pass

    return table, len(normalized), col_count


def main():
    if not DATA_SRC.is_dir():
        print(f"ERROR: Data directory not found: {DATA_SRC}")
        sys.exit(1)

    files = sorted(DATA_SRC.glob("*.bin.txt"))
    print(f"Found {len(files)} game data files in {DATA_SRC}")

    DB_OUT.parent.mkdir(parents=True, exist_ok=True)
    if DB_OUT.exists():
        DB_OUT.unlink()

    db = sqlite3.connect(str(DB_OUT))
    db.execute("PRAGMA synchronous = OFF")
    db.execute("PRAGMA journal_mode = MEMORY")
    db.execute("PRAGMA cache_size = -64000")

    total_rows = 0
    parsed = 0
    skipped = 0
    for f in files:
        try:
            table, rows, cols = process_file(f, db)
            total_rows += rows
            parsed += 1
            status = "OK" if rows > 0 else "EMPTY"
            print(f"  {status:5} | {table:40s} | {rows:5d} rows | {cols} cols | {f.name}")
            db.commit()
        except Exception as e:
            print(f"  ERROR  | {f.name:47s} | {e}")
            skipped += 1

    # Create metadata table
    db.execute("""
        CREATE TABLE IF NOT EXISTS _metadata (
            file TEXT PRIMARY KEY,
            table_name TEXT,
            row_count INTEGER,
            col_count INTEGER
        )
    """)
    for f in files:
        table = table_name_from_file(f.name)
        try:
            row_count = db.execute(f"SELECT COUNT(*) FROM [{table}]").fetchone()[0]
            col_count = len(db.execute(f"PRAGMA table_info([{table}])").fetchall())
            db.execute(
                "INSERT INTO _metadata VALUES (?, ?, ?, ?)",
                (f.name, table, row_count, col_count),
            )
        except sqlite3.Error:
            pass
    db.commit()

    print(f"\n{'='*60}")
    print(f"  Parsed: {parsed} files")
    print(f"  Skipped: {skipped} files")
    print(f"  Total rows: {total_rows:,}")
    print(f"  Database: {DB_OUT}")
    print(f"  Size: {DB_OUT.stat().st_size / 1024:.1f} KB")

    db.close()


if __name__ == "__main__":
    main()
