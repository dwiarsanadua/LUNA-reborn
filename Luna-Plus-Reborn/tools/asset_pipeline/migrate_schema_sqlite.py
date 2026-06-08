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
