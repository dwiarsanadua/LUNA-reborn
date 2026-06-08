#!/usr/bin/env python3
"""Apply SQLite schema migration for Phase 5 (ODBC -> SQLite parity)."""

import sqlite3
from pathlib import Path

BASE = Path(__file__).resolve().parents[2]
DATA = BASE / "assets" / "data"
SCHEMA_GAME = BASE / "database" / "schema_game_sqlite.sql"
SCHEMA_MAP = BASE / "database" / "schema_map_server.sql"

DATABASES = {
    "member": DATA / "luna_member.db",
    "agent": DATA / "luna_agent.db",
    "map": DATA / "luna_map.db",
}


def apply_sql(conn: sqlite3.Connection, sql_path: Path) -> None:
    if not sql_path.is_file():
        raise FileNotFoundError(sql_path)
    conn.executescript(sql_path.read_text(encoding="utf-8"))


def ensure_map_column_migrations(conn: sqlite3.Connection) -> None:
    """Add columns introduced after initial Phase 6 deploy."""
    rows = conn.execute(
        "SELECT name FROM sqlite_master WHERE type='table' AND name='player_pet'"
    ).fetchall()
    if not rows:
        return
    cols = {r[1] for r in conn.execute("PRAGMA table_info(player_pet)")}
    if "evolution" not in cols:
        conn.execute("ALTER TABLE player_pet ADD COLUMN evolution INTEGER DEFAULT 1")
    if "exp" not in cols:
        conn.execute("ALTER TABLE player_pet ADD COLUMN exp INTEGER DEFAULT 0")
    rows = conn.execute(
        "SELECT name FROM sqlite_master WHERE type='table' AND name='player_family'"
    ).fetchall()
    if rows:
        fam_cols = {r[1] for r in conn.execute("PRAGMA table_info(player_family)")}
        if "married_date" not in fam_cols:
            conn.execute("ALTER TABLE player_family ADD COLUMN married_date INTEGER DEFAULT 0")
    terr_rows = conn.execute(
        "SELECT name FROM sqlite_master WHERE type='table' AND name='phase6_territories'"
    ).fetchall()
    if terr_rows:
        terr_cols = {r[1] for r in conn.execute("PRAGMA table_info(phase6_territories)")}
        additions = [
            ("siege_time", "INTEGER DEFAULT 0"),
            ("attacker_guild_id", "INTEGER DEFAULT 0"),
            ("attacker_guild_name", "TEXT DEFAULT ''"),
            ("is_castle", "INTEGER DEFAULT 0"),
            ("defense_bonus", "INTEGER DEFAULT 0"),
            ("tax_accumulated", "INTEGER DEFAULT 0"),
        ]
        for col, typedef in additions:
            if col not in terr_cols:
                conn.execute(f"ALTER TABLE phase6_territories ADD COLUMN {col} {typedef}")
    t_rows = conn.execute(
        "SELECT name FROM sqlite_master WHERE type='table' AND name='phase6_tournaments'"
    ).fetchall()
    if t_rows:
        t_cols = {r[1] for r in conn.execute("PRAGMA table_info(phase6_tournaments)")}
        if "prize_claimed" not in t_cols:
            conn.execute("ALTER TABLE phase6_tournaments ADD COLUMN prize_claimed INTEGER DEFAULT 0")
    shop_rows = conn.execute(
        "SELECT name FROM sqlite_master WHERE type='table' AND name='phase6_shop_items'"
    ).fetchall()
    if shop_rows:
        shop_cols = {r[1] for r in conn.execute("PRAGMA table_info(phase6_shop_items)")}
        shop_additions = [
            ("description", "TEXT DEFAULT ''"),
            ("currency_type", "INTEGER DEFAULT 0"),
            ("stack_count", "INTEGER DEFAULT 1"),
            ("max_purchase", "INTEGER DEFAULT 99"),
            ("on_sale", "INTEGER DEFAULT 0"),
            ("sale_price", "INTEGER DEFAULT 0"),
        ]
        for col, typedef in shop_additions:
            if col not in shop_cols:
                conn.execute(f"ALTER TABLE phase6_shop_items ADD COLUMN {col} {typedef}")


def migrate_database(label: str, db_path: Path) -> int:
    DATA.mkdir(parents=True, exist_ok=True)
    conn = sqlite3.connect(db_path)
    if label == "map":
        conn.executescript("""
        DROP TABLE IF EXISTS quest_conditions;
        DROP TABLE IF EXISTS quest_strings;
        DROP TABLE IF EXISTS quest_templates;
        DROP TABLE IF EXISTS map_triggers;
        DROP TABLE IF EXISTS player_quests;
        """)
    apply_sql(conn, SCHEMA_GAME)
    if label == "map" and SCHEMA_MAP.is_file():
        apply_sql(conn, SCHEMA_MAP)
        ensure_map_column_migrations(conn)
    tables = conn.execute(
        "SELECT COUNT(*) FROM sqlite_master WHERE type='table'"
    ).fetchone()[0]
    conn.commit()
    conn.close()
    print(f"  {label}: {tables} tables in {db_path.name}")
    return tables


def main() -> int:
    print("Phase 5 SQLite migration:")
    for label, path in DATABASES.items():
        migrate_database(label, path)
    print("SQLite migration OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
