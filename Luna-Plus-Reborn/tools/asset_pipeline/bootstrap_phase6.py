#!/usr/bin/env python3
"""Bootstrap Phase 6: secondary feature seeds (family, pet, fish, siege, shop)."""

import sqlite3
import subprocess
import sys
from pathlib import Path

BASE = Path(__file__).resolve().parents[2]
DATA = BASE / "assets" / "data"
MAP_DB = DATA / "luna_map.db"
MIGRATE = BASE / "tools" / "asset_pipeline" / "migrate_schema_sqlite.py"

FISH_SEED = [
    (21000001, "Common Carp", 0, 1.0),
    (21000002, "Silver Trout", 1, 1.5),
    (21000003, "Golden Koi", 2, 2.0),
]

TERRITORY_SEED = [
    (1, "Alker Harbor", 0, "", 10),
    (2, "Red Orc Outpost", 0, "", 12),
    (3, "Siege Castle", 0, "", 15),
]

SHOP_SEED = [
    (1001, "Health Potion", 50, "Consumables"),
    (1002, "Mana Potion", 50, "Consumables"),
    (201, "Pet Summon Scroll", 200, "Pets"),
    (301, "Costume Box", 500, "Cosmetics"),
    (401, "Fishing Bait x10", 30, "Misc"),
]


def run_migration() -> None:
    subprocess.run([sys.executable, str(MIGRATE)], check=True)


def seed_phase6(cur: sqlite3.Cursor) -> None:
    for row in FISH_SEED:
        cur.execute(
            "INSERT OR REPLACE INTO phase6_fish_types (item_id, name, rarity, weight) "
            "VALUES (?, ?, ?, ?)",
            row,
        )
    for row in TERRITORY_SEED:
        cur.execute(
            "INSERT OR REPLACE INTO phase6_territories "
            "(territory_id, name, owner_guild_id, owner_guild_name, tax_rate) "
            "VALUES (?, ?, ?, ?, ?)",
            row,
        )
    for row in SHOP_SEED:
        cur.execute(
            "INSERT OR REPLACE INTO phase6_shop_items (item_id, name, price, category) "
            "VALUES (?, ?, ?, ?)",
            row,
        )

    cur.execute(
        "INSERT OR IGNORE INTO TB_PET "
        "(PetIdx, CharacterIdx, PetDBIdx, PetName, Level, HP, Satiation) "
        "VALUES (1, 1, 1, 'Fluffy', 1, 100, 100)"
    )
    cur.execute(
        "INSERT OR IGNORE INTO TB_HOUSE "
        "(HouseIdx, CharacterIdx, MapIdx, PosX, PosY, HouseType, FurnitureCount) "
        "VALUES (1, 1, 51, 10.0, 0.0, 0, 0)"
    )


def main() -> int:
    print("Phase 6 bootstrap:")
    run_migration()
    DATA.mkdir(parents=True, exist_ok=True)
    conn = sqlite3.connect(MAP_DB)
    seed_phase6(conn.cursor())
    fish = conn.execute("SELECT COUNT(*) FROM phase6_fish_types").fetchone()[0]
    shop = conn.execute("SELECT COUNT(*) FROM phase6_shop_items").fetchone()[0]
    conn.commit()
    conn.close()
    print(f"  phase6_fish_types: {fish}")
    print(f"  phase6_shop_items: {shop}")
    print("Phase 6 bootstrap OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
