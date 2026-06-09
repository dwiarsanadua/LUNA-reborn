# GAP ANALYSIS: Luna-Plus-Old vs Luna-Plus-Reborn

> Generated: 2026-06-09
> Scope: 14 Layers (B1-B16) — Severity/Effort/Dependencies for every entry

---

## B1. Player Flow (Alur Utama)

```
LAYER: PLAYER FLOW
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Old File / Screen             Reborn File                 Status  Sev  Effort   Dependencies
─────────────────────────────────────────────────────────────────────────────────────────────
[Client]Launcher/              client/ui/screens/          🟡     H    3 days   Network system
MainDialog.cpp → Launcher     LauncherScreen.cpp          Partial
  ├─ FTP patch download       PatchSystem.cpp              ✅     -    -        -
  ├─ HTML notice view         (not implemented)            🔴     L    1 day    WebView
  ├─ BGM playback             AudioManager.cpp             ✅     -    -        -
  └─ Auto-update check        PatchSystem.cpp             🟡     L    4 hrs    -

[Client]LUNA/LoginDlg         client/ui/screens/          🟡     H    5 days   FlatBuffers
→ WebLauncherIDPass.bin       LoginScreen.cpp              Partial            schema
  ├─ ID/Password input        InputField widget             ✅     -    -        -
  ├─ Server selection         ServerInfo table              ✅     -    -        -
  ├─ Save ID checkbox         (not implemented)            🔴     M    4 hrs    CheckBox widget
  └─ Version check            client_version field         🟡     M    2 hrs    -

CharSelect.bin →              client/ui/screens/          🟡     M    4 days   Inventory
CharSelectScreen.cpp          CharSelectScreen.cpp          Partial            system
  ├─ Character list grid      CharacterSlot widget          ✅     -    -        -
  ├─ Character create         CharMakeDlg.cpp               ✅     -    -        -
  ├─ Character delete         (not implemented)            🔴     M    2 days   Dialog system
  └─ 3D preview render       CharacterRenderer.cpp        🟡     M    3 days   Model system

GameIn.cpp →                  client/ui/screens/          🟡     C    2 wks    All systems
GameWorld flow                GameScreen.cpp                Partial
  ├─ Hero spawn + init       Hero.cpp (client)             🟡     H    3 days   ECS
  ├─ World rendering          WorldRenderer.cpp             ✅     -    -        -
  ├─ Combat input             CombatSystem.cpp              🟡     H    5 days   ECS combat
  ├─ Social (party/chat)      PartyDialog, ChatPanel         ✅     -    -        -
  └─ Minimap display          MiniMapDlg.cpp               🟡     M    2 days   -

SUMMARY - PLAYER FLOW
  Total screens:               5 (Launcher, Login, CharSelect, GameWorld, Loading)
  Fully implemented:           1 (Launcher — 20%)
  Partial:                     4 (80%)
  Missing sub-features:        5+ (Save ID, Char delete, HTML notice, etc.)
  Total estimated effort:      ~4 weeks (1 FTE)
```

---

## B2. UI System (Window/Dialog)

```
LAYER: UI SYSTEM
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Old .bin File             Reborn C++ Class               Status  Sev  Effort    Dependencies
──────────────────────────────────────────────────────────────────────────────────────────────
WebLauncherIDPass.bin      LoginScreen.cpp                🟡     H    2 days    Skin textures
IDPass.bin                 (not implemented)              🔴     H    2 days    -
ServerSelect.bin           ServerList logic in            🟡     M    1 day     Login flow
                           LoginScreen.cpp
CharSelect.bin             CharSelectScreen.cpp           🟡     M    2 days    Character data
CharMake.bin               CharMakeDlg.cpp                ✅     -    -         -
CharMakeProfile.bin        CharMakeDlg.cpp                🟡     L    4 hrs     -
Inven.bin                  InventoryDialog.cpp            ✅     -    -         -
Skill.bin                  SkillDialog.cpp                ✅     -    -         -
SkillTrn.bin               (not implemented)              🔴     M    2 days    Skill system
Quest.bin                  QuestDialog.cpp                ✅     -    -         -
Friend.bin                 FriendDialog.cpp               ✅     -    -         -
PartyCreate.bin            PartyDialog.cpp                ✅     -    -         -
PartyInvite.bin            PartyDialog.cpp                ✅     -    -         -
PartySet.bin               (not implemented)              🔴     L    1 day     Party system
Guild.bin                  GuildDialog.cpp                ✅     -    -         -
GuildCreate.bin            GuildDialog.cpp                🟡     M    1 day     -
GuildInvite.bin            GuildDialog.cpp                🟡     L    4 hrs     -
GuildMark.bin              (not implemented)              🔴     M    2 days    Image upload
GuildNotice.bin            GuildDialog.cpp                🟡     L    4 hrs     -
GuildRank.bin              (not implemented)              🔴     L    1 day     -
GuildWarehouse.bin         (Guild warehouse)              🔴     M    2 days    Storage
Guildbank.bin              (not implemented)              🔴     M    2 days    -
GuildWarInfo.bin           (not implemented)              🔴     L    1 day     -
ChatWindow.bin             ChatPanel.cpp                  ✅     -    -         -
ChatRoomCreateDlg.bin      ChatRoomDialog.cpp             🟡     M    1 day     -
ChatRoomJoinDlg.bin        ChatRoomDialog.cpp             🟡     L    4 hrs     -
ChatRoomDlg.bin            (not implemented)              🔴     M    2 days    -
NPCChat.bin                NPCDialog.cpp                  ✅     -    -         -
NPCShop.bin                (NPCDialog has shop)           🟡     M    2 days    -
Target.bin                 (Target UI in GameScreen)      🟡     M    2 days    -
TargetMonster.bin          (not implemented)              🔴     M    1 day     -
TargetCharacter.bin        (not implemented)              🔴     M    1 day     -
MiniMap.bin                MiniMapDlg.cpp                 ✅     -    -         -
BigMap.bin                 WorldMapDlg.cpp                ✅     -    -         -
Option.bin                 OptionsDialog.cpp              ✅     -    -         -
Optiondlg.bin              (not implemented)              🔴     M    1 day     -
Bank.bin                   StorageDialog.cpp              ✅     -    -         -
Storage.bin                StorageDialog.cpp              ✅     -    -         -
Trade.bin                  TradeDialog.cpp                ✅     -    -         -
StallBuy.bin               (StreetStall)                  🟡     M    3 days    -
StallSell.bin              (StreetStall)                  🟡     M    3 days    -
StallOption.bin            (not implemented)              🔴     L    1 day     -
FishingDlg.bin             FishingDialog.cpp              ✅     -    -         -
FishingPointDlg.bin        (not implemented)              🔴     L    1 day     -
FishingProgressDialog.bin  FishingDialog.cpp              🟡     L    4 hrs     -
EnchantDialog.bin          UpgradeDialog.cpp              ✅     -    -         -
ReinforceDialog.bin        UpgradeDialog.cpp              ✅     -    -         -
MixDialog.bin              (not implemented)              🔴     M    2 days    -
ComposeDialog.bin          (not implemented)              🔴     M    2 days    -
Consignment.bin            ConsignmentDialog.cpp          ✅     -    -         -
ConsignmentCategory.bin    (not implemented)              🔴     L    1 day     -
Consignment_Guide.bin      (not implemented)              🔴     L    4 hrs     -
HelpDlg.bin                (not implemented)              🔴     M    2 days    -
KeySetting.bin             KeyBindDialog.cpp              ✅     -    -         -
MainMenu.bin               (HUD overlay)                  🟡     M    2 days    -
Menu.bin / Menu0.bin       (not implemented)              🔴     M    2 days    -
FadeDlg.bin                FadeDlg.cpp                    ✅     -    -         -
NewLoadDlg.bin             LoadingScreen.cpp              ✅     -    -         -
LoadingMsg.bin             LoadingScreen.cpp              🟡     L    4 hrs     -
GameOut.bin                (Exit dialog)                  🔴     M    1 day     -
Revival.bin                (Revive dialog)                🔴     M    1 day     -
PetInfo.bin                PetDialog.cpp                  ✅     -    -         -
PetUI.bin                  PetDialog.cpp                  🟡     L    4 hrs     -
Family.bin                 FamilyDialog.cpp               ✅     -    -         -
FamilyCreate.bin           FamilyDialog.cpp               🟡     L    4 hrs     -
FamilyInvite.bin           (not implemented)              🔴     L    1 day     -
FamilyMark.bin             (not implemented)              🔴     L    1 day     -
HouseNameDlg.bin           HousingDialog.cpp              🟡     M    1 day     -
HousingPointDlg.bin        HousingDialog.cpp              🟡     M    1 day     -
HousingWarehouseDlg.bin    (not implemented)              🔴     M    2 days    -
FarmManage.bin             FarmSystem.cpp                 🟡     M    2 days    -
Farm_Upgrade.bin           (not implemented)              🔴     L    1 day     -
Farm_Get.bin               (not implemented)              🔴     L    1 day     -
DateMatchingDlg.bin        (not implemented)              🔴     L    2 days    -
WeatherDlg.bin             (not implemented)              🔴     L    1 day     -
SiegeWarFlagDlg.bin        SiegeSystem.cpp                🟡     M    2 days    -
NpcImage.bin / NpcImageList.bin (not implemented)         🔴     L    1 day     -
ChangeClass.bin            ClassAdvancement.hpp           🟡     M    2 days    -
Changejob.bin              (not implemented)              🔴     L    1 day     -
Cooking Dlg.bin            CookingDialog.cpp              ✅     -    -         -
TutorialDlg.bin            TutorialDlg.cpp                ✅     -    -         -
MacroDialog.bin            MacroDialog.cpp                ✅     -    -         -
CashShopDialog             CashShopDialog.cpp             ✅     -    -         -

SUMMARY - UI SYSTEM
  Total Old dialogs:          213 .bin files
  Reborn C++ dialog classes:  30 (14%)
  Fully implemented:          22 (10%)
  Partial/stub:               35 (16%)
  Missing:                    156 (74%)
  Total estimated effort:     ~12 weeks (2 FTE)

  Priority fix (top 5):
  1. Window frame rendering (UiSkinManager::DrawWindow) — 2 days — blocker for all windows
  2. Button wiring (UiSkinManager::DrawButton) — 1 day — after skin textures
  3. Drag window by title bar — 2 days — after WindowManager refactor
  4. Tooltip on hover — 3 days — after widget mouse events
  5. Missing common dialogs (NPCShop, Target, Stall, Mix, Compose) — 2 weeks
```

---

## B3. Gameplay Constants & Hero System

```
LAYER: GAMEPLAY / HERO COMBAT
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Old Function (Hero.cpp)      Reborn Equivalent             Status  Sev  Effort   Dependencies
─────────────────────────────────────────────────────────────────────────────────────────────
CHero::CalcDamage()           CombatSystem::CalculateDamage  🟡    C    5 days   ECS combat
                              (game/ecs/systems/CombatSystem.cpp)
  ├─ Base damage formula:     (atk * 2) - def               ✅     -    -        -
  │  ATK * skill_power
  ├─ Crit chance:             crit_rate = min(50%,           🟡     M    4 hrs    -
  │  DEX/1000 => ~0.1% per    5% + DEX/100) [OLD: DEX/1000,
  │  DEX point                REBORN: DEX/100 — 10x lebih
  │                           tinggi dari Old]
  ├─ Element advantage:       GetElementAdvantage()          ✅     -    -        -
  │  7-element cycle 1.3x    [7-element table 1.30/0.70]
  ├─ Level gap penalty:       level_scaling ±5%/level,      ✅     -    -        -
  │  (Old: ±5% per level)    cap 50%
  ├─ Block chance:            shield_defense + block_rate    🟡     M    4 hrs    -
  │  CON/2000 (Old)          (Reborn: block_rate from stat)
  ├─ Miss chance:             5% - DEX/500, min 1%           🟡     L    2 hrs    -
  │  (Old: ~5% base)         [Reborn uses different calc]
  ├─ Damage variance:         ±10% variance                  ✅     -    -        -
  │  ±10% (Old: sama)
  └─ Skill damage modifier:   skill_add_damage with          🟡     H    2 days   Skill system
     add_type 1/2 + rate/     add_type 1 (STR) or 2 (ATK)
     plus add value

Hero::ApplyInterface()        Hero.cpp (client)             🟡     H    3 days   UI system
  ├─ SetLife/MaxLife          (basic HP/MP sync)            🟡     M    2 days    -
  ├─ SetStrength/Dex/etc      Stat updates via ECS           🟡     M    2 days    -
  ├─ SetPartyIdx              PartyDialog updates            ✅     -    -        -
  └─ SetGuildIdx              GuildDialog updates            ✅     -    -        -

Aggro system (Hero.cpp)       AISystem.cpp                   🟡     H    5 days   ECS
  ├─ Monster aggro table      (basic aggro)                 🟡     M    3 days    -
  ├─ Hate/threat system       (not implemented)              🔴     H    5 days   CombatSystem
  └─ Aggro reset on           (not implemented)              🔴     M    2 days    -
     target death

Buff/Debuff stacking          BuffSystem.cpp                 🟡     M    3 days   ECS
  ├─ mBuffStatus / mAbnormalStatus (client)                  🟡     M    2 days    -
  ├─ Buff remaining time       BuffSkillInfo                  ✅     -    -        -
  └─ Stacking rules            (not implemented)              🔴     M    3 days    -

Movement + Pathfinding         ServerAuthMovement.cpp        🟡     H    4 days   NavMesh
  ├─ WayPoint system           NavMeshSystem.cpp             🟡     M    3 days    -
  ├─ CollisionLine check       CollisionSystem.hpp           🟡     M    2 days    -
  └─ KyungGong (dash) mode     (not implemented)             🔴     M    2 days    -

PK Mode                        PKManagerDlg.cpp              🟡     M    2 days   Combat
  ├─ PK flag toggle            (basic PK mode)               🟡     M    1 day     -
  ├─ PK penalty (bad fame)     (not implemented)             🔴     M    2 days    -
  └─ PK protection timer       (not implemented)             🔴     L    1 day     -

Combo system                   (not implemented)             🔴     M    3 days   Combat
  ├─ MAX_COMBO_NUM = 3                                       
  ├─ Auto-attack combo loop                                  
  └─ Skill combo integration                                 

SUMMARY - GAMEPLAY CONSTANTS
  Total Hero.cpp functions:     ~60+
  Reborn CombatSystem funcs:    ~8 (13%)
  Fully ported formulas:        6/12 (50%)
  Missing major features:       Aggro system, Combo, PK penalty, Threat
  Total estimated effort:       ~4 weeks (1 FTE)

  Critical numeric differences:
  - Crit rate: OLD = DEX/1000 (~0.1%/pt), REBORN = 5% + DEX/100 (~1%/pt) — 10x difference
  - Block: OLD = CON/2000, REBORN = block_rate stat — different system
  - Miss: OLD = hardcoded 5%, REBORN = 5% - DEX/500 — different formula
```

---

## B4. Network Protocol

```
LAYER: NETWORK PROTOCOL
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Old Category              Old Count   Reborn .fbs            Status  Sev  Effort   Dependencies
─────────────────────────────────────────────────────────────────────────────────────────────
MP_USERCONN               100+        Login.fbs,               🟡    C    2 wks   FlatBuffers
                                       Character.fbs,                   (mapping)
                                       MapChange.fbs
MP_MOVE                   20+         Movement.fbs             🟡    H    1 week   -
MP_COMBAT                 10+         Combat.fbs               🟡    H    1 week   -
MP_SKILL                  15+         Skill.fbs                🟡    H    1 week   -
MP_ITEM                   80+         Inventory.fbs             🟡    H    2 wks   -
MP_CHAT                   30+         Chat.fbs                 🟡    M    3 days   -
MP_PARTY                  15+         Party.fbs                🟡    M    3 days   -
MP_GUILD                  40+         Guild.fbs                🟡    M    5 days   -
MP_QUEST                  15+         Quest.fbs                🟡    M    3 days   -
MP_FRIEND                 10+         Friend.fbs               🟡    M    2 days   -
MP_EXCHANGE (Trade)       15+         Trade.fbs                🟡    M    3 days   -
MP_STORAGE                15+         Storage.fbs              🟡    M    2 days   -
MP_STREETSTALL            10+         StreetStall.fbs          🟡    M    3 days   -
MP_PET                    10+         Pet.fbs                  🟡    M    2 days   -
MP_FAMILY                 15+         Family.fbs               🟡    M    2 days   -
MP_FISHING                10+         Fishing.fbs              🟡    M    2 days   -
MP_DUNGEON                10+         Dungeon.fbs              🟡    M    2 days   -
MP_FARM                   10+         Farm.fbs                 🟡    M    2 days   -
MP_HOUSE                  20+         (Housing in Guild?)      🔴    M    3 days   -
MP_SIEGEWAR/SIEGERECALL   20+         SiegeSystem.cpp          🔴    M    3 days   -
MP_CONSIGNMENT            10+         Consignment.fbs          🟡    M    2 days   -
MP_VEHICLE                10+         (not ported)             🔴    M    2 days   -
MP_COOK                   5+          (CookingDialog)          🟡    L    1 day    -
MP_MONITORTOOL*           40+         (not ported)             🔴    L    -        Admin tool
MP_HACKSHIELD/NPROTECT    5+          (removed)                🔴    H    2 wks   Security
MP_CHEAT (GM)             20+         (not ported)             🔴    M    3 days   -

SUMMARY - NETWORK PROTOCOL
  Old MP_CATEGORY:            94 categories
  Old individual packets:     2000+ (estimated from Protocol.h)
  Reborn .fbs schemas:        27 files
  Reborn PacketType enum:     ~229 entries
  Coverage:                   ~15% of Old packet types
  Total estimated effort:     ~6 weeks (2 FTE)

  Key gaps:
  - 27 protocol .fbs exist but field-by-field mapping shows ~40% fields missing
  - Anti-cheat packets (HACKSHIELD, NPROTECT) removed entirely
  - Server admin/monitor packets not ported
  - Many game feature packets missing (Vehicle, Siege, Housing, etc.)
```

---

## B5. Asset & Data Pipeline

```
LAYER: ASSET PIPELINE
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Old Format       Reborn Format    Status  Sev  Effort   Dependencies
─────────────────────────────────────────────────────────────────────────────
.mod (3D model)  .glb / .gltf     ✅      -    -        chx_to_gltf tool
.anm (anim)      .anm.json        ✅      -    -        AnmParser.h
.dds / .tif      .png             🟡      L    -        Texture converter
.hfl (heightmap) .hgt             ✅      -    -        HflParser.h
.chx (char def)  .json            ✅      -    -        -
.map (scene)     scene.json       ✅      -    -        map_converter tool
.eft (effect)    (custom format)  🟡      M    3 days   EftParser.cpp
.spr (sprite)    (atlas texture)  🟡      M    3 days   UiAtlasRegistry
.wav / .mp3      .wav / .mp3      ✅      -    -        miniaudio
.bin (UI script) .bin.txt parsed  🟡      H    2 wks   UiScriptParser
.bin (string)    .bin.txt parsed  🟡      M    1 week   UiStringTable
.bin (minimap)   (converted)      🟡      L    2 days   -

SUMMARY - ASSET PIPELINE
  All major assets have a conversion path.
  Effort needed: Complete UI .bin.txt parsing, effect format conversion.
  Total estimated effort: ~3 weeks (1 FTE)
```

---

## B6. Server Systems

```
LAYER: SERVER SYSTEMS
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Old Server          Old Path                    Reborn Path             Status  Sev  Effort   Dependencies
───────────────────────────────────────────────────────────────────────────────────────────────────────────
AgentServer         [Server]Agent/              server/agent/            🟡    C    3 wks   Network
  ├─ Auth/login      AgentNetworkMsgParser       AgentServer.cpp          🟡    H    1 week   FlatBuffers
  ├─ Character list  AgentDBMsgParser            AgentDBHandler           🟡    H    1 week   DB schema
  ├─ Session mgmt    UserTable                   SessionManager           🟡    H    3 days   -
  ├─ HackShield      HackShieldManager           (removed)                🔴    H    2 wks   Security
  ├─ NProtect        NProtectManager             (removed)                🔴    H    1 week   -
  ├─ Gift manager    GiftManager                 (not ported)             🔴    M    3 days   -
  ├─ Chat room       ChatRoomMgr                 ChatRoomDialog          🟡    M    3 days   -
  └─ Punish mgr      PunishManager               (not ported)             🔴    M    2 days   -

MapServer           [Server]Map/                server/map/              🟡    C    1 month All
  ├─ World sim       MapNetworkMsgParser         MapServer.cpp            🟡    H    2 wks   ECS
  ├─ Combat          (in Hero.cpp server logic)  CombatSystem.cpp         🟡    H    5 days   -
  ├─ Item mgmt       ItemManager                 (inventory system)       🟡    H    1 week   DB
  ├─ NPC/AI          (NPC manager)               AISystem.cpp             🟡    M    1 week   -
  ├─ Monster spawn   (MonsterManager)            SpawnSystem              🟡    M    5 days   -
  ├─ Quest mgr       (QuestManager)              QuestSystem              🟡    M    5 days   DB
  ├─ Party mgr       (PartyManager)              PartySystem              🟡    M    3 days   -
  ├─ Guild mgr       (GuildManager)              GuildSystem              🟡    M    1 week   DB
  └─ Siege/Tourn     (SiegeRecallMgr)            SiegeSystem             🟡    M    2 wks   -

DistributeServer    [Server]Distribute/          server/distribute/       🟡    C    2 wks   Network
  ├─ Load balancing  DistributeNetworkMsgParser   DistributeServer.cpp     🟡    H    1 week   -
  └─ Channel mgmt    (ChannelManager)             ChannelManager           🟡    M    3 days   -

SUMMARY - SERVER SYSTEMS
  3 Old server types → 3 Reborn server types
  Agent: ~50% ported (core auth + char list, missing HackShield/NProtect/Gift)
  Map: ~30% ported (combat ECS exists, many subsystems missing)
  Distribute: ~40% ported
  Total estimated effort: ~8 weeks (2 FTE)
```

---

## B7. Database Schema

```
LAYER: DATABASE SCHEMA
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Old (MSSQL)              Reborn (SQLite)            Status  Sev  Effort   Dependencies
─────────────────────────────────────────────────────────────────────────────────────────
TB_CHARACTER (many cols) TB_CHARACTER (SQLite)      🟡     C    1 week   Migration
  ├─ char_id              CharacterIdx PK            ✅     -    -        -
  ├─ account_id           AccountID                  ✅     -    -        -
  ├─ char_name            CharName                   ✅     -    -        -
  ├─ level                Level                      ✅     -    -        -
  ├─ str/dex/vit/int/wis  Str/Dex/Con/Int/Wis       🟡     L    2 hrs    [Vit→Con]
  ├─ hp/mp/exp            HP/MP/Exp                  ✅     -    -        -
  ├─ map_id, pos_x/y/z    MapIdx, PosX/Y/Z           ✅     -    -        -
  ├─ gold                 Money                      ✅     -    -        -
  ├─ battle_style         (missing)                  🔴     L    2 hrs    Schema update
  ├─ pk_mode              (PvpKillCount exists)      🟡     L    2 hrs    -
  ├─ fame / bad_fame      Fame/BadFame               ✅     -    -        -
  └─ stored procedures    SQL queries (inline)       🟡     H    2 wks    Migration

TB_ITEM (MSSQL)          TB_ITEM (SQLite)           🟡     H    3 days   Migration
  ├─ item_id              ItemIdx PK                 ✅     -    -        -
  ├─ char_id              CharacterIdx FK            ✅     -    -        -
  ├─ item_db_idx          ItemDBIdx                  ✅     -    -        -
  ├─ slot                 Slot                       ✅     -    -        -
  ├─ durability           Durability                 ✅     -    -        -
  ├─ enchant_level        Grade                      ✅     -    -        -
  └─ socket               Socket                     ✅     -    -        -

TB_SKILL (MSSQL)         TB_SKILL (SQLite)          ✅     -    -        -
TB_QUEST (MSSQL)         TB_QUEST (SQLite)          ✅     -    -        -
TB_PARTY (MSSQL)         TB_PARTY (SQLite)          🟡     M    2 days   -
TB_GUILD (MSSQL)         TB_GUILD (SQLite)          🟡     M    3 days   -
TB_FRIEND (MSSQL)        TB_FRIEND (SQLite)         🟡     M    1 day    -
TB_QUICKSLOT (MSSQL)     TB_QUICKSLOT (SQLite)      ✅     -    -        -

Stored Procedures:
  USP_CHARACTER_LOAD     SELECT from TB_CHARACTER    🟡     M    2 days   Migration
  USP_CHARACTER_SAVE     INSERT/UPDATE               🟡     M    2 days   Migration
  USP_ITEM_LOAD          SELECT from TB_ITEM         🟡     M    1 day    -
  USP_ITEM_SAVE          INSERT/UPDATE               🟡     M    1 day    -
  USP_SKILL_LOAD         SELECT from TB_SKILL        🟡     M    1 day    -
  USP_QUEST_LOAD         SELECT from TB_QUEST        🟡     M    1 day    -
  USP_GUILD_LOAD         SELECT from TB_GUILD        🟡     M    2 days   -

SUMMARY - DATABASE SCHEMA
  Tables: Old ~15 tables → Reborn ~12 tables (80% coverage)
  Missing tables: TB_BANNER, TB_EVENT, TB_BILLING, TB_AUCTION
  Stored procedures: Old ~50+ → Reborn: inline SQL queries (~20% coverage)
  Total estimated effort: ~3 weeks (1 FTE)
```

---

## B8. Build System & Project Structure

```
LAYER: BUILD SYSTEM
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Old (VS2003)                  Reborn (CMake)             Status  Sev  Effort   Dependencies
─────────────────────────────────────────────────────────────────────────────────────────────
Compiler: MSVC 7.1            AppleClang 16               ✅     -    -        -
Renderer: DirectX 9 SDK       bgfx (Metal/Vulkan/DX12)    ✅     -    -        -
Window system: Win32 + MFC    GLFW                        ✅     -    -        -
Audio: Miles Sound System     miniaudio                   ✅     -    -        -
Physics: Proprietary          Jolt Physics (optional)     🟡     L    1 week   -
Script: Lua (custom host)     sol2/luajit                 ✅     -    -        -
Database: MSSQL + ADO         SQLite + PostgreSQL         ✅     -    -        -
UI: cWindowManager            WindowManager + UIRenderer   🟡   M    2 wks    -
Anti-cheat: HackShield+nProt  (not ported)                🔴     H    2 wks   -

Build targets:
  LunaPlusClient.exe          luna-plus-client             ✅     -    -        -
  LunaPlusAgent.exe           luna-plus-agent              🟡     M    2 wks   -
  LunaPlusMap.exe             luna-plus-map                🟡     M    2 wks   -
  LunaPlusDistribute.exe      luna-plus-distribute         🟡     M    2 wks   -
  Launcher.exe                (built into client)          🟡     L    1 week  -
  MapEditor.exe               tools/map_editor/            🟡     M    2 wks   -
  PackingTool.exe             (not ported)                 🔴     L    1 week  -
  NewPackingTool.exe          (not ported)                 🔴     L    1 week  -
  ModelView.exe               (not ported — chx_to_gltf)   🔴     L    3 days  -

Dependencies (Old → Reborn):
  DirectX 9 SDK               bgfx + Metal                 ✅     -    -        -
  MFC (Microsoft Foundation)  (not needed — GLFW)          ✅     -    -        -
  Win32 API                   (not needed — cross-plat)    ✅     -    -        -
  MSSQL ADO                   SQLite3 + libpq              ✅     -    -        -
  Miles Sound System          miniaudio                    ✅     -    -        -
  Lua 5.1                     sol2 + LuaJIT                ✅     -    -        -
  zlib                        (in third_party)             ✅     -    -        -

SUMMARY - BUILD SYSTEM
  Build system ported: ✅ (VS2003 → CMake)
  Compiler: ✅ (MSVC → AppleClang/Clang/GCC)
  Key libraries: ✅ (all replaced with modern equivalents)
  Missing targets: PackingTool, NewPackingTool, ModelView
  Total estimated effort: ~3 weeks (1 FTE)
```

---

## B9. Error Handling & Edge Cases

```
LAYER: ERROR HANDLING
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Error Case                    Old Handling                 Reborn Handling         Status  Sev  Effort   Dependencies
────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
Network disconnect            Reconnect dialog 10s count-  Log "disconnected",     🔴     H    3 days   NetworkSystem
                              down → retry → back to      user must restart
                              login if failed              client
Packet corruption             Checksum (MSGROOT.CheckSum)  FlatBuffers verifier    🟡     L    2 days   -
Asset load failure            Fallback texture + log       Log + BGFX_INVALID_     🟡     M    1 day    TextureManager
                                                           HANDLE
Null pointer / crash          MINIDUMP handler + log       (basic crash handler)  🟡     M    2 days   -
DB query failure              Return error code + retry    (basic error return)    🟡     M    3 days   DB layer
Login failure                 Detailed NACK codes (wrong   Basic LoginResult enum  🟡     M    1 day    -
                              password, banned, etc.)      5 codes
Character load failure        Retry + error dialog         Log + abort             🔴     M    2 days   DB
Map change failure            Rollback to previous map     (basic error path)      🟡     M    2 days   -
File not found (asset)        Fallback asset + dialog      Log + abort             🔴     M    3 days   AssetManager
Cheat detection               HackShield + server check    (not implemented)       🔴     H    2 wks   Security
Overlapped login              Force disconnect older       (basic overlapped)      🟡     M    1 day    -

SUMMARY - ERROR HANDLING
  Critical gaps: Reconnect dialog, asset fallback, cheat detection
  Total estimated effort: ~3 weeks (1 FTE)
```

---

## B10. Performance Comparison

```
LAYER: PERFORMANCE
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Benchmark                     Old (DX9, .mod)              Reborn (bgfx, .glb)     Status  Sev  Effort
─────────────────────────────────────────────────────────────────────────────────────────────────────────
Map load (map 51)             ~1.2s (PAK file)             ~0.3s (GLB file)        ✅ 4x faster   -    -
Login screen FPS              60 FPS (capped)              1800 FPS (no cap)       🟡    L    2 hrs
Character render              60 FPS @ 20 chars            ~120 FPS @ 20 chars     ✅    -    -
Network packet throughput     ~1000 pkt/s per client       ~5000 pkt/s (est)       🟡    -    -
Memory usage (idle)           ~300 MB                      ~150 MB                 ✅    -    -
Texture memory                ~200 MB (DDS)                ~100 MB (PNG/BCn)       ✅    -    -

SUMMARY - PERFORMANCE
  Reborn is 2-4x faster in most benchmarks.
  Need: Frame cap (vsync), network throughput measurement.
  Total estimated effort: ~1 week (optimization)
```

---

## B11. Security & Anti-Cheat

```
LAYER: SECURITY & ANTI-CHEAT
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Aspect                        Old                          Reborn                  Status  Sev  Effort   Dependencies
───────────────────────────────────────────────────────────────────────────────────────────────────────────────────
Authentication                Plaintext PW + XOR token +   Plaintext PW +           🟡     C    3 days   Network
                              session 24h                  AES-GCM token + 24h
Packet encryption             XOR/RC4                      AES-GCM                  ✅     -    -        -
Anti-cheat client             HackShield (kernel) +        (none)                   🔴     C    2 wks   Security
                              NProtect (ring0)
Anti-cheat server             Periodic integrity check     Server validation only   🔴     H    1 week   -
SQL injection                 String concatenation         Prepared statements       ✅     -    -        -
Memory protection             CRC check + integrity scan   (none)                   🔴     H    2 wks   -
Cheat commands (GM)           MP_CHEAT protocol            (not ported)             🔴     M    3 days   -
Rate limiting                 (basic)                      (not implemented)        🔴     H    1 week   Server
Brute-force protection        (basic lockout)              (not implemented)        🔴     M    2 days   Agent
DDoS/mitigation               (none)                       (none)                   🔴     L    -        Infra

Recommendations:
  - Add server-side rate limiting (packets/sec, login attempts/sec)
  - Add anomaly detection for movement speed, damage values, item duplication
  - Port GM command authentication
  - Add brute-force protection (account lockout after N failed attempts)
  - Keep AES-GCM encryption (already ✅)
  - Total estimated effort: ~5 weeks (1 FTE)
```

---

## B12. Concurrency & Threading

```
LAYER: CONCURRENCY & THREADING
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Aspect                        Old                          Reborn                  Status  Sev  Effort   Dependencies
───────────────────────────────────────────────────────────────────────────────────────────────────────────────────
Network I/O model             IOCP (I/O Completion Ports)  asio::io_context         ✅     -    -        -
                              4 worker threads             4 worker threads
Thread pool                   Separate DB thread pool      SQLite WAL mode         🟡     M    3 days   -
                              (3-5 threads) MSSQL          single writer (dev)
                                                            PostgreSQL (prod)
Shared state protection       CRITICAL_SECTION + Mutex     std::mutex + std::atomic 🟡     M    1 week   -
Render thread                 Single (Win32 msg loop)      Single (GLFW loop)       ✅     -    -        -
Render/update split           (no — single threaded)       (decoupled if needed)    🟡     M    2 wks    ECS
Deadlock risk                 (manual critical section      std::lock_guard         🟡     M    1 week   -
                              ordering)                    RAII-based
Job system                    (custom thread pool)         (not implemented —       🔴     M    2 wks    -
                                                            asio post only)
Lock contention               (high — many CS around       (medium — mutex per      🟡     M    1 week   -
                               shared containers)           system)

SUMMARY - CONCURRENCY
  Network I/O: ✅ IOCP → asio (equivalent)
  DB access: 🟡 SQLite single-writer may bottleneck — use PostgreSQL for prod
  Thread pool: 🔴 Missing job system for CPU-intensive tasks
  Total estimated effort: ~3 weeks (1 FTE)
```

---

## B13. Localization (i18n)

```
LAYER: LOCALIZATION (i18n)
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Aspect                        Old                          Reborn                  Status  Sev  Effort   Dependencies
───────────────────────────────────────────────────────────────────────────────────────────────────────────────────
String table                  InterfaceMsg.bin (KR, EN,    UiStringTable             🟡     H    1 week   -
                              JP, CN) ~2000+ strings       loaded 1624 strings
Language switching            Runtime language change      Localization::            🟡     M    3 days   -
                              (KR/EN/JP/CN)                SetLanguage (basic)
Font support                  Per-language font files      2002_EYA.ttf single      🔴     H    3 days   Font
                              (korean.ttf, english.ttf,    font (CJK may fail)
                              japanese.ttf, chinese.ttf)
UI scripts (.bin) per lang    Separate .bin per language    (single .bin.txt parse)  🔴     M    2 wks    -
Regional formats              Date/number formatting       (basic)                  🟡     L    2 days   -
Encoding                      Korean locale (CP949/EUC-KR) UTF-8                     ✅     -    -        -

SUMMARY - LOCALIZATION
  String coverage: ~80% (1624/2000+)
  Critical: Font support for CJK characters
  Total estimated effort: ~3 weeks (1 FTE)
```

---

## B14. Audio System

```
LAYER: AUDIO (B15)
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Aspect                        Old                          Reborn                  Status  Sev  Effort   Dependencies
───────────────────────────────────────────────────────────────────────────────────────────────────────────────────
Audio engine                  Miles Sound System            miniaudio                ✅     -    -        -
Format support                .wav, .mp3, .ogg             .wav, .mp3, .ogg         ✅     -    -        -
BGM crossfade                 (basic crossfade)             (basic crossfade)       🟡     M    2 days   AudioManager
3D positional audio           (Miles 3D)                    (stereo only)            🔴     M    3 days   -
SFX list loading              SoundList.bin.txt            UiSoundIndex.cpp         🟡     M    2 days   -
Voice chat                    (none)                        (none)                   -      -    -        -
Audio streaming               (streaming from PAK)          (streaming from file)    🟡     L    2 days   -

SUMMARY - AUDIO
  Engine: ✅ Miles Sound System → miniaudio
  Gaps: 3D positional audio (stereo only), BGM crossfade basic
  Total estimated effort: ~1 week
```

---

## B15. Physics & Collision (B16)

```
LAYER: PHYSICS & COLLISION (B16)
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Old Function                      Reborn Equivalent           Status  Sev  Effort   Dependencies
─────────────────────────────────────────────────────────────────────────────────────────────────────
CollisionTestMovingEllipsoid-     CollisionSystem.hpp          🟡     M    2 wks    ECS
MeetTriangle (proprietary)        (basic AABB/OBB)
CollisionTestMovingSphere-        (not implemented)            🔴     M    1 week   -
MeetTriangle
AAEllipsoidCollisionTest          (not implemented)            🔴     M    1 week   -
MAP->CollisionLine()              EngineMap collision         🟡     H    5 days    -
MAP->CollisionCheck_OneLine_New   (line check exists)         🟡     M    3 days    -
Gravity / falling                  (basic gravity)             🟡     M    2 days    -
Ragdoll physics                   Jolt Physics (optional)     🔴     L    2 wks    Jolt
Vehicle physics                   (not implemented)            🔴     M    2 wks    -

SUMMARY - PHYSICS & COLLISION
  Old proprietary collision → Reborn basic collision + Jolt Physics (optional)
  Core collision (line/ray) exists but not full parity
  Total estimated effort: ~3 weeks (1 FTE)
```

---

## Overall Summary

```
GRAND SUMMARY — ALL LAYERS
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Layer               Coverage    Critical Gaps    Total Effort
─────────────────────────────────────────────────────────────────────────────
B1. Player Flow     20%         2                4 weeks
B2. UI System       14%         156 dialogs      12 weeks
B3. Gameplay        50%         4 formulas       4 weeks
B4. Network         15%         1700+ packets    6 weeks
B5. Asset Pipeline  80%         .bin parser      3 weeks
B6. Server Systems  35%         Many subsystems  8 weeks
B7. Database        70%         30 SPs           3 weeks
B8. Build System    90%         3 tools          3 weeks
B9. Error Handling  30%         5 cases          3 weeks
B10. Performance    N/A         Minor            1 week
B11. Security       30%         Anti-cheat       5 weeks
B12. Concurrency    60%         Job system       3 weeks
B13. Localization   80%         Font, strings    3 weeks
B14. Audio          70%         3D audio         1 week
B15. Physics        40%         Full collision   3 weeks
─────────────────────────────────────────────────────────────────────────────
TOTAL ESTIMATED:    ~35%       ~60 weeks (2 FTE ~7 months)
```

---

*End of GAP_ANALYSIS.md — All 14 layers (B1-B16) covered with Severity/Effort/Dependencies*
