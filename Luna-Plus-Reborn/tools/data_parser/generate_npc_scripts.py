#!/usr/bin/env python3
"""
Generate NPC dialog Lua scripts from legacy NPC data files.

Parses:
  Luna-Plus-Old/NEW_LUNA/data/Npc_Script.bin.txt
  Luna-Plus-Old/NEW_LUNA/data/Npc_Msg.bin.txt
  Luna-Plus-Old/NEW_LUNA/data/Npc_HyperText.bin.txt
  Reborn/assets/data/npcs.json

Output:
  Reborn/assets/scripts/NPC/npc_XXXX.lua
"""

import json
import re
from pathlib import Path

BASE = Path(__file__).resolve().parents[2]
REPO_ROOT = BASE.parent
OLD_DATA = REPO_ROOT / "Luna-Plus-Old" / "NEW_LUNA" / "data"
OUTPUT_DIR = BASE / "assets" / "scripts" / "NPC"
NPC_JSON = BASE / "assets" / "data" / "npcs.json"

SCRIPT_FILE = OLD_DATA / "Npc_Script.bin.txt"
MSG_FILE = OLD_DATA / "Npc_Msg.bin.txt"
HYPERTEXT_FILE = OLD_DATA / "Npc_HyperText.bin.txt"


def esc(s: str) -> str:
    s = s.replace("^s", " ").replace("\x1f", " ")
    s = re.sub(r'[\x00-\x08\x0b\x0c\x0e-\x1f]', '', s)
    s = re.sub(r'\s+', ' ', s).strip()
    return s.replace("\\", "\\\\").replace('"', '\\"').replace("\n", "\\n")


def parse_messages(path: Path) -> dict[int, str]:
    msgs = {}
    text = path.read_text(encoding="latin-1")
    for m in re.finditer(r'#Msg\s+(\d+)\s*\{\s*(.*?)\s*\}', text, re.DOTALL):
        mid = int(m.group(1))
        content = m.group(2).strip()
        content = content.encode("latin-1").decode("utf-8", errors="replace")
        msgs[mid] = content
    return msgs


def parse_hypertext(path: Path) -> dict[int, str]:
    ht = {}
    for line in path.read_text(encoding="latin-1").splitlines():
        line = line.strip()
        if not line or "\t" not in line:
            continue
        parts = line.split("\t", 1)
        try:
            hid = int(parts[0])
            ht[hid] = parts[1].strip()
        except ValueError:
            continue
    return ht


def _find_brace_block(text: str, start: int) -> tuple[str, int]:
    """Extract content inside matching braces from start position."""
    if text[start] != '{':
        return "", start
    depth = 1
    i = start + 1
    while i < len(text) and depth > 0:
        if text[i] == '{':
            depth += 1
        elif text[i] == '}':
            depth -= 1
        i += 1
    return text[start + 1: i - 1], i


def parse_npc_scripts(path: Path) -> dict[int, list]:
    npcs = {}
    text = path.read_text(encoding="latin-1")
    i = 0
    while i < len(text):
        # Find $NPC marker
        idx = text.find('$NPC', i)
        if idx == -1:
            break
        brace = text.find('{', idx)
        if brace == -1:
            break
        body, end = _find_brace_block(text, brace)
        i = end

        # Extract NPC ID
        nid_match = re.search(r'#NPCID\s+(\d+)', body)
        if not nid_match:
            continue
        nid = int(nid_match.group(1))

        # Extract pages
        pages = []
        pos = 0
        while True:
            pidx = body.find('$PAGE', pos)
            if pidx == -1:
                break
            pbrace = body.find('{', pidx)
            if pbrace == -1:
                break
            pbody, pend = _find_brace_block(body, pbrace)
            pos = pend

            page_info = re.search(r'#PAGEINFO\s+([\d\s]+)', pbody)
            dialogues = re.findall(r'#DIALOGUE\s+([\d\s]+)', pbody)
            hyperlinks = re.findall(r'#HYPERLINK\s+([\d\s]+)', pbody)

            info_parts = (
                page_info.group(1).strip().split() if page_info else []
            )
            page_data = {
                "page_id": int(info_parts[0]) if info_parts else 0,
                "dialogue_ids": [
                    int(x) for d in dialogues for x in d.strip().split()
                ],
                "links": [],
            }
            for h in hyperlinks:
                parts = h.strip().split()
                if len(parts) >= 2:
                    page_data["links"].append({
                        "text_id": int(parts[0]),
                        "action": (
                            int(parts[1]) if len(parts) > 1 else 0
                        ),
                        "target_page": (
                            int(parts[2]) if len(parts) > 2 else 0
                        ),
                    })
            pages.append(page_data)

        if pages:
            npcs[nid] = pages
    return npcs


def generate_npc_lua(npc_id: int, name: str, pages: list,
                     msgs: dict, hypertext: dict) -> str:
    lines = [
        f"-- NPC {npc_id}: {name}",
        "-- MapScriptRuntime NPC dialog hook",
        "local npc = {",
        f'  id = {npc_id},',
        f'  name = "{esc(name)}",',
        '  pages = {',
    ]

    for i, page in enumerate(pages):
        lines.append(f"    [{page['page_id']}] = {{")
        lines.append(f"      page_id = {page['page_id']},")

        dialogues = []
        for did in page["dialogue_ids"]:
            msg = msgs.get(did, "")
            dialogues.append(f'{{ id = {did}, text = "{esc(msg)}" }}')
        if dialogues:
            lines.append(f"      dialogues = {{ {', '.join(dialogues)} }},")
        else:
            lines.append("      dialogues = {},")

        links = []
        for link in page["links"]:
            text = hypertext.get(link["text_id"], f"Option {link['text_id']}")
            links.append(
                '{{ text = "{}", action = {}, target = {} }}'.format(
                    esc(text), link["action"], link["target_page"]
                )
            )
        if links:
            lines.append(f"      links = {{ {', '.join(links)} }},")
        else:
            lines.append("      links = {},")

        lines.append("    },")

    lines.extend([
        "  },",
        "}",
        "",
        f"function npc_get_dialog(npc_id)",
        f'  if npc_id ~= {npc_id} then return nil end',
        "  return npc",
        "end",
        "",
        f"function npc_get_page(npc_id, page_id)",
        f'  if npc_id ~= {npc_id} then return nil end',
        "  for _, p in ipairs(npc.pages) do",
        "    if p.page_id == page_id then return p end",
        "  end",
        "  return nil",
        "end",
        "",
        "return npc",
    ])
    return "\n".join(lines) + "\n"


def main() -> None:
    if not SCRIPT_FILE.is_file():
        raise SystemExit(f"Missing {SCRIPT_FILE}")
    if not MSG_FILE.is_file():
        raise SystemExit(f"Missing {MSG_FILE}")
    if not HYPERTEXT_FILE.is_file():
        raise SystemExit(f"Missing {HYPERTEXT_FILE}")

    if not NPC_JSON.is_file():
        print(f"[warn] {NPC_JSON} not found, NPC names will be empty")
        npc_names = {}
    else:
        with open(NPC_JSON, "r", encoding="utf-8") as f:
            npc_list = json.load(f)
        npc_names = {}
        for n in npc_list:
            name = n["name"]
            name = name.replace("^s", " ").replace("\x1f", " ")
            name = re.sub(r'\s+', ' ', name).strip()
            npc_names[n["id"]] = name

    print("[generate_npc_scripts] Parsing NPC messages...")
    msgs = parse_messages(MSG_FILE)
    print(f"  Loaded {len(msgs)} messages")

    print("[generate_npc_scripts] Parsing hypertext...")
    hypertext = parse_hypertext(HYPERTEXT_FILE)
    print(f"  Loaded {len(hypertext)} hypertext entries")

    print("[generate_npc_scripts] Parsing NPC scripts...")
    npc_scripts = parse_npc_scripts(SCRIPT_FILE)
    print(f"  Found {len(npc_scripts)} NPCs with scripts")

    OUTPUT_DIR.mkdir(parents=True, exist_ok=True)
    count = 0
    for nid in sorted(npc_scripts.keys()):
        name = npc_names.get(nid, f"NPC_{nid}")
        lua = generate_npc_lua(nid, name, npc_scripts[nid], msgs, hypertext)
        path = OUTPUT_DIR / f"npc_{nid:04d}.lua"
        path.write_text(lua, encoding="utf-8")
        count += 1

    print(f"[generate_npc_scripts] Wrote {count} NPC scripts to {OUTPUT_DIR}")


if __name__ == "__main__":
    main()
