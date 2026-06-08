#!/usr/bin/env python3
"""Flatten monsters.json nested spawns into assets/data/spawns.json."""

import json
from pathlib import Path

REBORN = Path(__file__).resolve().parents[2]
MONSTERS = REBORN / "assets/data/monsters.json"
OUT = REBORN / "assets/data/spawns.json"

# Map 51 is the login showcase — original data has no combat spawns; add ambient mobs for QA.
LOGIN_AMBIENT = [
    {"id": 90001, "map_id": 51, "monster_id": 798, "x": 18.0, "y": 0.0, "z": 22.0, "max_count": 2, "respawn_time": 45.0},
    {"id": 90002, "map_id": 51, "monster_id": 107, "x": -12.0, "y": 0.0, "z": 28.0, "max_count": 1, "respawn_time": 45.0},
    {"id": 90003, "map_id": 51, "monster_id": 798, "x": 5.0, "y": 0.0, "z": -15.0, "max_count": 1, "respawn_time": 45.0},
]


def main():
    if not MONSTERS.exists():
        print(f"Missing {MONSTERS}")
        return 1

    with MONSTERS.open() as f:
        monsters = json.load(f)

    spawns = []
    next_id = 1
    for m in monsters:
        mid = m.get("id", 0)
        for s in m.get("spawns", []):
            spawns.append({
                "id": next_id,
                "map_id": s.get("map_id", s.get("map", 0)),
                "monster_id": s.get("monster_id", mid),
                "x": s.get("x", s.get("pos_x", 0.0)),
                "y": s.get("y", s.get("pos_y", 0.0)),
                "z": s.get("z", s.get("pos_z", 0.0)),
                "respawn_time": s.get("respawn_time", s.get("respawn", 10.0)),
                "max_count": s.get("max_count", s.get("count", 1)),
                "aggro_range": s.get("aggro_range", 10.0),
                "patrol_radius": s.get("patrol_radius", 0),
            })
            next_id += 1

    spawns.extend(LOGIN_AMBIENT)
    OUT.parent.mkdir(parents=True, exist_ok=True)
    with OUT.open("w") as f:
        json.dump(spawns, f, indent=2)

    from collections import Counter
    by_map = Counter(s["map_id"] for s in spawns)
    print(f"Wrote {len(spawns)} spawn points to {OUT}")
    print(f"Map 51 spawns: {by_map.get(51, 0)}")
    print(f"Top maps: {by_map.most_common(5)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
