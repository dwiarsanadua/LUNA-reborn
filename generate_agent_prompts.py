#!/usr/bin/env python3
"""
Prompt Builder: Generate N independent agent prompts from GAP analysis files.
Group items by file to ensure ZERO conflicts between agents.

Usage: python3 generate_agent_prompts.py
Output: ./agent_prompts/prompt_001.md ... prompt_050.md
"""

import csv, os, re, json
from collections import defaultdict, OrderedDict
from pathlib import Path

REBORN_ROOT = "/Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn"
GAP_FILE = "/Users/macbookair/PRIBADI/luna-plus-master/GAP_ANALYSIS.md"
SPEC_FILE = "/Users/macbookair/PRIBADI/luna-plus-master/ADAPTASI_SPEC.md"
PKT_FILE = "/Users/macbookair/PRIBADI/luna-plus-master/PACKET_MAPPING.md"
DB_FILE = "/Users/macbookair/PRIBADI/luna-plus-master/DB_QUERY_MAPPING.md"
OUTPUT_DIR = "/Users/macbookair/PRIBADI/luna-plus-master/agent_prompts"
MAX_AGENTS = 50

os.makedirs(OUTPUT_DIR, exist_ok=True)

# ─── Item definitions ─────────────────────────────────────────

ITEMS = [
    # (Layer, Status, Item, Reborn File, Old Reference, Severity, Effort)
    # Manually extracted from agent analysis output
    
    # ── PLAYER FLOW ──
    ("PLAYER FLOW", "🟡", "Launcher patch download", "PatchSystem.cpp", "MainDialog.cpp", "H", "3 days"),
    ("PLAYER FLOW", "🔴", "HTML notice view", "(new file)", "MainDialog.cpp", "L", "1 day"),
    ("PLAYER FLOW", "🟡", "Login ID/PW wiring", "client/ui/screens/LoginScreen.cpp", "WebLauncherIDPass.bin", "H", "5 days"),
    ("PLAYER FLOW", "🔴", "Save ID checkbox", "client/ui/screens/LoginScreen.cpp", "WebLauncherIDPass.bin", "M", "4 hrs"),
    ("PLAYER FLOW", "🔴", "Character delete", "client/ui/screens/CharSelectScreen.cpp", "CharSelect.bin", "M", "2 days"),
    ("PLAYER FLOW", "🟡", "3D char preview", "client/rendering/CharacterRenderer.cpp", "CharSelect.bin", "M", "3 days"),
    ("PLAYER FLOW", "🟡", "GameScreen hero spawn", "client/gameobjects/Hero.cpp", "GameIn.cpp", "H", "3 days"),
    ("PLAYER FLOW", "🟡", "Combat input system", "game/ecs/systems/CombatSystem.cpp", "GameIn.cpp", "H", "5 days"),
    
    # ── UI SYSTEM — each dialog is one file ──
    ("UI SYSTEM", "🔴", "SkillTrn dialog", "client/ui/dialogs/SkillTrnDialog.cpp", "SkillTrn.bin", "M", "2 days"),
    ("UI SYSTEM", "🔴", "PartySet dialog", "client/ui/dialogs/PartySetDialog.cpp", "PartySet.bin", "L", "1 day"),
    ("UI SYSTEM", "🟡", "GuildMark dialog", "client/ui/dialogs/GuildMarkDialog.cpp", "GuildMark.bin", "M", "2 days"),
    ("UI SYSTEM", "🟡", "GuildWarehouse dialog", "client/ui/dialogs/GuildWarehouseDialog.cpp", "GuildWarehouse.bin", "M", "2 days"),
    ("UI SYSTEM", "🟡", "GuildBank dialog", "client/ui/dialogs/GuildBankDialog.cpp", "Guildbank.bin", "M", "2 days"),
    ("UI SYSTEM", "🟡", "GuildWarInfo dialog", "client/ui/dialogs/GuildWarInfoDialog.cpp", "GuildWarInfo.bin", "L", "1 day"),
    ("UI SYSTEM", "🟡", "ChatRoom dialog wiring", "client/ui/dialogs/ChatRoomDialog.cpp", "ChatRoomCreateDlg.bin", "M", "1 day"),
    ("UI SYSTEM", "🔴", "ChatRoom full dialog", "client/ui/dialogs/ChatRoomDialog.cpp", "ChatRoomDlg.bin", "M", "2 days"),
    ("UI SYSTEM", "🟡", "NPCShop integration", "client/ui/dialogs/NPCDialog.cpp", "NPCShop.bin", "M", "2 days"),
    ("UI SYSTEM", "🟡", "Target UI placeholder", "client/ui/dialogs/TargetDialog.cpp", "Target.bin", "M", "2 days"),
    ("UI SYSTEM", "🔴", "TargetMonster dialog", "client/ui/dialogs/TargetMonsterDialog.cpp", "TargetMonster.bin", "M", "1 day"),
    ("UI SYSTEM", "🔴", "TargetCharacter dialog", "client/ui/dialogs/TargetCharacterDialog.cpp", "TargetCharacter.bin", "M", "1 day"),
    ("UI SYSTEM", "🟡", "Option sub-dialog", "client/ui/dialogs/OptionsDialog.cpp", "Optiondlg.bin", "M", "1 day"),
    ("UI SYSTEM", "🟡", "StreetStall buy", "client/ui/dialogs/StreetStallDialog.cpp", "StallBuy.bin", "M", "3 days"),
    ("UI SYSTEM", "🟡", "StreetStall sell", "client/ui/dialogs/StreetStallDialog.cpp", "StallSell.bin", "M", "3 days"),
    ("UI SYSTEM", "🔴", "StallOption dialog", "client/ui/dialogs/StallOptionDialog.cpp", "StallOption.bin", "L", "1 day"),
    ("UI SYSTEM", "🔴", "FishingPoint dialog", "client/ui/dialogs/FishingDialog.cpp", "FishingPointDlg.bin", "L", "1 day"),
    ("UI SYSTEM", "🔴", "Mix dialog", "client/ui/dialogs/MixDialog.cpp", "MixDialog.bin", "M", "2 days"),
    ("UI SYSTEM", "🔴", "Compose dialog", "client/ui/dialogs/ComposeDialog.cpp", "ComposeDialog.bin", "M", "2 days"),
    ("UI SYSTEM", "🔴", "ConsignmentCategory dialog", "client/ui/dialogs/ConsignmentDialog.cpp", "ConsignmentCategory.bin", "L", "1 day"),
    ("UI SYSTEM", "🔴", "Help dialog", "client/ui/dialogs/HelpDialog.cpp", "HelpDlg.bin", "M", "2 days"),
    ("UI SYSTEM", "🟡", "MainMenu HUD", "client/ui/dialogs/MainMenuDialog.cpp", "MainMenu.bin", "M", "2 days"),
    ("UI SYSTEM", "🔴", "GameOut exit dialog", "client/ui/dialogs/GameOutDialog.cpp", "GameOut.bin", "M", "1 day"),
    ("UI SYSTEM", "🔴", "Revival dialog", "client/ui/dialogs/RevivalDialog.cpp", "Revival.bin", "M", "1 day"),
    ("UI SYSTEM", "🔴", "FamilyInvite dialog", "client/ui/dialogs/FamilyDialog.cpp", "FamilyInvite.bin", "L", "1 day"),
    ("UI SYSTEM", "🔴", "FamilyMark dialog", "client/ui/dialogs/FamilyMarkDialog.cpp", "FamilyMark.bin", "L", "1 day"),
    ("UI SYSTEM", "🔴", "HousingWarehouse dialog", "client/ui/dialogs/HousingDialog.cpp", "HousingWarehouseDlg.bin", "M", "2 days"),
    ("UI SYSTEM", "🔴", "FarmUpgrade dialog", "client/ui/dialogs/FarmUpgradeDialog.cpp", "Farm_Upgrade.bin", "L", "1 day"),
    ("UI SYSTEM", "🔴", "FarmGet dialog", "client/ui/dialogs/FarmGetDialog.cpp", "Farm_Get.bin", "L", "1 day"),
    ("UI SYSTEM", "🔴", "DateMatching dialog", "client/ui/dialogs/DateMatchingDialog.cpp", "DateMatchingDlg.bin", "L", "2 days"),
    ("UI SYSTEM", "🔴", "Weather dialog", "client/ui/dialogs/WeatherDialog.cpp", "WeatherDlg.bin", "L", "1 day"),
    ("UI SYSTEM", "🟡", "NpcImage dialog", "client/ui/dialogs/NpcImageDialog.cpp", "NpcImage.bin", "L", "1 day"),
    ("UI SYSTEM", "🔴", "ChangeJob dialog", "client/ui/dialogs/ChangeJobDialog.cpp", "Changejob.bin", "L", "1 day"),
    
    # ── GAMEPLAY / COMBAT ──
    ("GAMEPLAY", "🟡", "CalcDamage formula refactor", "game/ecs/systems/CombatSystem.cpp", "CHero::CalcDamage()", "C", "5 days"),
    ("GAMEPLAY", "🟡", "Crit chance formula (Old: DEX/1000)", "game/ecs/systems/CombatSystem.cpp", "CHero::CalcDamage()", "M", "4 hrs"),
    ("GAMEPLAY", "🟡", "Block chance formula (Old: CON/2000)", "game/ecs/systems/CombatSystem.cpp", "CHero::CalcDamage()", "M", "4 hrs"),
    ("GAMEPLAY", "🟡", "Miss chance formula (Old: 1% base)", "game/ecs/systems/CombatSystem.cpp", "CHero::CalcDamage()", "L", "2 hrs"),
    ("GAMEPLAY", "🟡", "Skill damage modifier (add_type)", "game/ecs/systems/CombatSystem.cpp", "CHero::CalcDamage()", "H", "2 days"),
    ("GAMEPLAY", "🟡", "Aggro system (damage-based)", "game/ecs/systems/AISystem.cpp", "Hero.cpp", "H", "5 days"),
    ("GAMEPLAY", "🔴", "Hate/threat system", "game/ecs/systems/CombatSystem.cpp", "Hero.cpp", "H", "5 days"),
    ("GAMEPLAY", "🔴", "Aggro reset on target death", "game/ecs/systems/AISystem.cpp", "Hero.cpp", "M", "2 days"),
    ("GAMEPLAY", "🟡", "Buff/debuff stacking rules", "game/ecs/systems/BuffSystem.cpp", "Hero.cpp", "M", "3 days"),
    ("GAMEPLAY", "🔴", "KyungGong (dash) mode", "client/gameobjects/Hero.cpp", "Hero.cpp", "M", "2 days"),
    ("GAMEPLAY", "🟡", "PK penalty (bad fame)", "client/gameobjects/Hero.cpp", "Hero.cpp", "M", "2 days"),
    ("GAMEPLAY", "🔴", "PK protection timer", "client/gameobjects/Hero.cpp", "Hero.cpp", "L", "1 day"),
    ("GAMEPLAY", "🔴", "Combo system", "game/ecs/systems/ComboSystem.cpp", "Hero.cpp", "M", "3 days"),
    
    # ── NETWORK PROTOCOL ──
    ("NETWORK", "🟡", "MP_USERCONN: add missing fields (dwCRC, dwClientTime)", "game/network/protocol/Login.fbs", "MP_USERCONN (100+ packets)", "C", "2 wks"),
    ("NETWORK", "🟡", "MP_MOVE: add KyungGongIdx field", "game/network/protocol/Movement.fbs", "MP_MOVE (20+ packets)", "H", "1 week"),
    ("NETWORK", "🟡", "MP_COMBAT: add isBlocked field", "game/network/protocol/Combat.fbs", "MP_COMBAT (10+ packets)", "H", "1 week"),
    ("NETWORK", "🟡", "MP_ITEM: add byDstSlot, wSrcStorageType, wDstStorageType", "game/network/protocol/Inventory.fbs", "MP_ITEM (80+ packets)", "H", "2 wks"),
    ("NETWORK", "🟡", "MP_ITEM: add wDurability to InventorySlot", "game/network/protocol/Inventory.fbs", "InventorySlot", "H", "1 week"),
    ("NETWORK", "🟡", "MP_CHAT: wire Whisper/Party/Guild/Shout/Family types", "game/network/protocol/Chat.fbs", "MP_CHAT (30+ packets)", "M", "3 days"),
    ("NETWORK", "🟡", "MP_CHAT: add timestamp field", "game/network/protocol/Chat.fbs", "MP_CHAT.dwTime", "L", "1 hr"),
    ("NETWORK", "🟡", "MP_GUILD: add szGuildMark[256] emblem field", "game/network/protocol/Guild.fbs", "GuildInfo.szGuildMark[256]", "M", "2 days"),
    ("NETWORK", "🟡", "MP_GUILD: add dwGuildPoint contribution", "game/network/protocol/Guild.fbs", "GuildInfo.dwGuildPoint", "M", "1 day"),
    ("NETWORK", "🔴", "MP_HOUSE: new schema + handler", "game/network/protocol/Housing.fbs", "MP_HOUSE (20+ packets)", "M", "3 days"),
    ("NETWORK", "🔴", "MP_VEHICLE: new schema + handler", "game/network/protocol/Vehicle.fbs", "MP_VEHICLE (10+ packets)", "M", "2 days"),
    ("NETWORK", "🔴", "MP_NPC: add npc_action, MP_NPC_TALK_SYN, SHOP, QUEST, CHANGEMAP, RECALL", "game/network/protocol/NPC.fbs", "MP_NPC_TALK_SYN", "M", "2 days"),
    
    # ── ASSET PIPELINE ──
    ("ASSET", "🟡", ".eft effect parser wiring", "client/effects/EftParser.cpp", ".eft (effect)", "M", "3 days"),
    ("ASSET", "🟡", ".spr sprite → atlas wiring", "client/ui/UiAtlasRegistry.cpp", ".spr (sprite)", "M", "3 days"),
    ("ASSET", "🟡", ".bin UI script: parse remaining layouts", "client/ui/UiScriptParser.cpp", ".bin (UI script)", "H", "2 wks"),
    
    # ── SERVER SYSTEMS ──
    ("SERVER", "🟡", "AgentServer: char list handler", "server/agent/AgentServer.cpp", "AgentDBMsgParser", "H", "1 week"),
    ("SERVER", "🟡", "AgentServer: session management", "server/agent/SessionManager.cpp", "UserTable", "H", "3 days"),
    ("SERVER", "🔴", "AgentServer: gift manager", "server/agent/GiftManager.cpp", "GiftManager", "M", "3 days"),
    ("SERVER", "🔴", "AgentServer: punish manager", "server/agent/PunishManager.cpp", "PunishManager", "M", "2 days"),
    ("SERVER", "🟡", "MapServer: combat system", "server/map/systems/CombatSystem.cpp", "Hero.cpp server logic", "H", "5 days"),
    ("SERVER", "🟡", "MapServer: item management", "server/map/systems/ItemSystem.cpp", "ItemManager", "H", "1 week"),
    ("SERVER", "🟡", "MapServer: NPC/AI system", "server/map/systems/AISystem.cpp", "(NPC manager)", "M", "1 week"),
    ("SERVER", "🟡", "MapServer: monster spawn", "server/map/systems/SpawnSystem.cpp", "(MonsterManager)", "M", "5 days"),
    ("SERVER", "🟡", "MapServer: quest manager", "server/map/systems/QuestSystem.cpp", "(QuestManager)", "M", "5 days"),
    ("SERVER", "🟡", "MapServer: party manager", "server/map/systems/PartySystem.cpp", "(PartyManager)", "M", "3 days"),
    ("SERVER", "🟡", "MapServer: guild manager", "server/map/systems/GuildSystem.cpp", "(GuildManager)", "M", "1 week"),
    ("SERVER", "🟡", "DistributeServer: channel management", "server/distribute/ChannelManager.cpp", "(ChannelManager)", "M", "3 days"),
    
    # ── DATABASE ──
    ("DATABASE", "🔴", "TB_CHARACTER: add battle_style column", "database/schema_game_sqlite.sql", "TB_CHARACTER.battle_style", "L", "2 hrs"),
    ("DATABASE", "🟡", "USP_ITEM_LOAD: storage_type → StorageType", "database/schema_game_sqlite.sql", "TB_ITEM.storage_type", "M", "2 hrs"),
    ("DATABASE", "🟡", "USP_GUILD_LOAD: add emblem_data", "database/schema_game_sqlite.sql", "TB_GUILD.emblem_data", "M", "1 day"),
    ("DATABASE", "🟡", "USP_FRIEND_LIST: memo → Memo", "database/schema_game_sqlite.sql", "TB_FRIEND.memo", "L", "2 hrs"),
    ("DATABASE", "🔴", "~30 remaining SP not mapped", "database/schema_game_sqlite.sql", "Various USP_*", "H", "3 weeks"),
    
    # ── ERROR HANDLING ──
    ("ERROR", "🔴", "Network reconnect dialog", "client/network/NetworkClient.cpp", "Reconnect dialog 10s", "H", "3 days"),
    ("ERROR", "🟡", "Asset load fallback texture", "engine/gx_render/Texture.cpp", "Fallback texture + log", "M", "1 day"),
    ("ERROR", "🔴", "Character load failure dialog", "client/gameobjects/PersistenceManager.cpp", "Retry + error dialog", "M", "2 days"),
    ("ERROR", "🟡", "Map change failure rollback", "client/engine/EngineMap.cpp", "Rollback to previous map", "M", "2 days"),
    ("ERROR", "🔴", "File not found fallback asset", "engine/gx_render/VFS.cpp", "Fallback asset + dialog", "M", "3 days"),
    ("ERROR", "🟡", "Login failure NACK codes", "server/agent/AgentServer.cpp", "Detailed NACK codes", "M", "1 day"),
    
    # ── SECURITY ──
    ("SECURITY", "🟡", "Authentication: password hashing (bcrypt)", "server/agent/AgentServer.cpp", "AES-GCM token", "C", "3 days"),
    ("SECURITY", "🔴", "Rate limiting", "server/agent/AgentServer.cpp", "(basic)", "H", "1 week"),
    ("SECURITY", "🔴", "Brute-force protection", "server/agent/AgentServer.cpp", "(basic lockout)", "M", "2 days"),
    
    # ── CONCURRENCY ──
    ("CONCURRENCY", "🔴", "Job system (asio post)", "engine/network/NetworkLayer.cpp", "(custom thread pool)", "M", "2 wks"),
    
    # ── LOCALIZATION ──
    ("LOCALIZATION", "🟡", "String table full coverage", "client/ui/UiStringTable.cpp", "InterfaceMsg.bin", "H", "1 week"),
    ("LOCALIZATION", "🔴", "CJK font support", "client/rendering/UIRenderer.cpp", "Per-language fonts", "H", "3 days"),
    ("LOCALIZATION", "🟡", "Language switching wire", "config/Localization.cpp", "Runtime language change", "M", "3 days"),
    
    # ── AUDIO ──
    ("AUDIO", "🟡", "BGM crossfade implementation", "client/audio/AudioManager.cpp", "(basic crossfade)", "M", "2 days"),
    ("AUDIO", "🔴", "3D positional audio", "client/audio/AudioManager.cpp", "(Miles 3D)", "M", "3 days"),
    ("AUDIO", "🟡", "SFX list full loading", "client/ui/UiSoundIndex.cpp", "SoundList.bin.txt", "M", "2 days"),

    # ── PHYSICS ──
    ("PHYSICS", "🔴", "CollisionTestMovingSphereMeetTriangle", "engine/physics/PhysicsWorld.cpp", "(proprietary)", "M", "1 week"),
    ("PHYSICS", "🔴", "AAEllipsoidCollisionTest", "engine/physics/PhysicsWorld.cpp", "(proprietary)", "M", "1 week"),
    ("PHYSICS", "🟡", "MAP->CollisionLine implementation", "client/engine/EngineMap.cpp", "MAP->CollisionLine()", "H", "5 days"),
]


# ─── Group items by Reborn file ───────────────────────────────

file_groups = defaultdict(list)
for item in ITEMS:
    filepath = item[3]  # Reborn File
    file_groups[filepath].append(item)

# Sort: biggest files first (most items)
sorted_files = sorted(file_groups.items(), key=lambda x: len(x[1]), reverse=True)

# ─── Distribute across agents ────────────────────────────────

# Each agent gets files that are DISJOINT (no file shared between agents)
agents = defaultdict(list)  # agent_id -> list of (file, items)
agent_files = defaultdict(set)  # agent_id -> set of files

agent_id = 0
for filepath, items in sorted_files:
    # Assign to agent with fewest items
    agents[agent_id].append((filepath, items))
    agent_id = (agent_id + 1) % MAX_AGENTS

# ─── Generate prompts ────────────────────────────────────────

def generate_prompt(agent_num, assignments):
    lines = []
    lines.append(f"# Agent #{agent_num+1:03d} — Tugas Adaptasi Luna Old → Reborn\n")
    lines.append(f"Lokasi: {REBORN_ROOT}\n")
    lines.append("Build: cmake --build build/macos-debug -j$(sysctl -n hw.ncpu)\n")
    lines.append("---\n")
    lines.append("## File yang harus diubah\n")
    lines.append("")
    
    NEW_FILE_PREFIXES = [
        "client/ui/dialogs/", "game/network/protocol/Housing",
        "game/network/protocol/NPC", "game/network/protocol/Vehicle",
        "game/ecs/systems/ComboSystem", "server/agent/Gift",
        "server/agent/Punish", "server/agent/Session",
        "server/distribute/Channel",
    ]
    
    creates_header = False
    for filepath, items in assignments:
        is_new = False
        for prefix in NEW_FILE_PREFIXES:
            if filepath.startswith(prefix):
                is_new = True
                break
        action = "BARU" if is_new or "(new file)" in filepath or "new" in filepath.lower() else "UPDATE"
        
        if filepath.endswith('.h') or filepath.endswith('.hpp') or filepath.endswith('.fbs'):
            creates_header = True
        
        lines.append(f"### {filepath} [{action}]")
        lines.append("")
        
        for item in items:
            status = item[1]
            sev = item[5]
            effort = item[6]
            ref = item[4]
            desc = item[2]
            
            lines.append(f"- **{status} {desc}** (Severity: {sev}, Effort: {effort})")
            lines.append(f"  - Referensi Old: `{ref}`")
            
            # Add extra instruction from GAP/SPEC
            if "CombatSystem.cpp" in filepath and "CalcDamage" in desc:
                lines.append(f"  - Baca file Old `CHero::CalcDamage()` di Luna-Plus-Old, update formula constants di Reborn")
            elif ".fbs" in filepath:
                lines.append(f"  - Baca schema `.fbs` dan update field-by-field sesuai mapping di PACKET_MAPPING.md")
            elif ".sql" in filepath:
                lines.append(f"  - Baca perbandingan kolom di DB_QUERY_MAPPING.md")
            
            lines.append("")
    
    if creates_header:
        lines.append("**📌 CATATAN: Tugas ini membuat file HEADER baru (.h/.hpp/.fbs).**")
        lines.append("Agent lain mungkin menunggu file ini selesai. Prioritaskan pengerjaan.\n")
    
    lines.append("---\n")
    lines.append("## Aturan Keamanan Eksekusi Paralel\n")
    lines.append("1. BACA file Reborn yang tercantum sebelum mengubah\n")
    lines.append("2. BACA file Old reference untuk memahami behavior asli\n")
    lines.append("3. JANGAN ubah file di luar daftar ini\n")
    lines.append("4. KODE BARU harus mengikuti style yang sudah ada (PascalCase class, snake_case function)\n")
    lines.append("5. **🚫 JANGAN build atau compile**. Cukup tulis kode. Build akan dilakukan 1x di akhir oleh koordinator.\n")
    lines.append("6. **🚫 JANGAN commit atau push**. Cukup tulis kode.\n")
    lines.append("7. **⚠️ Jika tugas Anda membuat file .h BARU (header)**: beri tahu koordinator karena agent lain mungkin depend pada header Anda.\n")
    lines.append("8. **✅ Jika tugas selesai**: cukup return pesan \"Agent #{agent_num+1:03d} done: files modified\".\n")
    lines.append("")
    
    return "\n".join(lines)

# Write prompts
total_items = 0
for agent_num in range(MAX_AGENTS):
    if agent_num not in agents or not agents[agent_num]:
        # Write empty placeholder
        with open(f"{OUTPUT_DIR}/prompt_{agent_num+1:03d}.md", "w") as f:
            f.write(f"# Agent #{agent_num+1:03d} — No tasks assigned\n")
        continue
    
    prompt = generate_prompt(agent_num, agents[agent_num])
    with open(f"{OUTPUT_DIR}/prompt_{agent_num+1:03d}.md", "w") as f:
        f.write(prompt)
    
    item_count = sum(len(items) for _, items in agents[agent_num])
    total_items += item_count
    print(f"Agent {agent_num+1:03d}: {len(agents[agent_num])} files, {item_count} items")

print(f"\n=== SUMMARY ===")
print(f"Total agents: {len([a for a in agents if agents[a]])}")
print(f"Total items: {total_items}")
print(f"Output directory: {OUTPUT_DIR}")
print(f"Build: cmake --build build/macos-debug -j$(sysctl -n hw.ncpu)")
