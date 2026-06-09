# GAP ANALYSIS v3 — Final: Luna-Plus-Old vs Luna-Plus-Reborn

> Generated: 2026-06-09 (Scan langsung Old codebase file-by-file)
> Scope: 14 Layers (B1-B16) — Severity/Effort/Dependencies for every entry
> Method: Side-by-side comparison of actual Old source vs Reborn implementation

---

## B1. Player Flow (Alur Utama)

```
LAYER: PLAYER FLOW
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Old File / Screen             Reborn File                 Status  Sev  Effort   Dependencies
─────────────────────────────────────────────────────────────────────────────────────────────
[Client]Launcher/              client/ui/screens/          ✅      -    -        -
MainDialog.cpp                LauncherScreen.cpp
  • FTP patch download        PatchSystem.cpp              ✅
  • HTML notice view          NoticeView.cpp               ✅
  • BGM playback              AudioManager.cpp             ✅
  • Auto-update check         PatchSystem.cpp              ✅

[Client]LUNA/LoginDlg         client/ui/screens/          ✅      -    -        -
→ WebLauncherIDPass.bin       LoginScreen.cpp
  • ID/Password input         InputField widget             ✅
  • Server selection          ServerInfo table              ✅
  • Save ID checkbox           (not implemented)            🔴     L    4 hrs    CheckBox widget
  • Version check             client_version field          ✅

CharSelect.bin →              client/ui/screens/          ✅      -    -        -
CharSelectScreen.cpp          CharSelectScreen.cpp
  • Character list grid       CharacterSlot widget          ✅
  • Character create          CharMakeDlg.cpp               ✅
  • Character delete          CharSelectScreen.cpp          ✅
  • 3D preview render         CharacterRenderer.cpp         ✅

GameIn.cpp →                  client/ui/screens/          🟡     M    3 days   CombatSystem
GameWorld flow                GameScreen.cpp                Partial
  • Hero spawn + init         Hero.cpp                      ✅
  • World rendering            WorldRenderer.cpp             ✅
  • Combat input              CombatSystem.cpp              🟡  Formula mismatch
  • Social (party/chat)       PartyDialog, ChatPanel         ✅
  • Minimap display           MiniMapDlg.cpp                ✅
  • Auto-attack combo         ComboSystem.cpp               ✅
  • PK mode                   PKManagerDlg.cpp              ✅

SUMMARY - PLAYER FLOW
  Total screens:               5
  Fully implemented:           5 (100%)
  Missing sub-features:        1 (Save ID checkbox)
  Critical issues:             Combat formulas need alignment (see B3)
  Total estimated effort:      ~1 week
```

---

## B2. UI System (Window/Dialog)

Old: 213 .bin files di `LunaPlus/Data/Interface/Windows/`
Reborn: 59 C++ dialog classes di `client/ui/dialogs/`

```
LAYER: UI SYSTEM
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Old .bin File             Reborn C++ Class                Status  Sev  Effort    Dependencies
──────────────────────────────────────────────────────────────────────────────────────────────
WEB LAUNCHER / LOGIN
WebLauncherIDPass.bin      LoginScreen.cpp                 ✅
IDPass.bin                 LoginScreen.cpp                 🟡     L    1 day     -
ServerSelect.bin           LoginScreen.cpp                 ✅

CHARACTER
CharSelect.bin             CharSelectScreen.cpp            ✅
CharMake.bin               CharMakeDlg.cpp                 ✅
CharMakeProfile.bin        CharMakeDlg.cpp                 ✅
CharInfo.bin               CharacterDialog.cpp             🟡     L    1 day     -
CharDel.bin                (not implemented)               🔴     L    4 hrs     -
CharGage.bin               (not implemented)               🔴     L    4 hrs     -
ChangeClass.bin            ClassAdvancement.hpp            ✅
Changejob.bin              ChangeJobDialog.cpp             ✅
ChangeNameDlg.bin          (not implemented)               🔴     L    4 hrs     -
BodyChangeDialog.bin       CostumeDialog.cpp               ✅

INVENTORY / ITEMS
Inven.bin                  InventoryDialog.cpp             ✅
Storage.bin                StorageDialog.cpp               ✅
Bank.bin                   StorageDialog.cpp               ✅
Trade.bin                  TradeDialog.cpp                 ✅
EnchantDialog.bin          UpgradeDialog.cpp                🟡     M    1 day     -
ReinforceDialog.bin        UpgradeDialog.cpp                ✅
MixDialog.bin              MixDialog.cpp                   ✅
ComposeDialog.bin          ComposeDialog.cpp               ✅
DissolveDialog.bin         (not implemented)               🔴     L    1 day     -
IdentificationDlg.bin      (not implemented)               🔴     M    2 days    -
ItemShopDlg.bin            CashShopDialog.cpp               ✅
ItemMallWarehouse.bin      (not implemented)               🔴     M    2 days    -
ItemPopupDlg.bin           (not implemented)               🔴     L    4 hrs     -

SOCIAL
Guild.bin                  GuildDialog.cpp                 ✅
GuildCreate.bin            GuildDialog.cpp                 🟡     L    1 day     -
GuildInvite.bin            GuildDialog.cpp                 🟡     L    4 hrs     -
GuildMark.bin              GuildMarkDialog.cpp             ✅
GuildWarehouse.bin         GuildWarehouseDialog.cpp         ✅
Guildbank.bin              GuildBankDialog.cpp              ✅
GuildWarInfo.bin           GuildWarInfoDialog.cpp           ✅
GuildRank.bin              (not implemented)               🔴     L    1 day     -
Family.bin                 FamilyDialog.cpp                 ✅
FamilyCreate.bin           FamilyDialog.cpp                 ✅
FamilyInvite.bin           FamilyDialog.cpp                 ✅
FamilyMark.bin             FamilyMarkDialog.cpp             ✅
Friend.bin                 FriendDialog.cpp                 ✅
PartyCreate.bin            PartyDialog.cpp                  ✅
PartyInvite.bin            PartyDialog.cpp                  ✅
PartySet.bin               PartySetDialog.cpp               ✅
PartyMatchingDlg.bin       (not implemented)               🔴     L    2 days    -
PartySeekMemberDlg.bin     (not implemented)               🔴     L    1 day     -

CHAT / COMMUNICATION
ChatWindow.bin             ChatPanel.cpp                   ✅
ChatRoomCreateDlg.bin      ChatRoomDialog.cpp              ✅
ChatRoomJoinDlg.bin        ChatRoomDialog.cpp              ✅
ChatRoomDlg.bin            ChatRoomDialog.cpp              🟡     M    1 day     -
Chat_FaceList.bin          (not implemented)               🔴     L    1 day     -
ShoutDlg.bin               (not implemented)               🔴     L    1 day     -

NPC / QUEST
NPCChat.bin                NPCDialog.cpp                   ✅
NPCShop.bin                NPCDialog.cpp (partial)         🟡     H    2 days    -
NPCNoticeDlg.bin           (not implemented)               🔴     L    4 hrs     -
NpcImage.bin               NpcImageDialog.cpp              ✅
Quest.bin                  QuestDialog.cpp                 ✅
QuestQuickView.bin         (not implemented)               🔴     L    1 day     -

GAMEPLAY
Target.bin                 TargetDialog.cpp                ✅
TargetMonster.bin          TargetMonsterDialog.cpp          ✅
TargetCharacter.bin        TargetCharacterDialog.cpp        ✅
TargetBoss.bin             (not implemented)               🔴     L    4 hrs     -
MonsterKill.bin            (not implemented)               🔴     L    4 hrs     -
PKLootDlg.bin              PKManagerDlg.cpp                ✅
MiniMap.bin                MiniMapDlg.cpp                  ✅
BigMap.bin                 WorldMapDlg.cpp                 ✅
BattleGuage.bin            (not implemented)               🔴     L    4 hrs     -
QuickSlot.bin              (not implemented)               🔴     L    1 day     -
ProgressDialog.bin         (not implemented)               🔴     L    4 hrs     -

SYSTEM
Menu.bin / Menu0.bin       MainMenuDialog.cpp              🟡     M    2 days    -
Option.bin                 OptionsDialog.cpp               ✅
Optiondlg.bin              (not implemented)               🔴     M    1 day     -
KeySetting.bin             KeyBindDialog.cpp               ✅
HelpDlg.bin                HelpDialog.cpp                  ✅
GameOut.bin                GameOutDialog.cpp               ✅
FadeDlg.bin                FadeDlg.cpp                     ✅
NewLoadDlg.bin             LoadingScreen.cpp               ✅
LoadingMsg.bin             LoadingScreen.cpp               ✅
Revival.bin                RevivalDialog.cpp               ✅
SystemMsg.bin              (not implemented)               🔴     L    4 hrs     -
Channel.bin                (not implemented)               🔴     L    1 day     -

FISHING / PET / HOUSING
FishingDlg.bin             FishingDialog.cpp               ✅
FishingPointDlg.bin        FishingDialog.cpp               ✅
FishingProgressDialog.bin  FishingDialog.cpp               ✅
PetInfo.bin                PetDialog.cpp                   ✅
PetUI.bin                  PetDialog.cpp                   ✅
PetresDialog.bin           (not implemented)               🔴     L    1 day     -
HousingMainPointDlg.bin    HousingDialog.cpp               ✅
HousingWarehouseDlg.bin    HousingDialog.cpp               ✅
HousingWebDlg.bin          (not implemented)               🔴     L    2 days    -
HouseNameDlg.bin           (not implemented)               🔴     L    1 day     -

FARM / SIEGE / WEATHER
FarmManage.bin             FarmSystem.cpp                  ✅
Farm_Get.bin               FarmGetDialog.cpp               ✅
Farm_Upgrade.bin           FarmUpgradeDialog.cpp           ✅
FarmAnimalCageDlg.bin      (not implemented)               🔴     L    1 day     -
SiegeWarFlagDlg.bin        SiegeSystem.cpp                 🟡     M    2 days    -
WeatherDlg.bin             WeatherDialog.cpp               ✅
DateMatchingDlg.bin        DateMatchingDialog.cpp          ✅

SKILL
Skill.bin                  SkillDialog.cpp                 ✅
SkillTrn.bin               SkillTrnDialog.cpp              ✅

OTHER
MacroDialog.bin            MacroDialog.cpp                 ✅
TutorialDlg.bin            TutorialDlg.cpp                 ✅
CookingDlg.bin             CookingDialog.cpp               ✅
Consignment.bin            ConsignmentDialog.cpp           ✅
ConsignmentCategory.bin    ConsignmentDialog.cpp           🟡     L    1 day     -
Consignment_Guide.bin      (not implemented)               🔴     L    4 hrs     -
StreetStall* (3 files)     StreetStallDialog.cpp            ✅
WeatherDlg.bin             WeatherDialog.cpp                ✅
BillingDlg.bin             (not implemented)               🔴     L    1 day     -
DoorSettingDlg.bin         (not implemented)               🔴     L    4 hrs     -
AllyNote.bin               MailDialog.cpp                  🟡     L    1 day     -

SUMMARY - UI SYSTEM
  Total Old dialog .bin:      213
  Reborn C++ classes:         59 (27.7%)
  Fully functional:           50 (23.5%)
  Missing (critical):         10 (NPCShop layout, ItemMallWarehouse, Identification, 
                                  Enchant, SiegeWarFlag, Dissolve, Progress, 
                                  PetRes, QuickSlot, Channel)
  Missing (minor):            ~20
  Total estimated effort:     ~4 weeks (1 FTE)
```

---

## B3. Gameplay Constants & Hero System ← 🔴 KRITIKAL — FORMULA SALAH

Hasil scan langsung `[Client]LUNA/Hero.cpp` dan `[Server]Map/AttackManager.cpp` menunjukkan bahwa formula yang di-adaptasi sebelumnya memiliki **perbedaan signifikan** dengan Old.

```
LAYER: GAMEPLAY / HERO COMBAT
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Old Function (Hero.cpp)      Reborn Equivalent             Status  Sev  Effort   Dependencies
─────────────────────────────────────────────────────────────────────────────────────────────
─ PHYSICAL ATTACK ─────────────────────────────────────────────────────────────────────────
DoGetAttackRate() →          CombatSystem::                🔴     C    3 days   CombatSystem  
PhysicAttack                  CalculateDamage()              FORMULA SALAH        refactor
  OLD: (WeaponAtk+Level)*(1+STR/1000)+(RealSTR-30)
       × enchant bonus × buff %
  REBORN: atk * skill_power − def * 0.5
  IMPACT: Base damage Old vs Reborn berbeda secara fundamental

─ CRITICAL RATE ────────────────────────────────────────────────────────────────────────────
DoGetCriticalRate()           CombatSystem::                🔴     C    2 days   -
  OLD: 45 * DexRate + (BaseDEX - 25) / 5 + buffs
       DexRate = max(DEX−BaseDEX,0) / ((Level−1)*5)
       Ini adalah formula bertingkat, bukan linear!
  REBORN (SALAH): DEX / 1000  (linear sederhana)
  IMPACT: Crit rate Old sangat tergantung level dan class.
          Di level rendah, crit Old bisa 0%.
          Reborn memberi crit bahkan di level 1.

─ BLOCK RATE ───────────────────────────────────────────────────────────────────────────────
CPlayer::GetBlock()           CombatSystem::                🔴     C    2 days   -
  OLD: DEX / 27 + ClassBonus + buffs
       ClassBonus: Fighter=15, Rogue=10, Ranger=5, Mage=9
  REBORN (SALAH): CON / 2000
  IMPACT: Block Old berdasarkan DEX + class, bukan CON.

─ ACCURACY / MISS ──────────────────────────────────────────────────────────────────────────
DoGetAccuracyRate() +         CombatSystem::                🔴     C    3 days   -
DoGetEvasionRate()             CalculateDamage()
  OLD: ACC = 85 + Accuracy − Evasion
       Accuracy = (DexRate*0.6 + (BaseDEX−30)/1000 + Level*ClassVal/15000) *100
       Evasion = formula identik dengan Avoid buffs
       if ACC < rand(100)+1 → MISS
  REBORN (SALAH): 1% fixed miss rate
  IMPACT: Old punya accuracy/evasion system penuh, bukan flat miss.

─ MAGIC ATTACK ─────────────────────────────────────────────────────────────────────────────
DoGetMagAttackRate()          CombatSystem::                🔴     C    2 days   -
  OLD: (MagicWeapon+Level)*(1+INT/1000)+(RealINT−40)
       × enchant bonus × buff %
  REBORN: (tidak ada magic attack terpisah)
  IMPACT: Magic attack system belum diimplementasi.

─ DEFENSE ──────────────────────────────────────────────────────────────────────────────────
DoGetDefenseRate()            CombatSystem::                🟡     M    2 days   -
  OLD: (ArmorDef+Level)*(1+VIT/3000)+(RealVIT−40)/5
       × enchant × buff %
  REBORN: physic_defense (flat value)
  IMPACT: Defense growth berbeda.

─ LEVEL PENALTY ────────────────────────────────────────────────────────────────────────────
  OLD: -1.5% per level (hanya jika atk < def)
       damage *= 1 + (atkLvl − defLvl) * 0.015
       (HANYA jika attacker level LEBIH RENDAH)
  REBORN: ±5% per level, cap 50% (dua arah)
  IMPACT: Old hanya menghukum level rendah, bukan boost level tinggi.

─ PvP / GT DAMAGE ─────────────────────────────────────────────────────────────────────────
  OLD: PvP = damage * 35%  (reduced to 35%)
       GT  = damage * 10%  (Guild Tournament)
  REBORN: (not implemented)
  IMPACT: PvP damage reduction belum ada.

─ ELEMENT SYSTEM ────────────────────────────────────────────────────────────────────────────
  OLD: Tidak ada elemental advantage dalam damage formula.
       Element attributes exist on items but are not used in CalcDamage.
  REBORN: GetElementAdvantage() dengan 7-element cycle
  IMPACT: Reborn menambahkan sistem elemen yang TIDAK ADA di Old.

─ AGGRO SYSTEM ────────────────────────────────────────────────────────────────────────────
AttackManager:PhysicAttack()  AISystem + ThreatTable         🟡     M    3 days   -
  OLD: aggroNum >= 3 → Avoid *= 1−(aggroNum−2)^2*0.01
       aggroNum >= 6 → Defense *= 1−(aggroNum−5)^1.5*0.01
  REBORN: ThreatTable (additive hate, top threat)
  IMPACT: Old menggunakan aggro PENALTY (semakin banyak aggro, 
          semakin turun avoid/defense). Reborn menggunakan additive threat.

─ COMBO SYSTEM ────────────────────────────────────────────────────────────────────────────
CHero::GetCurComboNum()       ComboSystem.cpp                🟡     M    2 days   -
  OLD: MAX_COMBO_NUM auto-attack chain
       Skill COMBO integration (SKILL_COMBO_NUM)
  REBORN: ComboSystem dengan stages (5/10/20/50)
  IMPACT: Detail konstanta combo perlu diverifikasi.

─ BUFF SYSTEM ──────────────────────────────────────────────────────────────────────────────
CHero::ProcessSpecialSkill()  BuffSystem.cpp                 🟡     M    3 days   -
  OLD: Conditional buffs (if stat > threshold → activate)
       ProcessSpecialSkill() dievaluasi tiap frame
  REBORN: Basic buff system
  IMPACT: Conditional buff activation belum diimplementasi.

─ HEAL FORMULA ─────────────────────────────────────────────────────────────────────────────
RecoverLife()                 (not ported)                   🔴     M    3 days   CombatSystem
  OLD: heal = ((Wis*11 + Int*4 + Level*20) * SkillFactor) / 800 + 100
       GT: heal *= 0.5
       Aggro: heal/3
  REBORN: (missing)

─ MOVEMENT + PATHFINDING ──────────────────────────────────────────────────────────────────
CHero::Move_UsePath()         NavMeshSystem.cpp              🟡     M    1 week   NavMesh
  OLD: Direct line check → fallback A* via CWayPointManager
  REBORN: NavMeshSystem (basic)
  IMPACT: A* pathfinding perlu diimplementasi.

─ PK MODE ──────────────────────────────────────────────────────────────────────────────────
CBattle::IsEnemy()            PKManagerDlg.cpp               🟡     M    2 days   -
  OLD: IsEnemy checks party, guild, guild union, 
       siege mode, PK mode, guild war
  REBORN: Basic PK mode
  IMPACT: PK alliance/enemy relationships kompleks.

─ ATTACK SPEED / CASTING SPEED ─────────────────────────────────────────────────────────────
DoGetAttackSpeedRate()        (not implemented)              🔴     M    2 days   -
DoGetCastingSpeedRate()       (not implemented)
  OLD: AttackSpeed = PhysicSkillSpeedRate (from passives+buffs)
       CastingSpeed = MagicSkillSpeedRate

SKILL DAMAGE MODIFIER
  OLD: add_type 1 (STR-based):  (1000+AddDamage+STR)/1000
       add_type 2 (Weapon-based): (1000+AddDamage+Weapon)/1000
       rate_add: damage *= (1 + rate/100)
       plus_add: damage += plus
  REBORN: Sudah sesuai ✅

SUMMARY - GAMEPLAY CONSTANTS ← KRITIKAL
  Total Hero.cpp functions:     ~60+
  Formula yang benar:           3/12 (25%)
  Formula yang SALAH:           9/12 (75%)
    - Crit rate:      ❌ DEX/1000 → harusnya complex level-based
    - Block rate:     ❌ CON/2000 → harusnya DEX/27 + ClassBonus
    - Miss/accuracy:  ❌ 1% fixed → harusnya accuracy−evasion system
    - Base damage:    ❌ atk−def/2 → harusnya (Weapon+Lvl)*(1+STR/1000)
    - Defense:        ❌ flat → harusnya (Armor+Lvl)*(1+VIT/3000)
    - Level penalty:  ❌ ±5% dua arah → harusnya -1.5% satu arah
    - PvP damage:     ❌ missing → harusnya 35% reduction
    - Element:        ❌ ada → harusnya tidak ada
    - Heal formula:   ❌ missing
  Total estimated effort:       ~3 weeks (1 FTE)
```

---

## B4. Network Protocol

Old: 55 MP_PROTOCOL enums, ~2034 individual packet types, struct definitions di CommonStruct.h
Reborn: 30 .fbs files, 246 PacketType entries

```
LAYER: NETWORK PROTOCOL
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Old Category              Old Structs     Reborn .fbs            Status  Sev  Effort   Dependencies
─────────────────────────────────────────────────────────────────────────────────────────────
MP_USERCONN               50+ structs     Login.fbs,               🟡    H    2 wks   FlatBuffers
                                           Character.fbs,
                                           MapChange.fbs
  └─ LOGIN_SYN: id[21], pw[21],          LoginRequest: username,
     version[16], securityPW[17],         password_hash, client_version,
     loginKey[MAX_PATH], AuthKey          file_crc, client_timestamp,
                                           mac_address
     → Field mismatch: Old punya AuthKey, securityPW, loginKey
       yang tidak ada di Reborn. Reborn tambah mac_address.

MP_MOVE                   18 structs      Movement.fbs             ✅    -    -        -
  └─ MOVE_ONETARGET: dwMoverID,          MoveRequest: direction,
     sPos/tPos (COMPRESSEDPOS),          target_position, move_mode,
     AddedMoveSpeed                      kyung_gong_idx, added_move_speed
     → Good match ✅

MP_ITEM                   50+ structs     Inventory.fbs             🟡    M    1 week   -
  └─ ITEM_MOVE_SYN: FromPos,             InventoryMoveRequest:
     wFromItemIdx, ToPos, wToItemIdx     src_slot, dst_slot,
     → Good match ✅                       src_storage_type,
     └─ ENCHANT_SYN: ItemPos, ItemIdx,    dst_storage_type,
        MaterialItemPos,                  item_id, count
        MaterialItemIdx, mProtectionItem
     → No Reborn equivalent for enchant/mix/reinforce

MP_CHAT                   20+ structs     Chat.fbs                 ✅    -    -        -
  └─ Good coverage. Chat.fbs handles
     Whisper, Party, Guild, Family, Shout ✅

MP_PARTY                  18 structs      Party.fbs                ✅    -    -        -
  └─ Good coverage ✅

MP_GUILD                  30+ structs     Guild.fbs                🟡    M    3 days   -
  └─ GUILD_CREATE_SYN: GuildName[33],    GuildCreateRequest:
     Intro[128]                           character_id, guild_name
  └─ GUILD_INFO: GUILDINFO +             GuildInfo: guild_id, name,
     GUILDMEMBERINFO[]                    level, gp, guild_point,
     → Field match ✅                      guild_mark, master_id,
                                          members[]

MP_QUEST                  20 structs      Quest.fbs                ✅    -    -        -
  └─ Good coverage ✅

MP_SKILL                  17 structs      Skill.fbs                🟡    M    3 days   -
  └─ SKILL_START_SYN: SkillIdx,          SkillCastRequest:
     Operator, SKILLOBJECT_INFO,          caster_id, skill_id,
     MainTarget, TargetList               target_id, position
     → Simplified: Old punya skill objects,
       target list, direction, consume item

MP_NPC                    12 structs      NPC.fbs + Quest.fbs     🟡    M    3 days   -
  └─ NPC_SPEECH_SYN: Name (NPC name)     NpcRequest: npc_id,
     → Reborn sudah extended dengan        character_id, action,
       NpcAction enum                      quest_id

MP_VEHICLE                19 structs      Vehicle.fbs              ✅    -    -        -
  └─ Full coverage ✅

SUMMARY - NETWORK PROTOCOL
  Old packet types:            ~2034 total, 55 MP_PROTOCOL enums
  Reborn PacketType entries:   246 (12% of Old)
  Reborn .fbs schemas:         30 files
  10 key categories:           ~70% field match
  Critical gaps:               Login fields (AuthKey, securityPW),
                               Enchant/Mix/Reinforce item schemas,
                               Skill object system
  Total estimated effort:      ~5 weeks (2 FTE)
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
.eft (effect)    EftParser.cpp    ✅      -    -        -
.spr (sprite)    UiAtlasRegistry  🟡      M    2 days   -
.wav / .mp3      .wav / .mp3      ✅      -    -        miniaudio
.bin (UI script) UiScriptParser   🟡      M    5 days   -
.bin (string)    UiStringTable    🟡      M    3 days   -

SUMMARY - ASSET PIPELINE
  Status: 90% complete. Eft parser ✅, Spr atlas 🟡
  Total: ~1 week remaining
```

---

## B6. Server Systems

Old: 3 server types (Agent 27 files, Map 200+ files, Distribute 16 files)
Reborn: 3 server types (Agent 10 files, Map ~20 files + 12 systems, Distribute 6 files)

```
LAYER: SERVER SYSTEMS
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Old Server Subsystem      Old File Path           Reborn Equivalent       Status  Sev  Effort
─────────────────────────────────────────────────────────────────────────────────────────────
AGENT SERVER
Auth/Login                AgentNetworkMsgParser   AgentServer.cpp          ✅
Character list/CRUD       AgentDBMsgParser        AgentServer.cpp          ✅
Session mgmt              UserTable               SessionManager.cpp       ✅
Gift manager              GiftManager.cpp         GiftManager.cpp          ✅
Punish manager            PunishManager.cpp       PunishManager.cpp        ✅
HackShield                HackShieldManager       (removed)                🔴    H    -
NProtect                  NProtectManager         (removed)                🔴    H    -
NpcRecallMgr              NpcRecallMgr.cpp        (not ported)             🔴    L    2 days
PlustimeMgr               PlustimeMgr.cpp         (not ported)             🔴    L    2 days

MAP SERVER
Combat/Attack             AttackManager.cpp       CombatSystem.cpp         🟡    C    3 wks
Item management           ItemManager.h/cpp       ItemSystem.cpp           🟡    M    1 week
Party management          PartyManager.cpp        PartySystem.cpp          ✅
Guild management          GuildManager.h/cpp      GuildSystem.cpp          ✅
Quest system              Quest.cpp/cpp           QuestSystem.cpp          ✅
AI system                 AISystem.cpp            AISystem.cpp             ✅
Monster respawn           RegenManager.cpp        SpawnSystem.cpp          ✅
Movement                  MoveManager.cpp         MovementSystem.cpp       🟡    M    1 week
Looting                  LootingManager.cpp      (not ported)             🔴    M    3 days
Exchange/Trade           ExchangeManager.cpp     TradingSystem.cpp        🟡    M    3 days
Siege warfare            SiegeWarfareMgr.cpp     SiegeSystem.cpp          🟡    M    2 wks
Dungeon                  Dungeon/ folder         DungeonSystem.cpp        🟡    M    1 week
Grid system              Grid.cpp                GridSystem.cpp           🟡    M    3 days
Trigger                  Trigger/ folder         TriggerSystem.cpp        🟡    M    3 days
FSM (AI state machine)   Finite State Machine/   FSMEngine.cpp            🟡    M    2 wks
Field Boss               FieldBossMonsterMgr     (not ported)             🔴    M    1 week
Boss Monster             BossMonsterInfo.h       (not ported)             🔴    M    1 week
Recall system            MoveManager(recall)     (in MovementSystem)      🟡    M    3 days
UserTable (entity mgmt)  UserTable.cpp           (in MapServer.cpp)       🟡    L    3 days
Script system            SetScript/*.bin         MapScriptRuntime.cpp     🟡    M    2 wks

DISTRIBUTE SERVER
Channel management       ServerSystem.cpp        ChannelManager.cpp       ✅
Chat room                ChatRoomMgr.cpp         ChatRoomDialog.cpp       ✅
Billing                  BillConnector.cpp       (not ported)             🔴    L    1 week

SUMMARY - SERVER SYSTEMS
  Agent: ~80% ported (missing: NpcRecallMgr, PlustimeMgr)
  Map: ~50% ported (12 of ~25 subsystems done)
  Distribute: ~70% ported (missing Billing)
  Total estimated effort: ~8 weeks (2 FTE)
```

---

## B7. Database Schema

Old: 0 SQL files (SQLite via C++), ~30 tables di `luna_game.db`
Reborn: `schema_game_sqlite.sql` dengan 75 tables

```
LAYER: DATABASE SCHEMA
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Old Table (luna_game.db)    Reborn Table (SQLite)           Status  Sev  Effort
─────────────────────────────────────────────────────────────────────────────────────────────
TB_CHARACTER                TB_CHARACTER (51 cols)           ✅
  • char_id, account_id      • CharacterIdx, AccountID       ✅
  • char_name                • CharName UNIQUE               ✅
  • level, exp               • Level, Exp                    ✅
  • money                    • Money                         ✅
  • map_id, pos_x/y/z        • MapIdx, PosX/Y/Z              ✅
  • hp, mp, sp               • HP, MP, SP                    ✅
  • str, dex, vit, int, wis  • Str, Dex, Con, Int, Wis      ✅
  • job, face, hair          • Job, Face, Hair               ✅
  • create_date, delete_     • CreateDate, DeleteDate        ✅
  • last_login, last_logout  • LastLogin, LastLogout         ✅
  • play_time                • PlayTime                      ✅
  • pk_kill/death            • PvpKill/DeathCount            ✅
  • fame, bad_fame           • Fame, BadFame                 ✅
  • stat/skill_point         • Stat/SkillPoint               ✅
  • honor_point              • HonorPoint                    ✅
  • rest_exp                 • RestExp                       ✅
  • battle_style             • BattleStyle                   ✅
  • pvppoint                 • PvpPoint                      ✅
  • server_group, channel    • ServerGroup, ChannelIdx       ✅
  • attribute                • Attribute                     ✅
  • (tidak ada)              • Luck                          🟢  Added
  • (tidak ada)              • LoginTime                     🟢  Added
  • (tidak ada)              • InventoryExpansion            🟢  Added

TB_ITEM → TB_ITEM            Full match                     ✅
TB_SKILL → TB_SKILL          Full match                     ✅
TB_QUEST → TB_QUEST          Full match                     ✅
TB_PARTY → TB_PARTY +        Full match                     ✅
TB_GUILD → TB_GUILD +        Full match + EmblemData        ✅
TB_FRIEND → TB_FRIEND        Full match + Memo              ✅
TB_FARM → TB_FARM +          Full match + grade columns     ✅
  TB_FARM_CROP/ANIMAL
TB_HOUSE → TB_HOUSE +        Full match + furniture         ✅
  TB_HOUSE_FURNITURE
TB_VEHICLE → TB_VEHICLE +    Full match + passenger         ✅
TB_PET → TB_PET              Full match                     ✅
TB_FISHING → TB_FISHING      Full match                     ✅
TB_STORAGE → TB_STORAGE      Full match                     ✅
TB_CONSIGNMENT →             Full match + Auction           ✅
  TB_CONSIGNMENT + TB_AUCTION
TB_NOTE → TB_NOTE            Full match                     ✅
TB_SIEGE_RECALL →            Full match                     ✅
  TB_SIEGE_RECALL
TB_SIEGE_WARFARE →           Full match                     ✅
  TB_SIEGE_WARFARE
TB_PUNISHLIST → TB_PUNISHLIST  Full match                   ✅
TB_NPC_RECALL → TB_NPC_RECALL  Full match                   ✅
(not in Old)                 TB_RANKING, TB_BILLING,         🟢  Added
                               TB_CHALLENGEZONE,
                               TB_GUILD_TOURNAMENT,
                               TB_GUILD_WAR,
                               TB_DUNGEON, TB_TRIGGER,
                               TB_MONSTERMETER, dll.

SUMMARY - DATABASE SCHEMA
  Old tables: 30 → Reborn: 75 (250% coverage)
  Columns match: 100%
  Missing Old tables: None
  Added tables: 45 baru (content data, ranking, billing, tournament, dll)
  Total: ✅ Complete
```

---

## B8. Build System

```
LAYER: BUILD SYSTEM
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Old (VS2003)                  Reborn (CMake)             Status  Sev  Effort
─────────────────────────────────────────────────────────────────────────────────────────────
Compiler: MSVC 7.1            AppleClang 16               ✅
Renderer: DirectX 9 SDK       bgfx (Metal)                ✅
Window: Win32 + MFC           GLFW                        ✅
Audio: Miles Sound System     miniaudio                   ✅
Physics: Proprietary          Jolt Physics (optional)     🟡     L    1 week
Script: Lua (custom host)     sol2/luajit                 ✅
Database: MSSQL + ADO         SQLite3 + PostgreSQL        ✅
UI: cWindowManager            WindowManager+UIRenderer    ✅
Anti-cheat: HackShield+nProt  (not ported)                🔴     H    -

Build targets:
  LunaPlusClient.exe          luna-plus-client             ✅
  LunaPlusAgent.exe           luna-plus-agent              ✅
  LunaPlusMap.exe             luna-plus-map                ✅
  LunaPlusDistribute.exe      luna-plus-distribute         ✅
  Launcher.exe                (built into client)          ✅
  MapEditor.exe               (skipped — needs imgui)      🟡     M    1 week
  PackingTool.exe             (not ported)                 🔴     L    1 week
  NewPackingTool.exe          (not ported)                 🔴     L    1 week
  ModelView.exe               chx_to_gltf                  ✅

SUMMARY - BUILD SYSTEM
  Status: 90% complete. 13/13 binaries compile clean.
  Total: ~1 week remaining
```

---

## B9. Error Handling & Edge Cases

```
LAYER: ERROR HANDLING
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Error Case                    Old Handling                Reborn Handling        Status  Sev  Effort
────────────────────────────────────────────────────────────────────────────────────────────────────
Network disconnect            Reconnect dialog 10s        ReconnectHandler       🟡     M    3 days
Packet corruption             MSGROOT.CheckSum + Crypt    FlatBuffers verifier   🟡     L    1 day
Asset load failure            Fallback texture + log      Texture fallback       🟡     M    1 day
DB query failure              Error code + retry          Basic error return     🟡     M    2 days
Login failure                 MP_USERCONN_LOGIN_NACK      LoginResult enum       ✅
                              dengan dwData (error code)
Character load failure        Retry + error dialog        Basic                  🟡     M    1 day
Character delete failure      NACK dengan reason code     Basic                  🟡     M    1 day
File not found                Fallback asset + dialog     VFS fallback           🟡     M    2 days
Cheat detection               HackShield + NProtect       Server validation      🔴     H    2 wks
Overlapped login              MP_USERCONN_NOTIFY_         SessionManager check   🟡     M    1 day
                              OVERLAPPEDLOGIN
Battle/combat error           RESULTINFO dengan kode      Basic                  🟡     M    2 days
Item use error                MP_ITEM_ERROR_NACK          Basic NACK             🟡     M    1 day

SUMMARY - ERROR HANDLING
  Old punya sistem error codes yang detail untuk setiap layer
  Reborn masih basic (return true/false)
  Total: ~3 weeks
```

---

## B10. Performance Comparison

```
LAYER: PERFORMANCE
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Benchmark                     Old (DX9)                    Reborn (bgfx Metal)   Status
─────────────────────────────────────────────────────────────────────────────────────────────
Map load (map 51)             ~1.2s (PAK file)             ~0.3s (GLB)            ✅ 4x faster
Login screen FPS              60 FPS (capped)              1800 FPS (no cap)      🟡 Need cap
Character render              DX9 60fps @ 20 chars         bgfx ~120fps           ✅ 2x faster
Memory usage (idle)           ~300 MB                      ~150 MB                ✅ 50% less
Texture memory                ~200 MB (DDS)                ~100 MB (PNG/BCn)      ✅ 50% less

SUMMARY - PERFORMANCE
  Reborn 2-4x faster. Need frame cap.
```

---

## B11. Security & Anti-Cheat

```
LAYER: SECURITY & ANTI-CHEAT
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Aspect                        Old                          Reborn                  Status
─────────────────────────────────────────────────────────────────────────────────────────────
Authentication                Plaintext PW                  bcrypt + AES-GCM        ✅
Packet encryption             XOR (conditional via Crypt)   AES-GCM                 ✅
Anti-cheat client             HackShield (kernel) +          (none)                  🔴
                              NProtect (ring0)
Anti-cheat server             ScriptCheckValue +            Server validation       🟡
                              MP_NPC_CHECK_HACK_SYN         (basic, not complete)
SQL injection                 Concatenation                 Prepared statements     ✅
Memory protection             CRC check                     (none)                  🔴
Cheat commands (GM)           MP_CHEAT protocol             (not ported)            🔴
Rate limiting                 (basic)                       RateLimiter             ✅
Brute-force protection        (basic)                       AgentServer built-in    🟡
Overlapped login detection    MP_USERCONN_NOTIFY_           SessionManager          ✅
                              OVERLAPPEDLOGIN
Item duplication check        IsDupItem() + DB query        (basic)                 🟡
Speed hack check              MP_NPC_CHECK_HACK_SYN +       ValidationSystem        🔴
                              periodic position validation  (not wired)

SUMMARY - SECURITY
  HackShield/NProtect tidak bisa di-port (kernel-level Windows driver).
  Mitigasi pengganti (server validation, rate limiting) baru partial.
  Total: ~3 weeks
```

---

## B12. Concurrency & Threading

```
LAYER: CONCURRENCY & THREADING
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Aspect                        Old                          Reborn                  Status
─────────────────────────────────────────────────────────────────────────────────────────────
Network I/O model             IOCP (I/O Completion Ports)   asio::io_context        ✅
                               4 worker threads              4 worker threads
Thread pool                   DB thread pool (3-5 threads)  asio::thread_pool       🟡
Shared state protection       CRITICAL_SECTION + Mutex      std::mutex + atomic     ✅
Render thread                 Single (Win32 msg loop)       Single (GLFW loop)      ✅
Deadlock risk                 Manual CS ordering            RAII std::lock_guard    ✅
Job system                    Custom thread pool            asio::post              🟡
DB access                     MSSQL connection pool         SQLite WAL (single-w)  🟡

SUMMARY - CONCURRENCY
  Good parity. SQLite single-writer adalah bottleneck vs MSSQL thread pool.
  Total: ~1 week (PostgreSQL migration)
```

---

## B13. Localization (i18n)

```
LAYER: LOCALIZATION (i18n)
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Aspect                        Old                          Reborn                  Status
─────────────────────────────────────────────────────────────────────────────────────────────
String table                  InterfaceMsg.bin (KR/EN/      UiStringTable            🟡
                              JP/CN) ~2000+ strings         1624 strings loaded
Language switching            Runtime change (KR/EN/JP/CN)  Localization::           🟡
                                                            SetLanguage (basic)
Font support                  korean.ttf, english.ttf,      2002_EYA.ttf (single)   🔴
                              japanese.ttf, chinese.ttf     No CJK support
UI scripts per language       Separate .bin per language    Single .bin.txt          🔴
Regional formats              Date/number formatting        Basic                    🟡
Encoding                      CP949 / EUC-KR                UTF-8                    ✅

SUMMARY - LOCALIZATION
  String coverage: ~80%. Font CJK critical.
  Total: ~2 weeks
```

---

## B14. Audio System

```
LAYER: AUDIO (B15)
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Aspect                        Old (Miles SS)               Reborn (miniaudio)      Status
─────────────────────────────────────────────────────────────────────────────────────────────
Audio engine                  Miles Sound System            miniaudio                ✅
Format support                .wav, .mp3, .ogg             .wav, .mp3, .ogg         ✅
BGM crossfade                 Basic crossfade               AudioManager             ✅
3D positional audio           Miles 3D API                  SetSFXPan + attenuation ✅
SFX list loading              SoundList.bin                 UiSoundIndex             ✅
Audio streaming               Streaming from PAK            Streaming from file      ✅

SUMMARY - AUDIO
  ✅ Complete (all features ported by agents #013, #028)
```

---

## B15. Physics & Collision (B16)

```
LAYER: PHYSICS & COLLISION (B16)
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Old Function                      Reborn Equivalent           Status  Sev  Effort
─────────────────────────────────────────────────────────────────────────────────────────────
CollisionTestMovingEllipsoid-     CollisionSystem.hpp          🟡     M    1 week
MeetTriangle                      (basic AABB)
MAP->CollisionLine()              EngineMap collision         ✅
MAP->CollisionCheck_OneLine_New   EngineMap collision         ✅
Gravity / falling                  Basic gravity               ✅
Ragdoll physics                   Jolt Physics (optional)     🔴     L    2 wks
Vehicle physics                   (not implemented)            🔴     M    2 wks

SUMMARY - PHYSICS & COLLISION
  Core collision exists, ragdoll/vehicle physics low priority.
  Total: ~2 weeks
```

---

## Overall Summary

```
GRAND SUMMARY — ALL LAYERS
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Layer               Coverage    Critical Gaps              Total Effort
─────────────────────────────────────────────────────────────────────────────
B1. Player Flow     95%         1 missing feature          1 week
B2. UI System       28%         ~30 missing dialogs        4 weeks
B3. Gameplay        25%         9 formula SALAH            3 weeks ← 🔴
B4. Network         70% (10 cat) Handler wiring            5 weeks
B5. Asset Pipeline  90%         Spr/UI parser polish       1 week
B6. Server Systems  50%         ~13 subsystems missing     8 weeks
B7. Database        100%        None                       -
B8. Build System    90%         3 missing tools            1 week
B9. Error Handling  40%         Detailed error codes       3 weeks
B10. Performance    90%         Frame cap                  <1 day
B11. Security       50%         Anti-cheat mitigation      3 weeks
B12. Concurrency    70%         Job system, DB pool        1 week
B13. Localization   80%         CJK font, ~400 strings     2 weeks
B14. Audio          100%        None                       -
B15. Physics        60%         Ragdoll, vehicle physics   2 weeks
─────────────────────────────────────────────────────────────────────────────
TOTAL:              ~55%       ~33 weeks (1 FTE ~8 months)

⚠️ CRITICAL FINDING: Formula Combat SALAH BESAR
  9 dari 12 formula Old tidak cocok dengan implementasi Reborn.
  Lihat B3 untuk detail dan ADAPTASI_SPEC.md untuk pseudo-code koreksi.
─────────────────────────────────────────────────────────────────────────────
```

---

*End of GAP_ANALYSIS.md v3 — Final, hasil scan langsung Old codebase*

