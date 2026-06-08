#!/usr/bin/env python3
"""
Agent Q — FSM Lua Script Generator

Reads assets/data/quests_full.json and generates Lua state machine scripts
for each quest chain.

Output: assets/data/quest_fsms/quest_XXXX.lua

States:
  NOT_STARTED → IN_PROGRESS → COMPLETE → REWARDED

Triggers:
  kill(monster_id, count)
  collect(item_id, count)
  talk(npc_id)
  level(min_lvl)
"""

import json
import os
from pathlib import Path

BASE = Path(__file__).resolve().parents[2]
REBORN = BASE
ASSETS = REBORN / "assets" / "data"
INPUT = ASSETS / "quests_full.json"
FSM_DIR = ASSETS / "quest_fsms"


def sanitize_name(name: str) -> str:
    safe = "".join(c if c.isalnum() or c in (" ", "-", "_", "'") else " " for c in name)
    safe = safe.strip().replace(" ", "_").replace("'", "")
    if not safe:
        safe = "Unnamed"
    return safe[:40]


def generate_fsm(quest: dict) -> str:
    qid = quest["id"]
    name = quest["name"]

    triggers: dict[str, list[dict]] = {"kill": [], "collect": [], "talk": [], "level": []}
    for cond in quest.get("conditions", []):
        ctype = cond.get("type", "")
        if ctype == "kill":
            triggers["kill"].append({"target_id": cond.get("target_id", 0), "count": cond.get("count", 1)})
        elif ctype == "collect":
            triggers["collect"].append({"item_id": cond.get("item_id", 0), "count": cond.get("count", 1)})
        elif ctype == "talk":
            triggers["talk"].append({"npc_id": cond.get("npc_id", 0)})

    rewards = quest.get("rewards", {})
    prereqs = quest.get("prerequisites", [])
    npc_start = quest.get("npc_start_id", 0)
    npc_complete = quest.get("npc_complete_id", 0)
    lvl_req = quest.get("level_required", 0)

    def esc(s):
        return s.replace('"', '\\"')

    L = []
    L.append("--[[")
    L.append("  Quest %d: %s" % (qid, name))
    L.append("  Level Required: %d" % lvl_req)
    L.append("  NPC Start: %d, NPC Complete: %d" % (npc_start, npc_complete))
    if prereqs:
        L.append("  Prerequisites: %s" % str(prereqs))
    parts = []
    if rewards.get("exp"):
        parts.append("EXP=%s" % rewards["exp"])
    if rewards.get("gold"):
        parts.append("Gold=%s" % rewards["gold"])
    if parts:
        L.append("  Rewards: %s" % ", ".join(parts))
    L.append("]]")
    L.append("")

    L.append("local fsm = require('fsm_engine')")
    L.append("")
    L.append("local quest_%d = fsm:new({" % qid)
    L.append("    id = %d," % qid)
    L.append('    name = "%s",' % esc(name))
    L.append("    level_required = %d," % lvl_req)
    L.append("    npc_start = %d," % npc_start)
    L.append("    npc_complete = %d," % npc_complete)
    if prereqs:
        L.append("    prerequisites = {%s}," % ", ".join(str(p) for p in prereqs))
    else:
        L.append("    prerequisites = {},")
    L.append("")

    # --- NOT_STARTED ---
    L.append("    states = {")
    L.append("        {")
    L.append('            name = "NOT_STARTED",')
    L.append("            on_enter = function(self)")
    L.append('                self:log("Quest %d: Awaiting acceptance")' % qid)
    L.append("            end,")
    L.append("            transitions = {")
    if triggers["talk"]:
        t = triggers["talk"][0]
        L.append("                {")
        L.append('                    trigger = "npc_talk",')
        L.append("                    npc_id = %d," % t["npc_id"])
        L.append('                    target = "IN_PROGRESS",')
        L.append("                    action = function(self)")
        L.append('                        self:log("Quest %d: Accepted")' % qid)
        L.append("                    end")
        L.append("                },")
    else:
        L.append("                {")
        L.append('                    trigger = "accept",')
        L.append('                    target = "IN_PROGRESS",')
        L.append("                },")
    L.append("            },")
    L.append("        },")
    L.append("")

    # --- IN_PROGRESS ---
    L.append("        {")
    L.append('            name = "IN_PROGRESS",')
    L.append("            on_enter = function(self)")
    L.append('                self:log("Quest %d: In progress")' % qid)
    L.append("            end,")
    L.append("            transitions = {")

    for kt in triggers["kill"]:
        L.append("                {")
        L.append('                    trigger = "kill",')
        L.append("                    target_id = %d," % kt["target_id"])
        L.append("                    count = %d," % kt["count"])
        L.append('                    target = "COMPLETE",')
        L.append("                    action = function(self)")
        L.append('                        self:log("Quest %d: Kill objective met")' % qid)
        L.append("                    end")
        L.append("                },")

    for ct in triggers["collect"]:
        L.append("                {")
        L.append('                    trigger = "collect",')
        L.append("                    item_id = %d," % ct["item_id"])
        L.append("                    count = %d," % ct["count"])
        L.append('                    target = "COMPLETE",')
        L.append("                    action = function(self)")
        L.append('                        self:log("Quest %d: Collect objective met")' % qid)
        L.append("                    end")
        L.append("                },")

    for tt in triggers["talk"]:
        L.append("                {")
        L.append('                    trigger = "npc_talk",')
        L.append("                    npc_id = %d," % tt["npc_id"])
        L.append('                    target = "COMPLETE",')
        L.append("                    action = function(self)")
        L.append('                        self:log("Quest %d: NPC talk objective met")' % qid)
        L.append("                    end")
        L.append("                },")

    if not triggers["kill"] and not triggers["collect"] and len(triggers["talk"]) <= 1:
        L.append("                {")
        L.append('                    trigger = "any",')
        L.append('                    target = "COMPLETE",')
        L.append("                },")

    L.append("            },")
    L.append("        },")
    L.append("")

    # --- COMPLETE ---
    L.append("        {")
    L.append('            name = "COMPLETE",')
    L.append("            on_enter = function(self)")
    L.append('                self:log("Quest %d: All objectives done, turn in")' % qid)
    L.append("            end,")
    L.append("            transitions = {")
    L.append("                {")
    L.append('                    trigger = "npc_talk",')
    L.append("                    npc_id = %d," % npc_complete)
    L.append('                    target = "REWARDED",')
    L.append("                    action = function(self)")
    L.append('                        self:log("Quest %d: Completed!")' % qid)
    L.append("                    end")
    L.append("                },")
    L.append("            },")
    L.append("        },")
    L.append("")

    # --- REWARDED ---
    L.append("        {")
    L.append('            name = "REWARDED",')
    L.append("            on_enter = function(self)")
    L.append('                self:log("Quest %d: Rewards given")' % qid)
    L.append("                self:give_rewards({")
    if rewards.get("exp"):
        L.append("                    exp = %s," % rewards["exp"])
    if rewards.get("gold"):
        L.append("                    gold = %s," % rewards["gold"])
    if rewards.get("items"):
        L.append("                    items = {")
        for item in rewards["items"]:
            L.append("                        { item_id = %s, count = %s }," % (item["item_id"], item["count"]))
        L.append("                    },")
    if rewards.get("sp_exp"):
        L.append("                    sp_exp = %s," % rewards["sp_exp"])
    L.append("                })")
    L.append("            end,")
    L.append("            transitions = {},")
    L.append("        },")
    L.append("    },")
    L.append("})")
    L.append("")
    L.append("return quest_%d" % qid)

    return "\n".join(L)


def main():
    print("[generate_quest_fsm] Loading quest data...", flush=True)
    with open(INPUT, "r", encoding="utf-8") as f:
        data = json.load(f)

    quests = data["quests"]
    print(f"[generate_quest_fsm] Loaded {len(quests)} quests", flush=True)

    FSM_DIR.mkdir(parents=True, exist_ok=True)

    count = 0
    for quest in quests:
        qid = quest["id"]
        lua = generate_fsm(quest)
        filename = f"quest_{qid:04d}.lua"
        filepath = FSM_DIR / filename
        with open(filepath, "w", encoding="utf-8") as f:
            f.write(lua)
        count += 1

    print(f"[generate_quest_fsm] Generated {count} FSM Lua scripts in {FSM_DIR}", flush=True)


if __name__ == "__main__":
    main()
