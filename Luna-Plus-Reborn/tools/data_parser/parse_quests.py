#!/usr/bin/env python3
"""
Agent Q — Quest Data Parser (v3)

Parses legacy quest data from QuestScript.bin.txt / QuestString.bin.txt
and outputs assets/data/quests_full.json in the format expected by QuestSystem.

Reads:
  Luna-Plus-Old/NEW_LUNA/data/QuestScript.bin.txt
  Luna-Plus-Old/NEW_LUNA/data/QuestString.bin.txt
  Luna-Plus-Old/NEW_LUNA/data/QuestItemList.bin.txt
  Reborn/assets/data/npcs.json

Writes:
  Reborn/assets/data/quests_full.json
"""

import json
import re
from pathlib import Path

BASE = Path("/Users/macbookair/PRIBADI/luna-plus-master")
OLD_DATA = BASE / "Luna-Plus-Old" / "NEW_LUNA" / "data"
REBORN = BASE / "Luna-Plus-Reborn"
ASSETS = REBORN / "assets" / "data"
OUTPUT = ASSETS / "quests_full.json"

SCRIPT_FILE = OLD_DATA / "QuestScript.bin.txt"
STRING_FILE = OLD_DATA / "QuestString.bin.txt"
ITEM_FILE = OLD_DATA / "QuestItemList.bin.txt"
NPC_FILE = ASSETS / "npcs.json"

S_SEP = re.compile(r"\^s")


def clean_text(raw: str) -> str:
    raw = raw.replace("\t", " ").strip()
    raw = S_SEP.sub(" ", raw)
    while "  " in raw:
        raw = raw.replace("  ", " ")
    return raw.strip()


def load_npc_names() -> dict[int, str]:
    if not NPC_FILE.exists():
        return {}
    with open(NPC_FILE, "r", encoding="utf-8") as f:
        npcs = json.load(f)
    out = {}
    for n in npcs:
        name = n.get("name", "")
        if "\x1f" in name:
            name = name.split("\x1f")[0]
        out[n["id"]] = name
    return out


def tokenize_line(raw: str) -> list[str]:
    raw = raw.strip()
    if not raw:
        return []
    tokens = []
    i, n = 0, len(raw)
    while i < n:
        if raw[i] in (" ", "\t"):
            i += 1
            continue
        start = i
        i += 1
        while i < n and raw[i] not in (" ", "\t"):
            i += 1
        tokens.append(raw[start:i])
    return tokens


QUEST_EVENT_MAP = {
    "@TALKTONPC": "talk",
    "@HUNT": "kill",
    "@COUNT": "count",
    "@GAMEENTER": "game_enter",
    "@LEVEL": "level",
    "@USEITEM": "use_item",
    "@MAPCHANGE": "map_change",
    "@DIE": "die",
    "@HUNTALL": "kill_all",
    "@TIME": "time",
}

EXECUTE_NAMES = {
    "*ENDQUEST": "end_quest",
    "*STARTQUEST": "start_quest",
    "*ENDSUB": "end_sub",
    "*STARTSUB": "start_sub",
    "*ENDOTHERSUB": "end_other_sub",
    "*ADDCOUNT": "add_count",
    "*MINUSCOUNT": "minus_count",
    "*GIVEQUESTITEM": "give_quest_item",
    "*TAKEQUESTITEM": "take_quest_item",
    "*GIVEITEM": "give_item",
    "*GIVEMONEY": "give_money",
    "*TAKEITEM": "take_item",
    "*TAKEMONEY": "take_money",
    "*TAKEEXP": "take_exp",
    "*TAKESEXP": "take_sexp",
    "*RANDOMTAKEITEM": "random_take_item",
    "*SELECTITEM": "select_item",
    "*REGENMONSTER": "regen_monster",
    "*MAPCHANGE": "map_change_ex",
    "*CHANGESTAGE": "change_stage",
    "*REGISTTIME": "regist_time",
    "*ADDCOUNTFQW": "add_count_fqw",
    "*ADDCOUNTFW": "add_count_fw",
    "*TAKEQUESTITEMFQW": "take_quest_item_fqw",
    "*TAKEQUESTITEMFW": "take_quest_item_fw",
    "*TAKEMONEYPERCOUNT": "take_money_per_count",
    "*ADDCOUNTLEVELGAP": "add_count_level_gap",
    "*ADDCOUNTMONLEVEL": "add_count_mon_level",
}

LIMIT_NAMES = {
    "&LEVEL": "level",
    "&MONEY": "money",
    "&QUEST": "quest",
    "&SUBQUEST": "subquest",
    "&STAGE": "stage",
    "&ATTR": "attr",
    "&RUNNING_QUEST": "running_quest",
}


BLOCK_KEYWORDS = {"$QUEST", "$SUBQUEST", "$QUESTSTR", "$SUBQUESTSTR"}

def parse_blocks(content: str) -> list[dict]:
    lines = content.split("\n")
    stack = []
    roots = []
    for raw_line in lines:
        stripped = raw_line.strip()
        if not stripped or stripped.startswith("//"):
            continue
        if stripped == "{":
            continue
        if stripped == "}":
            if stack:
                stack.pop()
            continue
        toks = tokenize_line(stripped)
        if not toks:
            continue
        if toks[0] in BLOCK_KEYWORDS:
            node = {"kind": toks[0], "params": toks[1:], "children": [], "directives": []}
            if stack:
                stack[-1]["children"].append(node)
            else:
                roots.append(node)
            stack.append(node)
        else:
            if stack:
                stack[-1]["directives"].append(toks)
    return roots


def find_nodes(roots: list[dict], kind: str) -> list[dict]:
    result = []
    for r in roots:
        if r["kind"] == kind:
            result.append(r)
        result.extend(find_nodes(r["children"], kind))
    return result


def parse_strings(content: str) -> dict[tuple[int, int], dict]:
    roots = parse_blocks(content)
    result = {}
    for node in find_nodes(roots, "$SUBQUESTSTR"):
        if len(node["params"]) < 2:
            continue
        try:
            qid = int(node["params"][0])
            sqid = int(node["params"][1])
        except ValueError:
            continue

        title = ""
        desc_parts = []
        for toks in node["directives"]:
            if not toks:
                continue
            if toks[0] == "#TITLE":
                title = " ".join(toks[1:])
            elif toks[0] == "#DESC":
                pass
            else:
                text = " ".join(toks).replace("{", "").replace("}", "").strip()
                if text:
                    desc_parts.append(text)

        for child in node["children"]:
            if child["kind"] == "#TITLE":
                title = " ".join(child["params"])
            elif child["kind"] == "#DESC":
                for ctoks in child.get("directives", []):
                    text = " ".join(ctoks).replace("{", "").replace("}", "").strip()
                    if text:
                        desc_parts.append(text)

        result[(qid, sqid)] = {
            "title": clean_text(title),
            "description": clean_text(" ".join(desc_parts)),
        }
    return result


def parse_quest_script(content: str) -> dict[int, dict]:
    roots = parse_blocks(content)
    quests = {}

    for qnode in find_nodes(roots, "$QUEST"):
        params = qnode["params"]
        if len(params) < 2:
            continue
        try:
            qid = int(params[0])
        except ValueError:
            continue

        subquests = []
        for sqnode in find_nodes(qnode["children"], "$SUBQUEST"):
            try:
                sqid = int(sqnode["params"][0])
            except (ValueError, IndexError):
                continue

            sq_data = {"id": sqid, "limits": [], "conditions": [], "executes": [], "npc_scripts": []}

            for toks in sqnode["directives"]:
                if not toks:
                    continue
                d = toks[0].upper()

                if d == "#LIMIT":
                    _parse_limit(toks[1:], sq_data)
                elif d == "#TRIGGER":
                    _parse_trigger_line(toks[1:], sq_data)
                elif d == "#NPCSCRIPT":
                    _parse_npcscript_tokens(toks[1:], sq_data)

            subquests.append(sq_data)

        quests[qid] = {"id": qid, "subquests": subquests}

    return quests


def _parse_limit(toks: list[str], sq_data: dict):
    if not toks:
        return
    lt = toks[0].upper()
    lm = LIMIT_NAMES.get(lt, lt.lower())

    if lm == "level":
        try:
            mn = int(toks[1]) if len(toks) > 1 else 0
            mx = int(toks[2]) if len(toks) > 2 else 0
            sq_data["limits"].append({"type": "level", "min": mn, "max": mx})
        except ValueError:
            pass
    elif lm == "quest":
        try:
            sq_data["limits"].append({"type": "quest", "quest_id": int(toks[1])})
        except (ValueError, IndexError):
            pass
    elif lm == "subquest":
        try:
            sq_data["limits"].append({"type": "subquest", "quest_id": int(toks[1]), "subquest_id": int(toks[2])})
        except (ValueError, IndexError):
            pass
    elif lm == "money":
        try:
            sq_data["limits"].append({"type": "money", "amount": int(toks[1])})
        except (ValueError, IndexError):
            pass
    elif lm == "stage":
        try:
            sq_data["limits"].append({"type": "stage", "stage_id": int(toks[1])})
        except (ValueError, IndexError):
            pass
    else:
        sq_data["limits"].append({"type": lm})


def _parse_trigger_line(toks: list[str], sq_data: dict):
    if not toks:
        return
    event_kind = toks[0].upper()
    mapped = QUEST_EVENT_MAP.get(event_kind)
    if mapped is None:
        return

    params = []
    i = 1
    while i < len(toks):
        if toks[i].startswith("*"):
            break
        try:
            params.append(int(toks[i]))
        except ValueError:
            params.append(toks[i])
        i += 1

    sq_data["conditions"].append({"event": mapped, "params": params})

    while i < len(toks):
        if toks[i].startswith("*"):
            exe_tok = toks[i]
            exe_params = []
            i += 1
            while i < len(toks):
                if toks[i].startswith("*"):
                    break
                try:
                    exe_params.append(int(toks[i]))
                except ValueError:
                    exe_params.append(toks[i])
                i += 1
            sq_data["executes"].append({
                "kind": EXECUTE_NAMES.get(exe_tok, exe_tok),
                "params": exe_params,
            })
        else:
            i += 1


def _parse_npcscript_tokens(toks: list[str], sq_data: dict):
    if not toks:
        return
    offset = 0
    if toks[0].upper() == "@NPC":
        offset = 1
    if len(toks) >= offset + 4:
        try:
            sq_data["npc_scripts"].append({
                "npc_id": int(toks[offset]),
                "script_page": int(toks[offset + 1]),
                "mark_type": int(toks[offset + 2]),
                "dialog_type": int(toks[offset + 3]),
            })
        except ValueError:
            pass


def accumulate_rewards(executes: list[dict]) -> dict:
    rewards = {"exp": 0, "gold": 0, "items": []}
    for exe in executes:
        k = exe["kind"]
        p = exe["params"]
        if k == "take_exp" and p:
            try:
                rewards["exp"] += int(p[0])
            except (ValueError, IndexError):
                pass
        elif k == "take_sexp" and p:
            try:
                rewards["sp_exp"] = rewards.get("sp_exp", 0) + int(p[0])
            except (ValueError, IndexError):
                pass
        elif k == "take_money" and p:
            try:
                rewards["gold"] += int(p[0])
            except (ValueError, IndexError):
                pass
        elif k == "give_money" and p:
            try:
                rewards["gold"] += int(p[0])
            except (ValueError, IndexError):
                pass
        elif k in ("take_item", "give_quest_item") and len(p) >= 2:
            try:
                rewards["items"].append({"item_id": int(p[0]), "count": int(p[1])})
            except ValueError:
                pass
    return rewards


def build_output(quest_map: dict, strings: dict, npc_names: dict) -> dict:
    out = []
    for qid in sorted(quest_map.keys()):
        q = quest_map[qid]
        sq_list = sorted(q["subquests"], key=lambda x: x["id"])

        level_min = 0
        prerequisites = []
        conditions = []
        all_executes = []
        all_npcs = set()

        for sq in sq_list:
            for lim in sq["limits"]:
                if lim["type"] == "level":
                    level_min = lim.get("min", 0) or level_min or lim["min"]
                elif lim["type"] == "quest":
                    prerequisites.append(lim.get("quest_id", 0))
            for ns in sq["npc_scripts"]:
                all_npcs.add(ns["npc_id"])
            for cond in sq["conditions"]:
                if cond["event"] == "talk" and cond["params"]:
                    npc_val = cond["params"][0]
                    conditions.append({"type": "talk", "npc_id": npc_val})
                    all_npcs.add(npc_val)
                elif cond["event"] == "kill" and cond["params"]:
                    target_id = cond["params"][0]
                    count = 1
                    for exe in sq["executes"]:
                        if exe["kind"] == "add_count" and len(exe["params"]) >= 2:
                            try:
                                count = int(exe["params"][1])
                                break
                            except ValueError:
                                pass
                    conditions.append({"type": "kill", "target_id": target_id, "count": count})
            all_executes.extend(sq["executes"])

        npc_start = min(all_npcs) if all_npcs else 0
        npc_complete = max(all_npcs) if all_npcs else 0

        # De-duplicate conditions
        seen = set()
        unique_conds = []
        for c in conditions:
            key = json.dumps(c, sort_keys=True)
            if key not in seen:
                seen.add(key)
                unique_conds.append(c)

        rewards = accumulate_rewards(all_executes)

        # Strings for this quest
        string_keys = sorted([k for k in strings if k[0] == qid])
        title = ""
        description = ""
        dialog_start = ""
        dialog_progress = ""
        dialog_complete = ""

        if string_keys:
            first_key = string_keys[0]
            title = strings[first_key]["title"]
            description = strings[first_key]["description"]
            dialog_start = description

            if len(string_keys) >= 3:
                mid_key = string_keys[len(string_keys) // 2]
                dialog_progress = strings[mid_key]["description"]
                if not dialog_progress:
                    for sk in string_keys[1:-1]:
                        if strings[sk]["description"]:
                            dialog_progress = strings[sk]["description"]
                            break
            last_key = string_keys[-1]
            dialog_complete = strings[last_key]["description"] or strings[last_key]["title"]

        entry = {
            "id": qid,
            "name": title or f"Quest {qid}",
            "description": description,
            "level_required": level_min,
            "npc_start_id": npc_start,
            "npc_complete_id": npc_complete,
            "prerequisites": list(dict.fromkeys(prerequisites)),
            "conditions": unique_conds,
            "rewards": rewards,
            "dialog_start": dialog_start,
            "dialog_progress": dialog_progress,
            "dialog_complete": dialog_complete,
        }
        if npc_start in npc_names:
            entry["npc_start_name"] = npc_names[npc_start]
        if npc_complete in npc_names:
            entry["npc_complete_name"] = npc_names[npc_complete]
        out.append(entry)

    return {"quests": out}


def main():
    print("[parse_quests] Loading quest data...", flush=True)
    npc_names = load_npc_names()
    print(f"[parse_quests] Loaded {len(npc_names)} NPC names", flush=True)

    print("[parse_quests] Parsing QuestScript.bin.txt...", flush=True)
    sc = SCRIPT_FILE.read_bytes().decode("euc-kr", errors="replace")
    quest_map = parse_quest_script(sc)
    sq_count = sum(len(q["subquests"]) for q in quest_map.values())
    print(f"[parse_quests] Parsed {len(quest_map)} quests, {sq_count} subquests", flush=True)

    print("[parse_quests] Parsing QuestString.bin.txt...", flush=True)
    st = STRING_FILE.read_bytes().decode("euc-kr", errors="replace")
    strings = parse_strings(st)
    print(f"[parse_quests] Loaded {len(strings)} string entries", flush=True)

    print("[parse_quests] Building output...", flush=True)
    output = build_output(quest_map, strings, npc_names)
    print(f"[parse_quests] Built {len(output['quests'])} entries", flush=True)

    ASSETS.mkdir(parents=True, exist_ok=True)
    with open(OUTPUT, "w", encoding="utf-8") as f:
        json.dump(output, f, indent=2, ensure_ascii=False)
    print(f"[parse_quests] Written to {OUTPUT}", flush=True)


if __name__ == "__main__":
    main()
