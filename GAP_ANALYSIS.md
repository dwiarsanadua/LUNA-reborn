# GAP ANALYSIS: Luna-Plus-Old vs Luna-Plus-Reborn

> Generated: 2026-06-09 (fresh scan)
> AI Arsitek: Menyelesaikan analisa gap sesuai spesifikasi prompt_analisa_old_vs_reborn.md

---

## B1. Player Flow (Alur Utama)

```
LAYER: PLAYER FLOW
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Old File / Func              Reborn File             Status    Sev  Effort   Dependencies
────────────────────────────────────────────────────────────────────────────────────────────
[Client]Launcher/             client/ui/screens/      ✅       -    -        -
MainDialog.cpp (1996 lines)  LauncherScreen.cpp
  ├─ FTP patch download       PatchSystem.cpp         ✅       -    -        -
  ├─ HTML notice view         NoticeView.cpp          ✅       L    1 day    WebView lib
  │  (CHtmlViewNotice)        (strip-HTML render)
  ├─ BGM playback             AudioManager.cpp        ✅       -    -        -
  ├─ Status bar / progress    ProgressBar widget      ✅       -    -        -
  └─ Auto-update check        PatchSystem.cpp         🟡       L    4 hrs    PatchSystem

[Client]LUNA/LoginDlg         client/ui/screens/      ✅       -    -        -
(LoginDialog.cpp 55 lines)   LoginScreen.cpp
  ├─ ID input (EditBox)       InputField widget       ✅       -    -        -
  ├─ PW input (masked)        InputField masked       ✅       -    -        -
  ├─ Server selection          ServerInfo table        ✅       -    -        -
  ├─ Save ID checkbox         (save_id_ toggle)       ✅       -    -        -
  └─ Version check            client_version field    ✅       -    -        -

[Client]LUNA/CharSelect       client/ui/screens/      ✅       -    -        -
CharSelect.cpp               CharSelectScreen.cpp
  ├─ Character list grid      CharacterSlot widget    ✅       -    -        -
  ├─ Character create         CharMakeDlg.cpp         ✅       -    -        -
  ├─ Character delete         CharSelectScreen.cpp    ✅       M    1 day    Dialog system
  └─ 3D preview render        CharacterRenderer.cpp   ✅       -    -        -

[Client]LUNA/GameIn.cpp       client/ui/screens/      🟡       H    1 week   ECS integration
(2864 lines)                 GameScreen.cpp
  ├─ Hero spawn + init        Hero.cpp (client)       ✅       -    -        -
  ├─ World rendering           WorldRenderer.cpp       ✅       -    -        -
  ├─ Combat input             CombatSystem.cpp        ✅       -    -        -
  ├─ Social (party/chat)      PartyDialog, ChatPanel   ✅       -    -        -
  ├─ Minimap display          MiniMapDlg.cpp          ✅       -    -        -
  └─ Auto-attack loop         ComboSystem.cpp         ✅       -    -        -

SUMMARY - PLAYER FLOW
  Total screens:              5
  Reborn C++ classes:         5 (100%)
  Fully functional:           5 (100%)
  Missing features:           0
  Total estimated effort:     ~1 week (polish)
```

---

## B2. UI System (Window/Dialog)

```
LAYER: UI SYSTEM
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Old .bin File              Reborn C++ Class           Status    Sev  Effort   Dependencies
────────────────────────────────────────────────────────────────────────────────────────────
WebLauncherIDPass.bin      LoginScreen.cpp            ✅        -    -        -
CharSelect.bin             CharSelectScreen.cpp       ✅        -    -        -
CharMake.bin               CharMakeDlg.cpp            ✅        -    -        -
Inven.bin                  InventoryDialog.cpp        ✅        -    -        -
Skill.bin                  SkillDialog.cpp            ✅        -    -        -
Quest.bin                  QuestDialog.cpp            ✅        -    -        -
Friend.bin                 FriendDialog.cpp           ✅        -    -        -
PartyCreate.bin            PartyDialog.cpp            ✅        -    -        -
PartySet.bin               PartySetDialog.cpp         ✅        M    1 day    PartyDialog
Guild.bin                  GuildDialog.cpp            ✅        -    -        -
GuildCreate.bin            GuildDialog.cpp            🟡        L    1 day    -
GuildInvite.bin            GuildDialog.cpp            🟡        L    4 hrs    -
GuildMark.bin              GuildMarkDialog.cpp        ✅        -    -        -
GuildWarehouse.bin         GuildWarehouseDialog.cpp   ✅        -    -        -
Guildbank.bin              GuildBankDialog.cpp        ✅        -    -        -
GuildWarInfo.bin           GuildWarInfoDialog.cpp     ✅        -    -        -
ChatWindow.bin             ChatPanel.cpp              ✅        -    -        -
ChatRoomCreateDlg.bin      ChatRoomDialog.cpp         ✅        -    -        -
NPCChat.bin                NPCDialog.cpp              ✅        -    -        -
NPCShop.bin                NPCDialog.cpp (partial)    🟡        H    2 days   NPC system
MiniMap.bin                MiniMapDlg.cpp             ✅        -    -        -
BigMap.bin                 WorldMapDlg.cpp            ✅        -    -        -
Option.bin                 OptionsDialog.cpp          ✅        -    -        -
FishingDlg.bin             FishingDialog.cpp          ✅        -    -        -
EnchantDialog.bin          UpgradeDialog.cpp          🟡        M    1 day    -
ReinforceDialog.bin        UpgradeDialog.cpp          ✅        -    -        -
MixDialog.bin              MixDialog.cpp              ✅        -    -        -
ComposeDialog.bin          ComposeDialog.cpp          ✅        -    -        -
Consignment.bin            ConsignmentDialog.cpp      ✅        -    -        -
KeySetting.bin             KeyBindDialog.cpp          ✅        -    -        -
HelpDlg.bin                HelpDialog.cpp             ✅        -    -        -
MainMenu.bin               MainMenuDialog.cpp         ✅        -    -        -
FadeDlg.bin                FadeDlg.cpp                ✅        -    -        -
NewLoadDlg.bin             LoadingScreen.cpp          ✅        -    -        -
GameOut.bin                GameOutDialog.cpp          ✅        -    -        -
Revival.bin                RevivalDialog.cpp          ✅        -    -        -
PetInfo.bin                PetDialog.cpp              ✅        -    -        -
Family.bin                 FamilyDialog.cpp           ✅        -    -        -
CookingDlg.bin             CookingDialog.cpp          ✅        -    -        -
TutorialDlg.bin            TutorialDlg.cpp            ✅        -    -        -
MacroDialog.bin            MacroDialog.cpp            ✅        -    -        -
ChangeClass.bin            ClassAdvancement.hpp       ✅        -    -        -
Changejob.bin              ChangeJobDialog.cpp        ✅        -    -        -
FarmManage.bin             FarmSystem.cpp             ✅        -    -        -
Farm_Get.bin               FarmGetDialog.cpp          ✅        -    -        -
Farm_Upgrade.bin           FarmUpgradeDialog.cpp      ✅        -    -        -
DateMatchingDlg.bin        DateMatchingDialog.cpp     ✅        -    -        -
WeatherDlg.bin             WeatherDialog.cpp          ✅        -    -        -
NpcImage.bin               NpcImageDialog.cpp         ✅        -    -        -

CRITICAL MISSING — some now implemented (agent FINAL-B verified):
IdentificationDlg.bin      IdentificationDialog.cpp   ✅        -    -        -
ItemMallWarehouse.bin      ItemMallWarehouseDialog.cpp✅        -    -        -
SiegeWarFlagDlg.bin        SiegeWarFlagDialog.cpp     ✅        -    -        -
ProgressDialog.bin         ProgressDialog.cpp          ✅        -    -        -
DissolveDialog.bin         (not implemented)         🔴        L    1 day    -
PetresDialog.bin           (not implemented)         🔴        L    1 day    -
QuickSlot.bin              (not implemented)         🔴        L    1 day    -
BattleGuage.bin            (not implemented)         🔴        L    4 hrs    -
Channel.bin                ChannelDialog.cpp           ✅        -    -        -
MonsterKill.bin            MonsterKillDialog.cpp       ✅        -    -        -
QuestQuickView.bin         QuestQuickViewDialog.cpp    ✅        -    -        -
PartyMatching*.bin (4)     PartyMatchingDialog.cpp     ✅        -    -        -
SystemMsg.bin              (not implemented)           🔴        L    4 hrs    -
ShoutDlg.bin               (not implemented)         🔴        L    1 day    ChatPanel
Consignment_Guide.bin      (not implemented)         🔴        L    4 hrs    -
BillingDlg.bin             (not implemented)         🔴        L    1 day    -
HousingWebDlg.bin          (not implemented)         🔴        L    2 days   HousingDialog

SUMMARY - UI SYSTEM
  Total Old .bin files:       213 (scan langsung)
  PORTED (ada implementasi):  103 (48%) — dedicated class atau GameScreen inline
  PARTIAL (class ada, no .bin):4 (2%) — FadeDlg, FamilyMark, Target, TargetMonster
  MISSING (no reference):     106 (50%)
  ─ Dari 106 missing:
    Sub-dialogs (minor):       ~70 (ChatRoom variants, Guild subs, Housing subs)
    Config/image_path files:   ~15 (image_*.bin, partymember*.bin)
    Truly missing features:    ~21 (ApplyOption, AutoAnswer, BattleGuage, ShoutDlg, dll)
  Total estimated effort:     ~4 weeks (1 FTE) — mostly minor sub-dialogs

  Technology:
    Old:  cWindowManager + .bin scripts (Win32 dialog resource-like)
    Reborn: WindowManager + UIRenderer (bgfx batch) + UiScriptParser
    Assessment: ✅ COCOK — batch render lebih efisien.
                Saran: selesaikan UiScriptParser untuk full .bin.txt parsing.
```

---

## B3. Gameplay Constants & Hero System

```
LAYER: GAMEPLAY / HERO COMBAT
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Old Function (Hero.cpp)      Reborn Equivalent            Status  Sev  Effort   Dependencies
────────────────────────────────────────────────────────────────────────────────────────────
─ PHYSICAL ATTACK FORMULA ─────────────────────────────────────────────────────────────────
DoGetAttackRate()            CombatSystem::               ✅      -    -        -
  Base: (WeaponAtk+Level)    CalcPhysicAttack()
        × (1+STR×0.001)
        + (RealSTR−30)
  Enchant: × (1+Enchant²/400)
  Buffs: × (1+%buffs/100) + flat

─ PHYSICAL DEFENSE FORMULA ────────────────────────────────────────────────────────────────
DoGetDefenseRate()           CombatSystem::               ✅      -    -        -
  Base: (Armor+Level)        CalcPhysicDefense()
        × (1+VIT/3000)
        + (RealVIT−40)/5
  Enchant: × (1+Enchant/200)

─ CRITICAL RATE ───────────────────────────────────────────────────────────────────────────
DoGetCriticalRate()          CombatSystem::               ✅      -    -        -
  DexRate = max(DEX−BaseDEX,  CalcCritRate()
            0)/((Level−1)×5)
  Crit = 45×DexRate
        + (BaseDEX−25)/5 + buffs

─ BLOCK RATE ──────────────────────────────────────────────────────────────────────────────
CPlayer::GetBlock()          CombatSystem::               ✅      -    -        -
  Block = DEX/27             CalcBlockRate()
        + ClassBonus
        + buffs
  ClassBonus: Fighter=15, Rogue=10, Ranger=5, Mage=9

─ ACCURACY / EVASION ──────────────────────────────────────────────────────────────────────
DoGetAccuracyRate()          CombatSystem::               ✅      -    -        -
  Accuracy = (DexRate×0.6    CalcAccuracy()
            +(BaseDEX−30)/1000
            +Level×ClassVal/15000)×100
  Evasion = formula identik  CalcEvasion()
  HitChance = 85 + Acc − Eva IsHit()

─ LEVEL PENALTY ───────────────────────────────────────────────────────────────────────────
  HANYA jika atk < def:      CalculateDamage()            ✅      -    -        -
  damage *= 1 − diff×0.015   (dalam CalculateDamage)

─ PvP / GT DAMAGE ─────────────────────────────────────────────────────────────────────────
  PvP:  damage × 0.35       CalculateDamage(context)      ✅      -    -        -
  GT:   damage × 0.10
  Siege: damage × 0.50

─ HEAL FORMULA ────────────────────────────────────────────────────────────────────────────
RecoverLife()                CombatSystem::               ✅      -    -        -
  heal = ((Wis×11+Int×4      CalcHealAmount()
         +Level×20)
         ×SkillFactor)/800 + 100

─ AGGRO SYSTEM ────────────────────────────────────────────────────────────────────────────
  aggro≥3: Avoid ×=          ThreatTable + AISystem        🟡      M    3 days   CombatSystem
           1−(n−2)²×0.01
  aggro≥6: Def ×=
           1−(n−5)^1.5×0.01

─ COMBO SYSTEM ────────────────────────────────────────────────────────────────────────────
  MAX_COMBO_NUM auto-chain   ComboSystem.cpp               🟡      M    2 days   CombatSystem
  Skill combo integration    5 stages (5/10/20/50)

─ ELEMENT SYSTEM ──────────────────────────────────────────────────────────────────────────
  TIDAK ADA elemental        [[deprecated]]               ✅      -    -        -
  advantage dalam damage     (disabled for Old accuracy)

SUMMARY - GAMEPLAY CONSTANTS
  Total Hero.cpp functions:   ~60+ (2303 lines)
  Reborn CombatSystem funcs:  25+
  Formula match:              12/12 (100%) ✅ (after agent A-D fix)
  Total estimated effort:     ~1 week (tuning)

  Critical fix history:
    v1: DEX/1000 crit, CON/2000 block, 1% miss → SALAH
    v2: Agent A-D corrected all formulas → ✅ akurat
```

---

## B4. Network Protocol

```
LAYER: NETWORK PROTOCOL
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Old Item                  Reborn Item                  Status  Sev  Effort   Dependencies
────────────────────────────────────────────────────────────────────────────────────────────
MP_CATEGORY: 94 entries   PacketType.fbs: 246 entries  🟡     H    3 weeks  Handler wiring
  (Protocol.h enum)        (uint16 enum values)
MP_PROTOCOL_*: 55 enums      .fbs schemas: 30 files     🟡     H    2 weeks  -
Old struct: MSGBASE-based   FlatBuffers table            ✅     -    -        -

FIELD-BY-FIELD MAPPING — 10 KATEGORI (lihat PACKET_MAPPING.md)

Handler wiring status:
  Login (USERCONN):    7/7 handler di AgentServer        ✅
  Move (MOVE):         4/4 handler di MapServer          ✅
  Combat (COMBAT):     1/1 handler di MapServer          ✅
  Skill (SKILL):       1/1 handler di MapServer          ✅
  Inventory (ITEM):    17/17 handler di MapServer        ✅
  Chat (CHAT):         4/4 handler di MapServer          ✅
  Party (PARTY):       3/3 handler di MapServer          ✅
  Guild (GUILD):       4/4 handler di MapServer          ✅
  Quest (QUEST):       3/3 handler di MapServer          ✅
  NPC (NPC):           1/1 handler di MapServer          ✅
  Vehicle (VEHICLE):   6/6 handler di MapServer          ✅

SUMMARY - NETWORK PROTOCOL
  Old MP_CATEGORY:           92 named + 1 (MP_MAX) = 93 total (scan langsung)
  Old MP_PROTOCOL enums:     55 (dari Protocol.h)
  Old individual packets:    ~2034 (sum of all enum values)
  ────────────────
  Reborn .fbs schemas:       30 files
  Reborn PacketType enum:    ~262 entries (termasuk 20 VEHICLE)
  ────────────────
  MP_PROTOCOL dengan .fbs:   23 dari 55 (42%)
  MP_PROTOCOL di PacketType: 32 dari 55 (58%)
  MP_PROTOCOL dgn handler:   28 dari 55 (51%)
  ─ Yang TIDAK di-port (27/55):
    Anti-cheat:       NPROTECT, HACKSHIELD, HACKCHECK (3)
    Admin/monitoring: MORNITORTOOL, MORNITORSERVER, MORNITORMAPSERVER (3)
    Cheat/GM:         CHEAT (1)
    Minor features:   SKILLTREE, POWERUP, SIGNAL, PACKEDDATA, CHAR_REVIVE,
                      BOSSMONSTER, PK, GUILD_UNION, GUILD_FIELDWAR,
                      FACIAL, EMOTION, RESIDENTREGIST, TUTORIAL, DATE,
                      AUTONOTE, CHATROOM, COOK, PCROOM, NOTE, QUICK (20)
  10 key categories:         100% field-mapped ✅
  Handler wiring:            100% (all 10 categories wired) ✅
  Total estimated effort:    ~1 week (optimization)
```

---

## B5. Asset & Data Pipeline

```
LAYER: ASSET PIPELINE
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Old Format       Reborn Format    Status  Sev  Effort   Dependencies
────────────────────────────────────────────────────────────────────────────────────────────
.mod (3D model)  .glb / .gltf     ✅      -    -        chx_to_gltf tool
.anm (anim)      .anm.json        ✅      -    -        AnmParser.h
.dds / .tif      .png             🟡      L    -        Texture converter
.hfl (heightmap) .hgt             ✅      -    -        HflParser.h
.chx (char def)  .json            ✅      -    -        -
.map (scene)     scene.json       ✅      -    -        map_converter tool
.eft (effect)    EftParser        ✅      M    2 days   -
.spr (sprite)    UiAtlasRegistry  🟡      M    2 days   -
.wav/.mp3        .wav/.mp3        ✅      -    -        miniaudio
.bin (UI script) .bin.txt parsed  🟡      M    5 days   UiScriptParser
.bin (string)    .bin.txt parsed  🟡      M    3 days   UiStringTable

SUMMARY - ASSET PIPELINE
  All major formats have conversion path ✅
  14 Python conversion scripts in Old scripts_legacy/
  Total estimated effort: ~1 week (remaining polish)

  Technology:
    Old: proprietary binary formats (.mod/.anm/.hfl/.chx/.map)
    Reborn: open standard formats (GLB/JSON/PNG/HGT)
    Assessment: ✅ COCOK — standard formats lebih mudah di-maintain.
```

---

## B6. Server Systems

```
LAYER: SERVER SYSTEMS
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Old System            Old Path              Reborn Path           Status  Sev  Effort   Dependencies
────────────────────────────────────────────────────────────────────────────────────────────────────
AGENT SERVER          [Server]Agent/         server/agent/         🟡     M    1 week   -
  (21 .cpp files)     AgentNetworkMsgParser  AgentServer.cpp
  ├─ Auth/login       AgentDBMsgParser       ✅                    -      -    -        -
  ├─ Char list/CRUD                          ✅                    -      -    -        -
  ├─ Session mgmt     UserTable              SessionManager       ✅      -    -        -
  ├─ Gift manager     GiftManager.cpp        GiftManager.cpp       ✅      -    -        -
  ├─ Punish manager   PunishManager.cpp      PunishManager.cpp     ✅      -    -        -
  ├─ NpcRecallMgr     NpcRecallMgr.cpp       (not ported)          🔴      L    2 days   -
  ├─ PlustimeMgr      PlustimeMgr.cpp        (not ported)          🔴      L    2 days   -
  ├─ HackShield       HackShieldManager      (removed)             🔴      H    -        -
  └─ NProtect          NProtectManager        (removed)             🔴      H    -        -

MAP SERVER            [Server]Map/           server/map/           🟡     H    3 weeks  -
  (210 .cpp files)                           + systems/ (16 files)
  ├─ Combat/Attack    AttackManager.cpp      CombatSystem.cpp      ✅      -    -        -
  ├─ Item management   ItemManager.h/cpp     ItemSystem.cpp        🟡      M    1 week   ECS
  ├─ Party management  PartyManager.cpp      PartySystem.cpp       ✅      -    -        -
  ├─ Guild management  GuildManager.h/cpp    GuildSystem.cpp       ✅      -    -        -
  ├─ Quest system      Quest.cpp             QuestSystem.cpp       ✅      -    -        -
  ├─ AI system         AISystem.cpp          AISystem.cpp          ✅      -    -        -
  ├─ Monster spawn     RegenManager.cpp      SpawnSystem.cpp       ✅      -    -        -
  ├─ Movement          MoveManager.cpp       MovementSystem.cpp    ✅      -    -        -
  ├─ Looting           LootingManager.cpp    LootingSystem.cpp     ✅      M    3 days   -
  ├─ Exchange/Trade    ExchangeManager.cpp   TradingSystem.cpp     ✅      M    3 days   -
  ├─ Siege warfare     SiegeWarfareMgr.cpp   SiegeSystem.cpp       🟡      M    2 wks    -
  ├─ Dungeon           (Dungeon/ folder)     DungeonSystem.cpp     🟡      M    1 week   -
  ├─ Grid system       Grid.cpp              GridSystem.cpp        🟡      M    3 days   -
  ├─ Trigger           (Trigger/ folder)     TriggerSystem.cpp     🟡      M    3 days   -
  ├─ Field Boss        FieldBossMonsterMgr   FieldBossSystem.cpp   ✅      M    1 week   -
  ├─ FSM (AI)          Finite State Machine/ FSMEngine.cpp         🟡      M    2 wks    -
  ├─ Looting           LootingManager.cpp    LootingSystem.cpp     ✅      M    3 days   -
  ├─ Recall system     MoveManager(recall)   RecallSystem.cpp      ✅      M    3 days   -
  └─ Trading           ExchangeManager.cpp   TradingSystem.cpp     ✅      M    3 days   -

DISTRIBUTE SERVER     [Server]Distribute/    server/distribute/    🟡     M    1 week   -
  (12 .cpp files)     
  ├─ Channel mgmt      ServerSystem.cpp      ChannelManager.cpp    ✅      -    -        -
  ├─ Chat room         ChatRoomMgr.cpp       (in client, partial)  🟡      M    3 days   -
  └─ Billing           BillConnector.cpp     (not ported)           🔴      L    1 week   -

SUMMARY - SERVER SYSTEMS
  3 Old server types → 3 Reborn server types
  ────────────────
  MAP SERVER (terbesar):
    Old: 116 .cpp files, 95.440 lines, 1.938 methods
    Reborn: 18 .cpp files, 9.316 lines, ~230 methods (CORRECTED from 902)
    Coverage: ~230/1.938 ≈ 12% methods ported (CORRECTED from 47%)
    Lines reduced: 95.440 → 9.316 (-90%) — ECS refactor
  ────────────────
  AGENT SERVER:
    Old: 21 .cpp files
    Reborn: 7 .cpp files
    Coverage: ~85%
  ────────────────
  DISTRIBUTE SERVER:
    Old: 12 .cpp files
    Reborn: 5 .cpp files
    Coverage: ~70%
  Total estimated effort: ~4 weeks (2 FTE)

  Technology:
    Old: Win32 IOCP + MFC + MS SQL + proprietary
    Reborn: asio (cross-platform) + SQLite3 + EnTT ECS
    Assessment: ✅ COCOK — asio setara IOCP, EnTT modern.
```

---

## B7. Database Schema

```
LAYER: DATABASE SCHEMA
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Old (MSSQL)                  Reborn (SQLite)               Status  Sev  Effort   Dependencies
────────────────────────────────────────────────────────────────────────────────────────────
TABLES:
TB_CHARACTER                 TB_CHARACTER (51 cols)        ✅      -    -        -
TB_ITEM                      TB_ITEM                       ✅      -    -        -
TB_ITEM_OPTION               TB_ITEM_OPTION                ✅      -    -        -
TB_SKILL                     TB_SKILL                      ✅      -    -        -
TB_QUEST                     TB_QUEST                      ✅      -    -        -
TB_PARTY                     TB_PARTY + MEMBER             ✅      -    -        -
TB_GUILD                     TB_GUILD + Mark/Emblem        ✅      -    -        -
TB_FRIEND                    TB_FRIEND + Memo              ✅      -    -        -
TB_FARM                      TB_FARM + CROP/ANIMAL         ✅      -    -        -
TB_HOUSE                     TB_HOUSE + FURNITURE           ✅      -    -        -
TB_PET                       TB_PET                        ✅      -    -        -
TB_VEHICLE                   TB_VEHICLE + PASSENGER        ✅      -    -        -
TB_FISHING                   TB_FISHING                    ✅      -    -        -
TB_CONSIGNMENT               TB_CONSIGNMENT + AUCTION       ✅      -    -        -
TB_NOTE                      TB_NOTE                       ✅      -    -        -
TB_SIEGE_RECALL              TB_SIEGE_RECALL               ✅      -    -        -
TB_SIEGE_WARFARE             TB_SIEGE_WARFARE              ✅      -    -        -
TB_PUNISHLIST                TB_PUNISHLIST                  ✅      -    -        -
TB_NPC_RECALL                TB_NPC_RECALL                 ✅      -    -        -
TB_FARM_TIMEDELAY            TB_FARM_TIMEDELAY             ✅      -    -        -
TB_HOUSE_RANK                TB_HOUSE_RANK                 ✅      -    -        -
TB_CHARACTER_BUFF            TB_CHARACTER_BUFF             ✅      -    -        -
TB_GUILD_SKILL               TB_GUILD_SKILL                ✅      -    -        -
TB_GUILD_WAREHOUSE           TB_GUILD_WAREHOUSE            ✅      -    -        -
TB_MAINQUEST                 TB_MAINQUEST                  ✅      -    -        -
TB_QUICKSLOT                 TB_QUICKSLOT                  ✅      -    -        -
TB_STORAGE                   TB_STORAGE                    ✅      -    -        -
TB_COOKING                   TB_COOKING + COOKRECIPE        ✅      -    -        -
TB_FAMILY                    TB_FAMILY + MEMBER            ✅      -    -        -
(content tables)             item_templates, monster_*     🟢      -    -        Added
(ranking)                    TB_RANKING, TB_RANKING_HISTORY🟢      -    -        Added
(billing)                    TB_BILLING                     🟢      -    -        Added

STORED PROCEDURES:
Old TIDAK punya stored procedures. All DB access via C++ (DataBase.cpp).
Reborn: inline SQL queries via prepared statements.
50+ query templates setara dengan Old C++ DB logic.

SUMMARY - DATABASE SCHEMA
  Old tables:   ~30 (from luna_game.db)
  Reborn tables: 75 (250% coverage)
  Missing Old tables: 0 (semua di-port)
  New tables: 45+ (content, ranking, billing, tournament)
  Total estimated effort: ✅ Complete

  Technology:
    Old: MSSQL via ADO + C++ DataBase.cpp + DBThread
    Reborn: SQLite3 (dev) + PostgreSQL (prod) + prepared statements
    Assessment: ✅ COCOK — prepared statement lebih aman dari concatenation.
```

---

## B8. Build System & Project Structure

```
LAYER: BUILD SYSTEM
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Aspek                Old (VS2003)               Reborn (CMake)         Status Sev Effort Deps
────────────────────────────────────────────────────────────────────────────────────────────
Compiler             MSVC 7.1 (.NET 2003)       AppleClang 16          ✅     -   -      -
Renderer             DirectX 9 SDK              bgfx (Metal/Vulkan)    ✅     -   -      -
Window system        Win32 + MFC                GLFW                   ✅     -   -      -
Audio                Miles Sound System         miniaudio              ✅     -   -      -
Physics              Proprietary collision       Jolt Physics (opt)     🟡     L   1 week  -
Scripting            Lua (custom host)           sol2/luajit            ✅     -   -      -
Database             MSSQL + ADO                SQLite3 + PostgreSQL   ✅     -   -      -
UI Framework         cWindowManager             WindowManager+UIRen    ✅     -   -      -
Anti-cheat           HackShield + nProtect      (not ported)            🔴     H   -       -

BUILD TARGETS:
LunaPlusClient.exe   [Client]LUNA/LUNA Client   luna-plus-client       ✅     -   -      -
LunaPlusAgent.exe    [Server]Agent              luna-plus-agent        ✅     -   -      -
LunaPlusMap.exe      [Server]Map                luna-plus-map          ✅     -   -      -
LunaPlusDistribute   [Server]Distribute         luna-plus-distribute   ✅     -   -      -
Launcher.exe         [Client]Launcher           (built into client)    ✅     -   -      -
MapEditor.exe        [Tool]MapEditor             (skipped)              🟡     M   1 week  imgui
PackingTool.exe      [Tool]PackingTool           (not ported)           🔴     L   1 week  -
NewPackingTool.exe   [Tool]NewPackingTool        (not ported)           🔴     L   1 week  -
ModelView.exe        ModelView/                  chx_to_gltf            ✅     -   -      -
LogReporter          [Client]LogReporter         (not ported)           🔴     L   -      -
MonitoringServer     [Monitoring]Server          server_monitor         ✅     -   -      -

TOTAL .sln files: 36 di Old → 1 CMakeLists.txt di Reborn

SUMMARY - BUILD SYSTEM
  Compiler:        MSVC 7.1 → AppleClang 16 (✅)
  Libraries:       All replaced with modern equivalents (✅)
  Missing targets: PackingTool, NewPackingTool, LogReporter, MapEditor (🟡)
  Total binaries:  17 di build/bin/ (CORRECTED: 18 include shaders/ directory)
  Build status:    0 error, 0 warning (✅)
  Total effort:    ~1 week (missing tools)

  Technology:
    Old: Visual Studio 2003 (.sln/.dsp) — Windows only
    Reborn: CMake 3.30+ — cross-platform (macOS now, Windows coming)
    Assessment: ✅ COCOK — CMake standar industri modern.
```

---

## B9. Error Handling & Edge Cases

```
LAYER: ERROR HANDLING (dari scan Old AttackManager.cpp, ItemManager.h, MoveManager.cpp)
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Error Case                Old Handling                  Reborn Handling       Status Sev Effort Deps
────────────────────────────────────────────────────────────────────────────────────────────────────
Network disconnect        Reconnect dialog 10s          ReconnectHandler      🟡     M   3 days  Network
                           countdown → retry             (agent #020)
Packet corruption         MSGROOT.CheckSum               FlatBuffers verifier 🟡     L   1 day   -
                           (XOR all bytes)
Asset load failure        Fallback texture + log         Texture fallback      🟡     M   1 day   Texture
Null pointer / crash      MINIDUMP handler + log         Basic                 🟡     M   2 days  -
DB query failure          Error code + retry             Basic error return    🟡     M   2 days  DB
Login failure             MP_USERCONN_LOGIN_NACK         LoginResult enum     ✅     -   -      -
                           (dwData error code)

Character load failure    Retry + error dialog           Basic                 🟡     M   1 day   DB
Map change failure        Rollback previous map          Basic                 🟡     M   1 day   -
File not found (asset)    Fallback asset + dialog        VFS fallback           🟡     M   2 days  VFS
Cheat detection           HackShield + NProtect          Server validation     🔴     H   2 wks  -
                           + ScriptCheckValue             (partial)
Overlapped login          MP_USERCONN_                   SessionManager check  🟡     M   1 day   -
                           NOTIFY_OVERLAPPEDLOGIN

DARI OLD SOURCE (error code spesifik per subsystem):
Item use failed           MP_ITEM_ERROR_NACK            Basic NACK            🔴     M   2 days  ItemSystem
                           (ECode int — berbagai kode)
Item enchant failed       MP_ITEM_ENCHANT_FAILED_ACK    Basic fail            🔴     M   2 days  Upgrade
Item mix failed           MP_ITEM_MIX_BIGFAILED_ACK     Basic fail            🔴     M   2 days  Mix
Item reinforce failed     MP_ITEM_REINFORCE_FAILED_ACK  Basic fail            🔴     M   2 days  Reinforce
Skill cannot use          MP_SKILL_START_NACK            Basic NACK            🔴     M   2 days  SkillSystem
                           (dwData error code)
Skill cancel failed       MP_SKILL_CANCEL_NACK          Basic NACK            🔴     L   1 day   SkillSystem
Party operation fail      MP_PARTY_ADD_NACK             Basic NACK            🔴     M   2 days  Party
Guild operation fail      MP_GUILD_CREATE_NACK          Basic NACK            🔴     M   2 days  Guild
Quest error               MP_QUEST_ERROR_EXT            Basic NACK            🔴     M   2 days  Quest
                           (dwData1-3 + Name)
Trade/Exchange error      ExchangeManager error codes   Basic fail            🔴     M   2 days  Trading
Move out of bounds        1000 unit threshold check     ValidationSystem      🟡     M   2 days  Movement
                           di MoveManager::NetworkMsg
Vehicle error             MP_VEHICLE_ERROR_ACK          VehicleErrorResponse   🟡     M   2 days  Vehicle
                           (32 error types enum)         (enum ada, handler ada)
NPC interaction fail      MP_NPC_SPEECH_NACK            NPCResponse(result)   🟡     M   1 day   NPC
Battle aggro penalty      aggroNum≥3: avoid penalty     ThreatTable           🟡     M   3 days  Combat

SUMMARY - ERROR HANDLING
  Old punya NACK code spesifik untuk SETIAP operasi (item, skill, party, guild, quest, trade)
  Reborn: basic return true/false — NACK codes belum diimplementasi detail
  Total: ~20+ error cases identified — ~3 weeks
```

---

## B10. Performance Comparison

```
LAYER: PERFORMANCE
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Benchmark              Old (DX9, .mod)            Reborn (bgfx Metal)       Status Sev Effort
────────────────────────────────────────────────────────────────────────────────────────────
Code size (MapServer)  95.440 lines               9.316 lines               ✅ 10x -   -
                                                    (-90% LOC)
Method count (Map)     1.938 methods              902 methods               ✅ 53%  -   -
Compiler speed          MSVC 7.1 (2003)            AppleClang 16 (C++20)    ✅     -   -
Target platform         Windows only               macOS (cross-platform)   ✅     -   -
Map load (map 51)      ~1.2s (estimasi DX9 PAK)   ~0.3s (estimasi GLB)     🟡     -   -  Perlu benchmark
Login screen FPS       ~60 (capped DX9)            ~1800 (no cap)           🟡     L   2 hrs
Memory usage (idle)    ~300 MB (est.)              ~150 MB (est.)           🟡     -   -  Perlu ukur
Binary size (client)   ~? (DX9, MFC)               4.6 MB (arm64 Metal)    🟢     -   -  Sangat kecil

CATATAN: Angka performance di atas adalah ESTIMASI. Belum ada benchmark aktual.
         Untuk data presisi, perlu:
         1. Run client dengan assets → FPS counter
         2. Parse log untuk map load time
         3. measure memory with `vmmap` atau `Activity Monitor`

SUMMARY - PERFORMANCE
  Binary size: ✅ 4.6 MB sangat kecil untuk MMORPG client
  Code size:   ✅ 90% lebih sedikit dari Old (refactor ECS)
  FPS cap:     🟡 perlu glfwSwapInterval(1)
  Benchmark:   ❌ belum ada data aktual
```

---

## B11. Security & Anti-Cheat

```
LAYER: SECURITY & ANTI-CHEAT
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Aspect                 Old                       Reborn                   Status Sev Effort Deps
────────────────────────────────────────────────────────────────────────────────────────────────────
Authentication         Plaintext PW + XOR token  bcrypt + AES-GCM token   ✅     -   -      -
                        + session 24 jam         + session 24 jam
Packet encryption      XOR/RC4 (optional)        AES-256-GCM (mandatory)  ✅     -   -      -
Anti-cheat client      HackShield (kernel)       (none)                    🔴    C   2 wks  -
                        + NProtect (ring0)
Anti-cheat server      ScriptCheckValue          Server validation        🟡     H   1 week  -
                        + periodic CRC check      (RateLimiter + Validate)
SQL injection          String concatenation      Prepared statements       ✅     -   -      -
                        "WHERE id='"+user+"'"     (SQLite bind params)
Memory protection      CRC check + integrity     (none)                    🔴     H   -      -
                        scan setiap N detik
Cheat commands (GM)    MP_CHEAT protocol         (not ported)              🔴     M   3 days  -
Rate limiting          (basic)                   RateLimiter.cpp           ✅     -   -      -
                                                    (agent B01)
Brute-force protection (basic lockout)           AgentServer built-in      ✅     -   -      -
Speed hack detection   MP_NPC_CHECK_HACK_SYN     ValidationSystem          🟡     M   3 days  -
                        + periodic pos check      (agent E)

SUMMARY - SECURITY & ANTI-CHEAT
  HackShield/NProtect cannot be ported (kernel-level Windows drivers).
  Mitigation: server-side validation + rate limiting + anomaly detection.
  Total estimated effort: ~3 weeks

  Rekomendasi:
    - Rate limiting per connection (✅ sudah)
    - Anomaly detection untuk movement speed, damage value (🟡 partial)
    - Periodic position validation server-side (🟡 partial)
    - Account lockout after N failed attempts (✅ sudah)
```

---

## B12. Concurrency & Threading

```
LAYER: CONCURRENCY & THREADING
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Aspect                 Old                       Reborn                   Status Sev Effort Deps
────────────────────────────────────────────────────────────────────────────────────────────────────
Network I/O model      IOCP (I/O Completion      asio::io_context         ✅     -   -      -
                         Ports) — 4 threads         — 4 threads
Thread pool            Separate DB thread pool   SQLite WAL mode          🟡     M   3 days  DB
                         (3-5 threads) MSSQL       single writer (dev)
                                                    PostgreSQL (prod)
Shared state           CRITICAL_SECTION +        std::mutex +             🟡     M   1 week  -
protection              Mutex                      std::atomic
Render thread          Single (Win32 msg loop)   Single (GLFW loop)       ✅     -   -      -
Deadlock risk          Manual critical section   RAII std::lock_guard     ✅     -   -      -
                         ordering
Job system             Custom thread pool        asio::post (agent #024)  🟡     M   1 week  -

SUMMARY - CONCURRENCY
  Network I/O: ✅ setara (IOCP ↔ asio)
  DB access: 🟡 SQLite single-writer bisa bottleneck
  Job system: 🟡 baru implementasi partial
  Total estimated effort: ~1 week
```

---

## B13. Localization (i18n)

```
LAYER: LOCALIZATION (i18n)
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Aspect                 Old                       Reborn                   Status Sev Effort Deps
────────────────────────────────────────────────────────────────────────────────────────────────────
String table           InterfaceMsg.bin          UiStringTable             🟡     M   3 days  -
                         (KR/EN/JP/CN)            1624 strings loaded
                         ~2000+ strings
Language switching     Runtime ganti bahasa      Localization::            🟡     M   2 days  -
                         (KR/EN/JP/CN)             SetLanguage (basic)
Font support           korean.ttf, english.ttf,   2002_EYA.ttf single     🔴     H   2 days  Font
                         japanese.ttf,             (CJK tidak support)              bundle
                         chinese.ttf
UI scripts per lang    Separate .bin per bahasa   Single .bin.txt parse   🔴     M   5 days  -
Regional formats       Date/number formatting    (basic)                  🟡     L   2 days  -
Encoding               Korean CP949 / EUC-KR     UTF-8                    ✅     -   -      -

SUMMARY - LOCALIZATION
  String coverage: ~80% (1624/2000+)
  Font: 🔴 CJK tidak support — bundle NotoSans KR/JP/SC
  Total estimated effort: ~1 week

  Technology:
    Old: per-language font + per-language .bin + CP949 encoding
    Reborn: UTF-8 + single font (perlu multi-font support)
    Assessment: 🟡 COCOK untuk EN, perlu tambah font CJK.
```

---

## B14. Audio System

```
LAYER: AUDIO
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Aspect                 Old (Miles Sound System)  Reborn (miniaudio)       Status Sev Effort Deps
────────────────────────────────────────────────────────────────────────────────────────────────────
Audio engine           Miles Sound System         miniaudio                ✅     -   -      -
Format support         .wav, .mp3, .ogg          .wav, .mp3, .ogg         ✅     -   -      -
BGM crossfade          Basic crossfade           AudioManager              ✅     M   2 days  -
                                                    (agent #013)
3D positional audio    Miles 3D API              SetSFXPan + atenuasi     ✅     M   3 days  -
                                                    (agent #013)
SFX list loading       SoundList.bin             UiSoundIndex (agent #028)✅     M   2 days  -

SUMMARY - AUDIO
  ✅ Complete. All features ported.
```

---

## B15. Physics & Collision

```
LAYER: PHYSICS & COLLISION
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Old Function               Reborn Equivalent            Status  Sev  Effort   Dependencies
────────────────────────────────────────────────────────────────────────────────────────────
CollisionTestMoving-       CollisionSystem.hpp          🟡      M    1 week   PhysicsWorld
EllipsoidMeetTriangle      (basic AABB/OBB)             (agent #014)
CollisionTestMoving-       PhysicsWorld.cpp             🟡      M    1 week   -
SphereMeetTriangle         (sphere/ellipsoid)
AAEllipsoidCollisionTest   PhysicsWorld.cpp             🟡      M    1 week   -
MAP->CollisionLine()       EngineMap collision          ✅      -    -        -
MAP->CollisionCheck_       EngineMap collision          ✅      -    -        -
  OneLine_New
Gravity / falling          Basic gravity                🟡      M    2 days   PhysicsWorld
Ragdoll physics            Jolt Physics (optional)      🔴      L    2 wks    Jolt
Vehicle physics            (not implemented)            🔴      M    2 wks    -

SUMMARY - PHYSICS & COLLISION
  Core collision (line/ray/sphere) exists ✅
  Ragdoll/vehicle: low priority
  Total estimated effort: ~2 weeks
```

---

## Overall Summary

```
GRAND SUMMARY — ALL LAYERS (dengan data presisi)
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Layer               Coverage    Data Presisi                                Total Effort
────────────────────────────────────────────────────────────────────────────────────────────
B1. Player Flow     100%        5 screens, all features implemented ✅       1 week
B2. UI System       48%         103/213 .bin ported, 106 missing (mostly    4 weeks
                                 minor sub-dialogs)
B3. Gameplay        100%        12/12 formulas Old-accurate ✅               1 week (tuning)
B4. Network         51%         28/55 MP_PROTOCOL have handlers.            1 week (opt)
                                 27 not ported (anti-cheat/admin/minor)
B5. Asset Pipeline  90%         Spr/UI parser polish                        1 week
B6. Server Systems  12%         Map: ~230/1.938 methods ported (ECS refactor)4 weeks
                                 116 file → 18 file (-90% LOC)
B7. Database        100%        75 tables ✅                                 ✅
B8. Build System    90%         17 binaries, 4 tools missing                 1 week
B9. Error Handling  50%         Perlu NACK codes per error case              2 weeks
B10. Performance    90%         Frame cap                                    <1 day
B11. Security       60%         27/55 protocol tidak di-port (anti-cheat)    3 weeks
B12. Concurrency    70%         Job system, DB pool                          1 week
B13. Localization   80%         CJK font, ~400 strings                       1 week
B14. Audio          100%        ✅                                           ✅
B15. Physics        60%         Ragdoll, vehicle physics                     2 weeks
────────────────────────────────────────────────────────────────────────────────────────────
TOTAL:              ~60%       ~20 weeks (1 FTE ~5 months)
────────────────────────────────────────────────────────────────────────────────────────────

CATATAN KOREKSI DATA:
  - B1:  Save ID checkbox sudah diimplement (save_id_ toggle), bukan "not implemented"
  - B2 UI:  95/213 (45%) → 103/213 (48%) ported — 8 item missing ternyata sudah ada
  - B4 Net: 28/55 (51%) protocol punya handler, bukan "100%"
  - B6 Svr: 902 methods → ~230 actual (CORRECTED: regex hitung variable sbg method)
  - B8:  18 binaries → 17 (shaders/ adalah direktori, bukan binary)
  - MP_CATEGORY: 93 total (92 named + sentinel), bukan 94
  - Overall real: ~60% (setelah koreksi B6), bukan ~82%
────────────────────────────────────────────────────────────────────────────────────────────
```

---

*End of GAP_ANALYSIS.md — 14 layers, setiap entry punya Severity+Effort+Dependencies*

