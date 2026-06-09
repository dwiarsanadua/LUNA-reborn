# PACKET MAPPING — Field-by-Field (10 Packet Categories)

> Old struct (CommonStruct.h / Protocol.h) → Reborn FlatBuffers schema
> Setiap tabel: Old Field | Type (Old) | Reborn Field | FBS Type | Status | Sev | Effort | Deps

---

## 1. LOGIN: MP_USERCONN_LOGIN_SYN → Login.fbs (LoginRequest)

```
PACKET: MP_USERCONN_LOGIN_SYN → Login.fbs (LoginRequest)           [Total effort: 2 days]
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Old Field (MSG_LOGIN_SYN)  Type (Old)    Reborn Field     FBS Type     Status Sev Effort Deps
─────────────────────────────────────────────────────────────────────────────────────────────────────
AuthKey                    DWORD         (implied)         -            🟡     -   -      -
id                         char[21]      username          string       ✅     -   -      -
pw                         char[21]      password_hash     [uint8]      🟡     M   4 hrs  Crypto
Version                    char[16]      client_version    string       ✅     -   -      -
strSecurityPW              char[17]      (missing)          -            🔴     L   1 hr   -
Check.mValue               LONGLONG      (missing)          -            🔴     L   -      Removed
mLoginKey                  char[260]     (missing)          -            🔴     L   -      Removed
(missing)                  -             file_crc          uint32       🟢     -   -      Added
(missing)                  -             client_timestamp  uint32       🟢     -   -      Added
(missing)                  -             mac_address       string       🟢     -   -      Added
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Total: Old 7 field, Reborn 6 field. Match: 3 direct + 3 added + 3 removed + 1 implied.
```

**Old struct:**
```cpp
struct MSG_LOGIN_SYN : public MSGBASE {
    DWORD AuthKey;
    char id[MAX_NAME_LENGTH+1];  // [21]
    char pw[MAX_NAME_LENGTH+1];  // [21]
    char Version[16];
    char strSecurityPW[16+1];
    ScriptCheckValue Check;      // LONGLONG
    char mLoginKey[MAX_PATH];    // [260]
};
```

**Reborn schema:**
```fbs
table LoginRequest {
    username: string (required);
    password_hash: [uint8] (required);
    client_version: string;
    file_crc: uint32;
    client_timestamp: uint32;
    mac_address: string;
}
```

---

## 2. MOVE: MP_MOVE_ONETARGET → Movement.fbs (MoveRequest)

```
PACKET: MP_MOVE_ONETARGET → Movement.fbs (MoveRequest)              [Total effort: 2 days]
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Old Field                  Type (Old)    Reborn Field     FBS Type     Status Sev Effort Deps
─────────────────────────────────────────────────────────────────────────────────────────────────────
dwMoverID                  DWORD         (implied)         -            🟡     -   -      -
sPos.wx                    WORD          direction.x       float        🟡     L   2 hrs  -
sPos.wz                    WORD          direction.z       float        🟡     L   2 hrs  -
tPos.wx                    WORD          target_position.x float        🟡     L   2 hrs  -
tPos.wz                    WORD          target_position.z float        🟡     L   2 hrs  -
MoveMode                   eMoveMode     move_mode         MoveMode     ✅     -   -      -
KyungGongIdx               WORD          kyung_gong_idx    uint16       ✅     M   1 day  Agent #001
AddedMoveSpeed             float         added_move_speed  float        ✅     L   2 hrs  -
(missing)                  -             target_position.y  float        🟢     -   -      Added
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Total: Old 8 field, Reborn 7 field. Match: 7/8 (88%). Added: target_position.y.
NOTE: timestamp exists in MoveResponse and EntityMove, NOT in MoveRequest.
```

---

## 3. COMBAT: MP_SKILL_START_SYN → Combat.fbs (AttackRequest)

```
PACKET: MP_SKILL_START_SYN → Combat.fbs (AttackRequest)             [Total effort: 3 days]
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Old Field (MSG_SKILL_START_SYN) Type (Old) Reborn Field   FBS Type     Status Sev Effort Deps
─────────────────────────────────────────────────────────────────────────────────────────────────────
SkillIdx                   DWORD         skill_id         uint16       🟡     M   2 hrs  -
Operator                   DWORD         (implied)         -            🟡     -   -      -
MainTarget.dwID            DWORD         target_id        uint32       ✅     -   -      -
MainTarget.cpPos           COMPRESSEDPOS position         Vec3         ✅     -   -      -
mConsumeItem               ITEMBASE      (missing)         -            🔴     M   1 day  -
SkillDir                   VECTOR3       (missing)         -            🔴     L   4 hrs  -
TargetList                 CTargetList   (missing)         -            🔴     H   2 days AoE
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Total: Old 7 field, Reborn 3 field. Major simplification — Old punya target list + consume item.
```

---

## 4. SKILL: MP_SKILL_START_SYN → Skill.fbs (SkillCastRequest / SkillCastResult)

Sama dengan COMBAT — Old menggunakan MSG_SKILL_START_SYN untuk kedua skill dan combat.

```
Old Field                  Type (Old)    Reborn Field     FBS Type     Status Sev Effort Deps
─────────────────────────────────────────────────────────────────────────────────────────────────────
SkillIdx                   DWORD         skill_id         uint16       ✅     -   -      -
Operator (caster)          DWORD         caster_id        uint32       ✅     -   -      -
MainTarget.dwID            DWORD         target_id        uint32       ✅     -   -      -
MainTarget.cpPos           COMPRESSEDPOS position         Vec3         ✅     -   -      -
 ── Response fields (SkillCastResult) ─────────────────────────────────────────────────────────────
(missing)                  -             hits             uint8        🟢     -   -      Added
(missing)                  -             results          [AttackResult] 🟢  -   -      Added
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Enhanced with results array — Old mengirim result per target individual.
NOTE: hits and results are in SkillCastResult (response), NOT in SkillCastRequest.
```

---

## 5. INVENTORY: MP_ITEM_MOVE_SYN → Inventory.fbs (InventoryMoveRequest)

```
PACKET: MP_ITEM_MOVE_SYN → Inventory.fbs (InventoryMoveRequest)     [Total effort: 3 days]
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Old Field                  Type (Old)    Reborn Field     FBS Type     Status Sev Effort Deps
─────────────────────────────────────────────────────────────────────────────────────────────────────
FromPos.Type               POSTYPE       src_storage_type  uint8       ✅     M   2 hrs  Agent #009
FromPos.Index              BYTE          src_slot          uint8       ✅     -   -      -
wFromItemIdx               DWORD         item_id           uint32      ✅     -   -      -
ToPos.Type                 POSTYPE       dst_storage_type  uint8       ✅     M   2 hrs  Agent #009
ToPos.Index                BYTE          dst_slot          uint8       ✅     M   2 hrs  Agent #009
wToItemIdx                 DWORD         (implied)         -            🟡     -   -      -
(missing)                  -             count             uint16       🟢     -   -      Added

InventorySlot:
bySlot                     BYTE          slot_index        uint8       ✅     -   -      -
dwItemDBIdx                DWORD         item_id           uint32      ✅     -   -      -
byCount                    BYTE          count             uint16      ✅     -   -      -
wDurability                WORD          durability        uint16      ✅     M   2 hrs  Agent #009
bEnchantLevel              BYTE          enchant           uint8       ✅     -   -      -
(banyak field option)      (varies)      TB_ITEM_OPTION   (DB table)  ✅     -   -      -
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Total: Old 5+ item fields, Reborn 5+. ✅ Complete.
```

---

## 6. QUEST: MP_QUEST_TOTALINFO → Quest.fbs (QuestListResponse)

```
PACKET: MP_QUEST_TOTALINFO → Quest.fbs (QuestListResponse)          [Total effort: 2 days]
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Old Field                  Type (Old)    Reborn Field     FBS Type     Status Sev Effort Deps
─────────────────────────────────────────────────────────────────────────────────────────────────────
QuestList[i].QuestIdx      WORD          quest_id         uint32       ✅     -   -      -
QuestList[i].state         QSTATETYPE    is_completed      bool         🟡     L   1 hr   -
(missing)                  -             objectives        [QuestObjState] 🟢  -   -     Added
(missing)                  -             is_reward_taken   bool         🟢     -   -      Added

QuestStartRequest:
dwNPCID / dwQuestID        DWORD x 2     npc_id, quest_id  uint32 x 2  ✅     -   -      -
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Good coverage. Enhanced with objectives array + reward tracking.
```

---

## 7. PARTY: MP_PARTY_INFO → Party.fbs (PartyInfo)

```
PACKET: MP_PARTY_INFO → Party.fbs (PartyInfo)                       [Total effort: 1 day]
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Old Field (PARTY_INFO)     Type (Old)    Reborn Field     FBS Type     Status Sev Effort Deps
─────────────────────────────────────────────────────────────────────────────────────────────────────
PartyDBIdx                 DWORD         party_id          uint32      ✅     -   -      -
Option                     BYTE          (in code)         -            🟡     -   -      -
SubOption                  BYTE          (in code)         -            🟡     -   -      -
Member[i].dwMemberID       DWORD         members[i].       uint32      ✅     -   -      -
                                          character_id
Member[i].Name             char[21]      members[i].name   string       ✅     -   -      -
Member[i].Level            LEVELTYPE     members[i].level  uint16      ✅     -   -      -
Member[i].LifePercent      BYTE          (hp/max_hp)       int32 x 2   ✅     -   -      -
Member[i].mMapType         MAPTYPE       members[i].       uint16      ✅     -   -      -
                                          map_id
Member[i].bLogged          BOOL          online            bool         ✅     -   -      -
(missing)                  -             is_leader         bool         🟢     -   -      Added
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Total: Old ~8 field, Reborn ~8 field. Excellent coverage. ✅
```

---

## 8. GUILD: MP_GUILD_INFO → Guild.fbs (GuildInfo)

```
PACKET: MP_GUILD_INFO → Guild.fbs (GuildInfo)                       [Total effort: 2 days]
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Old Field (GUILDINFO)      Type (Old)    Reborn Field     FBS Type     Status Sev Effort Deps
─────────────────────────────────────────────────────────────────────────────────────────────────────
GuildIdx                   DWORD         guild_id          uint32      ✅     -   -      -
GuildName                  char[33]      name              string       ✅     -   -      -
MasterIdx                  DWORD         master_id         uint32      ✅     -   -      -
GuildLevel                 LEVELTYPE     level             uint8       ✅     -   -      -
(missing)                  -             gp                uint32      🟢     -   -      Added
dwGuildPoint               DWORD         guild_point       uint32      ✅     M   1 day  Agent #011
szGuildMark[256]           CHAR[256]     guild_mark        string       ✅     M   2 days Agent #011
MarkName                   MARKNAMETYPE  (in guild_mark)   -            🟡     -   -      -
Member[i].MemberIdx        DWORD         members[i].       uint32      ✅     -   -      -
                                          character_id
Member[i].MemberName       char[21]      members[i].name   string       ✅     -   -      -
Member[i].Memberlvl        LEVELTYPE     members[i].level  uint16      ✅     -   -      -
Member[i].Rank             BYTE          members[i].rank   uint8       ✅     -   -      -
Member[i].bLogged          BOOL          members[i].online bool        ✅     -   -      -
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Total: Old 10+ fields, Reborn 10+ fields. ✅ Complete.
```

---

## 9. CHAT: MP_CHAT_ALL → Chat.fbs (ChatMessage)

```
PACKET: MP_CHAT_ALL → Chat.fbs (ChatMessage)                        [Total effort: 1 day]
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Old Field (MSG_CHAT)       Type (Old)    Reborn Field     FBS Type     Status Sev Effort Deps
─────────────────────────────────────────────────────────────────────────────────────────────────────
Name                       char[21]      sender_name       string       ✅     -   -      -
Msg                        char[256]     message           string       ✅     -   -      -
(missing)                  -             sender_id         uint32      🟢     -   -      Added
(missing)                  -             channel           ChatChannel  🟢     -   -      Added
(missing)                  -             timestamp         uint64      🟢     -   -      Added

Extended schemas (Chat.fbs):
WhisperMessage  → sender_id, sender_name, receiver_id, receiver_name, message, timestamp
PartyChatMessage → sender_id, sender_name, message, member_ids, timestamp
GuildChatMessage → sender_id, sender_name, guild_id, message, timestamp
FamilyChatMessage→ sender_id, sender_name, family_id, message, timestamp
ShoutMessage     → sender_id, sender_name, message, item_id, item_pos, timestamp
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Total: Old 2 field, Reborn 5+ field. Enhanced. ✅
```

---

## 10. NPC: MP_NPC_SPEECH_SYN → NPC.fbs (NpcRequest)

```
PACKET: MP_NPC_SPEECH_SYN → NPC.fbs (NpcRequest)                    [Total effort: 2 days]
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Old Field (MSG_NAME)       Type (Old)    Reborn Field     FBS Type     Status Sev Effort Deps
─────────────────────────────────────────────────────────────────────────────────────────────────────
Name (NPC name)            char[21]      npc_id            uint32      🟡     M   2 hrs  -
(missing)                  -             character_id      uint32      🟢     -   -      Added
(missing)                  -             action            NpcAction    🟢     -   -      Added
(missing)                  -             quest_id          uint32      🟢     -   -      Added

NpcAction enum:
  Talk=0, Shop=1, Quest=2, ChangeMap=3, Recall=4

NpcResponse:
  result, npc_id, action, dialog_text, shop_items, destinations, recall_destinations, quest_ids

Old packet varieties → all covered:
  MP_NPC_TALK_SYN      → NpcRequest(action=Talk)
  MP_NPC_SHOP_SYN      → NpcRequest(action=Shop) + NpcShopItem[]
  MP_NPC_QUEST_SYN     → NpcRequest(action=Quest) + quest_ids[]
  MP_NPC_CHANGEMAP_SYN → NpcRequest(action=ChangeMap) + NpcChangeMapDestination[]
  MP_NPC_RECALL_SYN    → NpcRequest(action=Recall) + NpcRecallDestination[]
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Total: Old 1 field, Reborn 4+ field. ✅ Complete with new NPC.fbs.
```

---

## Summary — 10 Packet Categories

```
RANKING OF COMPLETENESS
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Rank  Category     Field Match   Handler Wired   Status    Total Effort
─────────────────────────────────────────────────────────────────────────────────────────────
1     CHAT         100%          ✅              ✅        1 day
2     PARTY        100%          ✅              ✅        1 day
3     GUILD        95%           ✅              ✅        2 days
4     QUEST        95%           ✅              ✅        2 days
5     MOVEMENT     86%           ✅              ✅        2 days
6     INVENTORY    100%          ✅              ✅        3 days
7     NPC          100%          ✅              ✅        2 days
8     SKILL        60%           ✅              🟡        3 days
9     COMBAT       60%           ✅              🟡        3 days
10    LOGIN        50% (3/6)     ✅              🟡        2 days
─────────────────────────────────────────────────────────────────────────────────────────────
AVERAGE:         85%            100% wired       ✅        ~21 days total
─────────────────────────────────────────────────────────────────────────────────────────────

Catatan:
- Login: 3 field Old dihapus (strSecurityPW, Check, mLoginKey — tidak relevan)
- Skill/Combat: Old punya target list + consume item — Reborn simplified
- Handler wiring: ✅ semua 10 kategori sudah memiliki handler server dan client
```

---

*End of PACKET_MAPPING.md — 10 categories field-by-field*

