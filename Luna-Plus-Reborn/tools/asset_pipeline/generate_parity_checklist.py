#!/usr/bin/env python3
"""Generate docs/PARITY_CHECKLIST.md from legacy MP_CATEGORY vs Reborn implementation."""

import re
import sys
from datetime import date
from pathlib import Path

BASE = Path(__file__).resolve().parents[2]
ROOT = BASE.parent
PROTO_OLD = ROOT / "Luna-Plus-Old" / "[CC]Header" / "Protocol.h"
PACKET_FBS = BASE / "game" / "network" / "protocol" / "PacketType.fbs"
OUT = BASE / "docs" / "PARITY_CHECKLIST.md"

# Manual reborn coverage notes per legacy MP_CATEGORY
STATUS = {
    "MP_SERVER": ("partial", "Internal server routing only"),
    "MP_CHAR": ("done", "Char life, stats, respawn sync"),
    "MP_USERCONN": ("done", "Login, char list/make, game in, changemap"),
    "MP_MOVE": ("done", "Walk/run sync + monster broadcast"),
    "MP_SKILL": ("done", "Cast + combat attack validation"),
    "MP_ITEM": ("partial", "Loot, inventory sync; compose/enchant UI only"),
    "MP_CHAT": ("done", "All/whisper/party/guild chat"),
    "MP_PARTY": ("done", "Create/invite/leave + party chat"),
    "MP_FRIEND": ("done", "Add/del/list"),
    "MP_GUILD": ("done", "Create/info/member + guild chat"),
    "MP_EXCHANGE": ("done", "Player trade sync"),
    "MP_STORAGE": ("done", "Bank deposit/withdraw"),
    "MP_CONSIGNMENT": ("done", "Auction house + search"),
    "MP_STREETSTALL": ("done", "Personal + NPC stalls"),
    "MP_QUEST": ("done", "713+ quests, Lua FSM, DB persistence"),
    "MP_DUNGEON": ("done", "Entrance + info sync"),
    "MP_TRIGGER": ("done", "Map triggers + client notify"),
    "MP_FAMILY": ("done", "Family + marriage actions (Phase 6)"),
    "MP_MARRIAGE": ("done", "Via Family system (propose/accept/divorce)"),
    "MP_PET": ("done", "Feed/evolve/summon + DB (Phase 6)"),
    "MP_FISHING": ("partial", "Cast + loot; minigame UI simplified"),
    "MP_FARM": ("done", "Plant/water/harvest + DB (Phase 6)"),
    "MP_SIEGEWAR": ("partial", "Siege declare/tax/sim resolve (Phase 6)"),
    "MP_SIEGEWARFARE": ("partial", "Alias coverage via Siege tab"),
    "MP_GTOURNAMENT": ("partial", "Bracket sim + register/claim (Phase 6)"),
    "MP_HOUSE": ("partial", "Buy/enter/place furniture; no 3D editor"),
    "MP_CHEAT": ("missing", "GM tools not ported"),
    "MP_MORNITORTOOL": ("partial", "Monitor tool stub in tools/monitor"),
    "MP_MORNITORSERVER": ("partial", "DistributeServer registry"),
    "MP_MORNITORMAPSERVER": ("partial", "MapServer register/list"),
    "MP_AUTOPATCH": ("partial", "Launcher + bspatch + manifest"),
    "MP_OPTION": ("partial", "Options dialog; not all legacy toggles"),
    "MP_NPC": ("partial", "Dialog + shop; not all NPC scripts"),
    "MP_MONSTER": ("done", "Spawn, AI, combat, loot"),
    "MP_BATTLE": ("done", "Combat + skills"),
    "MP_WEATHER": ("partial", "Client weather FX; limited server sync"),
    "MP_COOK": ("partial", "Cooking dialog offline"),
    "MP_VEHICLE": ("partial", "Mount dialog offline"),
    "MP_MAIL": ("partial", "Mail dialog UI"),
    "MP_NOTE": ("partial", "Mail/notes UI overlap"),
}


def parse_mp_categories(path: Path) -> list[str]:
    text = path.read_text(encoding="utf-8", errors="replace")
    block = re.search(r"enum MP_CATEGORY\s*\{(.*?)\};", text, re.S)
    if not block:
        raise RuntimeError("MP_CATEGORY enum not found")
    cats = []
    for line in block.group(1).splitlines():
        line = line.strip().split("//")[0].strip().rstrip(",")
        if not line or line.startswith("/*"):
            continue
        cats.append(line)
    return cats


def parse_packet_tokens(path: Path) -> set[str]:
    text = path.read_text(encoding="utf-8")
    return set(re.findall(r"(MP_[A-Z0-9_]+)", text))


def infer_status(name: str, tokens: set[str]) -> tuple[str, str]:
    if name in STATUS:
        return STATUS[name]
    prefix = name.replace("MP_", "")
    hits = [t for t in tokens if prefix in t or name.replace("MP_", "MP_") in t]
    if hits:
        return ("partial", f"{len(hits)} packet(s) in PacketType.fbs")
    return ("missing", "Not wired in Reborn")


def generate() -> str:
    cats = parse_mp_categories(PROTO_OLD)
    tokens = parse_packet_tokens(PACKET_FBS)
    done = partial = missing = 0
    lines = [
        "# LUNA Plus Reborn — Protocol Parity Checklist",
        "",
        f"> Generated: {date.today().isoformat()}",
        f"> Legacy source: `{PROTO_OLD.relative_to(ROOT)}`",
        f"> Reborn packets: `{PACKET_FBS.relative_to(BASE)}`",
        "",
        "| # | MP_CATEGORY | Status | Notes |",
        "|---|-------------|--------|-------|",
    ]
    for i, cat in enumerate(cats, 1):
        if cat == "MP_MAX":
            continue
        status, note = infer_status(cat, tokens)
        if status == "done":
            done += 1
        elif status == "partial":
            partial += 1
        else:
            missing += 1
        icon = {"done": "✅", "partial": "🟡", "missing": "⬜"}.get(status, "?")
        lines.append(f"| {i} | `{cat}` | {icon} {status} | {note} |")

    total = done + partial + missing
    pct = int(100 * (done + partial * 0.5) / total) if total else 0
    lines.extend([
        "",
        "## Summary",
        "",
        f"- **Done:** {done}",
        f"- **Partial:** {partial}",
        f"- **Missing:** {missing}",
        f"- **Weighted progress:** ~{pct}%",
        "",
        "Regenerate: `python3 tools/asset_pipeline/generate_parity_checklist.py`",
    ])
    return "\n".join(lines) + "\n"


def main() -> int:
    if not PROTO_OLD.is_file():
        print(f"FAIL: missing {PROTO_OLD}")
        return 1
    OUT.parent.mkdir(parents=True, exist_ok=True)
    OUT.write_text(generate(), encoding="utf-8")
    print(f"Wrote {OUT}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
