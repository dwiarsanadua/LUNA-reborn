#!/usr/bin/env python3
"""
LUNA Plus Reborn — Database Migration Tool (Phase 1.0)
Migrates game content from legacy DB to luna_game.db.

Source: assets/data/game_data_legacy.db  (normalized game content tables)
Target: assets/data/luna_game.db        (runtime + content tables)

Architecture:
  game_data.db            — raw col_NNNN parsed data (archive/legacy)
  game_data_legacy.db     — normalized named-column game content
  luna_game.db            — runtime player data + migrated game content

Migration strategy:
  1. Read all game content tables from game_data_legacy.db
  2. Create/add content tables to luna_game.db
  3. Add indexes on key lookup columns
  4. Report tables copied, skipped/deprecated, row counts
"""

import sqlite3
import sys
import time
from pathlib import Path

BASE = Path(__file__).resolve().parents[2]
ASSETS_DATA = BASE / "assets" / "data"

LEGACY_SRC = ASSETS_DATA / "game_data_legacy.db"
GAME_DATA_SRC = ASSETS_DATA / "game_data.db"
LUNA_DB_DST = ASSETS_DATA / "luna_game.db"

DEPRECATED_TABLE_PATTERNS = [
    "game_vs_",        # shader tables (not game data)
    "game_bodylist_",  # avatar customization (legacy format)
    "game_facelist_",
    "game_hairlist_",
    "game_modlist_",
    "game_monster_63_",  # per-difficulty monster overrides
    "game_monster_64_",
    "game_monster_65_",
    "game_monster_limit_",
    "game_emoticon_back",  # merge targets
    "game_emoticon_front",
    "game_challengezonemonster",  # per-zone monster overrides
    "game_vs_",
]

RELEVANT_CONTENT_TABLES = [
    "item_templates",
    "monster_templates",
    "monster_drops",
    "npc_templates",
    "npc_positions",
    "npc_shop_entries",
    "quest_templates",
    "quest_conditions",
    "quest_strings",
    "skill_data",
    "buff_skills",
    "skill_trees",
    "map_warps",
    "map_boundaries",
    "map_data",
    "monster_spawns",
]

CONTENT_INDEXES = {
    "item_templates": [
        "CREATE INDEX IF NOT EXISTS idx_item_templates_type ON item_templates(item_type)",
        "CREATE INDEX IF NOT EXISTS idx_item_templates_subtype ON item_templates(item_subtype)",
        "CREATE INDEX IF NOT EXISTS idx_item_templates_level ON item_templates(level_required)",
    ],
    "monster_templates": [
        "CREATE INDEX IF NOT EXISTS idx_monster_templates_level ON monster_templates(level)",
        "CREATE INDEX IF NOT EXISTS idx_monster_templates_zone ON monster_templates(monster_type)",
        "CREATE INDEX IF NOT EXISTS idx_monster_templates_element ON monster_templates(element_type)",
    ],
    "monster_drops": [
        "CREATE INDEX IF NOT EXISTS idx_monster_drops_monster ON monster_drops(monster_id)",
        "CREATE INDEX IF NOT EXISTS idx_monster_drops_item ON monster_drops(item_id)",
    ],
    "npc_templates": [
        "CREATE INDEX IF NOT EXISTS idx_npc_templates_type ON npc_templates(npc_type)",
    ],
    "npc_positions": [
        "CREATE INDEX IF NOT EXISTS idx_npc_positions_map ON npc_positions(map_id)",
        "CREATE INDEX IF NOT EXISTS idx_npc_positions_npc ON npc_positions(npc_id)",
    ],
    "npc_shop_entries": [
        "CREATE INDEX IF NOT EXISTS idx_npc_shop_npc ON npc_shop_entries(npc_id)",
        "CREATE INDEX IF NOT EXISTS idx_npc_shop_item ON npc_shop_entries(item_id)",
    ],
    "quest_templates": [
        "CREATE INDEX IF NOT EXISTS idx_quest_templates_level ON quest_templates(level_required)",
        "CREATE INDEX IF NOT EXISTS idx_quest_templates_giver ON quest_templates(giver_npc_id)",
    ],
    "quest_conditions": [
        "CREATE INDEX IF NOT EXISTS idx_quest_conditions_quest ON quest_conditions(quest_id)",
    ],
    "quest_strings": [
        "CREATE INDEX IF NOT EXISTS idx_quest_strings_quest ON quest_strings(quest_id)",
    ],
    "skill_data": [
        "CREATE INDEX IF NOT EXISTS idx_skill_data_class ON skill_data(class_id)",
        "CREATE INDEX IF NOT EXISTS idx_skill_data_type ON skill_data(skill_type)",
        "CREATE INDEX IF NOT EXISTS idx_skill_data_level ON skill_data(level_required)",
    ],
    "buff_skills": [
        "CREATE INDEX IF NOT EXISTS idx_buff_skills_ref ON buff_skills(skill_ref_id)",
        "CREATE INDEX IF NOT EXISTS idx_buff_skills_type ON buff_skills(buff_type)",
    ],
    "skill_trees": [
        "CREATE INDEX IF NOT EXISTS idx_skill_trees_class ON skill_trees(class_id)",
        "CREATE INDEX IF NOT EXISTS idx_skill_trees_skill ON skill_trees(skill_id)",
    ],
    "map_warps": [
        "CREATE INDEX IF NOT EXISTS idx_map_warps_from ON map_warps(map_from)",
        "CREATE INDEX IF NOT EXISTS idx_map_warps_to ON map_warps(map_to)",
    ],
    "map_boundaries": [
        "CREATE INDEX IF NOT EXISTS idx_map_boundaries_a ON map_boundaries(map_a)",
        "CREATE INDEX IF NOT EXISTS idx_map_boundaries_b ON map_boundaries(map_b)",
    ],
    "monster_spawns": [
        "CREATE INDEX IF NOT EXISTS idx_monster_spawns_map ON monster_spawns(map_id)",
        "CREATE INDEX IF NOT EXISTS idx_monster_spawns_monster ON monster_spawns(monster_id)",
    ],
    "map_data": [
        "CREATE INDEX IF NOT EXISTS idx_map_data_name ON map_data(name)",
    ],
}


def is_deprecated(table: str) -> bool:
    for pat in DEPRECATED_TABLE_PATTERNS:
        if table.startswith(pat):
            return True
    return False


def get_table_schema(db: sqlite3.Connection, table: str) -> list[tuple]:
    return db.execute(f"PRAGMA table_info([{table}])").fetchall()


def copy_table(src: sqlite3.Connection, dst: sqlite3.Connection, table: str) -> int:
    schema = get_table_schema(src, table)
    if not schema:
        return 0

    col_names = [row[1] for row in schema]
    col_defs = ", ".join(
        f'"{row[1]}" {row[2]}'
        + (" PRIMARY KEY" if row[5] else "")
        + (" AUTOINCREMENT" if row[2].upper().startswith("INTEGER") and row[5] else "")
        + (f" DEFAULT {row[4]}" if row[4] is not None else "")
        + (" NOT NULL" if row[3] else "")
        for row in schema
    )

    dst.execute(f"DROP TABLE IF EXISTS [{table}]")
    dst.execute(f"CREATE TABLE [{table}] ({col_defs})")

    rows = src.execute(f"SELECT * FROM [{table}]").fetchall()
    placeholders = ", ".join("?" for _ in col_names)
    col_list = ", ".join(f'"{c}"' for c in col_names)
    dst.executemany(
        f"INSERT INTO [{table}] ({col_list}) VALUES ({placeholders})", rows
    )
    return len(rows)


def create_indexes(dst: sqlite3.Connection, table: str):
    for idx_sql in CONTENT_INDEXES.get(table, []):
        try:
            dst.execute(idx_sql)
        except sqlite3.Error as e:
            print(f"    [WARN] Index failed on {table}: {e}")


def count_table_rows(db: sqlite3.Connection, table: str) -> int:
    try:
        return db.execute(f"SELECT COUNT(*) FROM [{table}]").fetchone()[0]
    except sqlite3.Error:
        return 0


def add_migration_version(dst: sqlite3.Connection, version: str, description: str):
    dst.execute("""
        CREATE TABLE IF NOT EXISTS _migration_version (
            version TEXT PRIMARY KEY,
            applied_at TEXT NOT NULL DEFAULT (datetime('now')),
            description TEXT
        )
    """)
    dst.execute(
        "INSERT OR IGNORE INTO _migration_version (version, description) VALUES (?, ?)",
        (version, description),
    )


def main():
    print("=" * 64)
    print("  LUNA Plus Reborn — Database Migration Tool")
    print("=" * 64)

    # ── Validate sources ──────────────────────────────────────
    if not LEGACY_SRC.exists():
        print(f"\n[ERROR] Legacy source not found: {LEGACY_SRC}")
        print("[INFO]  No migration needed — run data_parser.py first")
        sys.exit(1)

    legacy_size = LEGACY_SRC.stat().st_size
    print(f"\n  Source:      {LEGACY_SRC.name} ({legacy_size / 1024:.1f} KB)")
    print(f"  Destination: {LUNA_DB_DST.name}")

    # ── Connect ───────────────────────────────────────────────
    src = sqlite3.connect(str(LEGACY_SRC))
    src.row_factory = sqlite3.Row
    src.execute("PRAGMA synchronous = OFF")

    dst = sqlite3.connect(str(LUNA_DB_DST))
    dst.execute("PRAGMA synchronous = OFF")
    dst.execute("PRAGMA journal_mode = MEMORY")
    dst.execute("PRAGMA foreign_keys = OFF")

    # ── Discover legacy tables ────────────────────────────────
    legacy_tables = {
        row[0]
        for row in src.execute(
            "SELECT name FROM sqlite_master WHERE type='table'"
        ).fetchall()
    }
    print(f"\n  Legacy tables found: {len(legacy_tables)}")

    # ── Check for deprecated tables in game_data.db ───────────
    deprecated_found = []
    if GAME_DATA_SRC.exists():
        gd = sqlite3.connect(str(GAME_DATA_SRC))
        gd_tables = {
            row[0]
            for row in gd.execute(
                "SELECT name FROM sqlite_master WHERE type='table'"
            ).fetchall()
        }
        deprecated_found = [t for t in gd_tables if is_deprecated(t)]
        gd.close()

    # ── Migrate content tables ────────────────────────────────
    tables_copied = []
    tables_skipped = []
    total_rows = 0

    for table in RELEVANT_CONTENT_TABLES:
        if table not in legacy_tables:
            tables_skipped.append((table, "not found in source"))
            continue

        try:
            rows = copy_table(src, dst, table)
            create_indexes(dst, table)
            tables_copied.append((table, rows))
            total_rows += rows
            print(f"  [OK]  {table:30s} → {rows:>6d} rows")
        except sqlite3.Error as e:
            tables_skipped.append((table, str(e)))
            print(f"  [ERR] {table:30s} → {e}")

    # ── Record migration version ──────────────────────────────
    add_migration_version(
        dst, "1.0.0", "Initial migration from game_data_legacy.db to luna_game.db"
    )

    dst.commit()
    dst.execute("VACUUM")

    # ── Report ────────────────────────────────────────────────
    print(f"\n{'=' * 64}")
    print(f"  MIGRATION REPORT")
    print(f"{'=' * 64}")
    print(f"  Tables copied:    {len(tables_copied)}")
    for name, count in tables_copied:
        print(f"    {name:30s} {count:>6d} rows")
    print(f"  Total rows migrated: {total_rows:,}")

    if tables_skipped:
        print(f"\n  Tables skipped:   {len(tables_skipped)}")
        for name, reason in tables_skipped:
            print(f"    {name:30s} ({reason})")

    if deprecated_found:
        print(f"\n  Deprecated tables identified in game_data.db: {len(deprecated_found)}")
        for t in sorted(deprecated_found):
            print(f"    {t}")

    dst_size = LUNA_DB_DST.stat().st_size
    print(f"\n  Database size:    {dst_size / 1024:.1f} KB")
    print(f"  Migration version: 1.0.0")
    print(f"{'=' * 64}")

    src.close()
    dst.close()


if __name__ == "__main__":
    main()
