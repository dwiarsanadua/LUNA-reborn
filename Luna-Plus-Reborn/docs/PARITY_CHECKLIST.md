# LUNA Plus Reborn — Protocol Parity Checklist

> Generated: 2026-06-09
> Legacy source: `Luna-Plus-Old/[CC]Header/Protocol.h`
> Reborn packets: `game/network/protocol/PacketType.fbs`

| # | MP_CATEGORY | Status | Notes |
|---|-------------|--------|-------|
| 1 | `MP_SERVER = 1` | ⬜ missing | Not wired in Reborn |
| 2 | `MP_POWERUP` | ⬜ missing | Not wired in Reborn |
| 3 | `MP_CHAR` | ✅ done | Char life, stats, respawn sync |
| 4 | `MP_MAP` | 🟡 partial | 16 packet(s) in PacketType.fbs |
| 5 | `MP_ITEM` | 🟡 partial | Loot, inventory sync; compose/enchant UI only |
| 6 | `MP_CHAT` | ✅ done | All/whisper/party/guild chat |
| 7 | `MP_USERCONN` | ✅ done | Login, char list/make, game in, changemap |
| 8 | `MP_MOVE` | ✅ done | Walk/run sync + monster broadcast |
| 9 | `MP_SKILLTREE` | ⬜ missing | Not wired in Reborn |
| 10 | `MP_AUCTIONBOARD` | ⬜ missing | Not wired in Reborn |
| 11 | `MP_CHEAT` | ⬜ missing | GM tools not ported |
| 12 | `MP_QUICK` | ⬜ missing | Not wired in Reborn |
| 13 | `MP_PACKEDDATA` | ⬜ missing | Not wired in Reborn |
| 14 | `MP_PARTY` | ✅ done | Create/invite/leave + party chat |
| 15 | `MP_PEACEWARMODE` | ⬜ missing | Not wired in Reborn |
| 16 | `MP_UNGIJOSIK` | ⬜ missing | Not wired in Reborn |
| 17 | `MP_AUCTION` | ⬜ missing | Not wired in Reborn |
| 18 | `MP_AUTOPATCH` | 🟡 partial | Launcher + bspatch + manifest |
| 19 | `MP_SIGNAL` | ⬜ missing | Not wired in Reborn |
| 20 | `MP_MUNPA` | ⬜ missing | Not wired in Reborn |
| 21 | `MP_SKILL` | ✅ done | Cast + combat attack validation |
| 22 | `MP_KYUNGGONG` | ⬜ missing | Not wired in Reborn |
| 23 | `MP_SIMBUB` | ⬜ missing | Not wired in Reborn |
| 24 | `MP_MORNITORTOOL` | 🟡 partial | Monitor tool stub in tools/monitor |
| 25 | `MP_MORNITORSERVER` | 🟡 partial | DistributeServer registry |
| 26 | `MP_MORNITORMAPSERVER` | 🟡 partial | MapServer register/list |
| 27 | `MP_EXCHANGE` | ✅ done | Player trade sync |
| 28 | `MP_STREETSTALL` | ✅ done | Personal + NPC stalls |
| 29 | `MP_STORAGE` | ✅ done | Bank deposit/withdraw |
| 30 | `MP_BATTLE` | ✅ done | Combat + skills |
| 31 | `MP_CHAR_REVIVE` | ⬜ missing | Not wired in Reborn |
| 32 | `MP_FRIEND` | ✅ done | Add/del/list |
| 33 | `MP_BOSSMONSTER` | ⬜ missing | Not wired in Reborn |
| 34 | `MP_MONSTER` | ✅ done | Spawn, AI, combat, loot |
| 35 | `MP_OPTION` | 🟡 partial | Options dialog; not all legacy toggles |
| 36 | `MP_NPC` | 🟡 partial | Dialog + shop; not all NPC scripts |
| 37 | `MP_MURIMNET` | ⬜ missing | Not wired in Reborn |
| 38 | `MP_QUEST` | ✅ done | 713+ quests, Lua FSM, DB persistence |
| 39 | `MP_DEBUG` | ⬜ missing | Not wired in Reborn |
| 40 | `MP_PK` | ⬜ missing | Not wired in Reborn |
| 41 | `MP_HACKCHECK` | ⬜ missing | Not wired in Reborn |
| 42 | `MP_RMTOOL_CONNECT` | ⬜ missing | Not wired in Reborn |
| 43 | `MP_RMTOOL_USER` | ⬜ missing | Not wired in Reborn |
| 44 | `MP_RMTOOL_MUNPA` | ⬜ missing | Not wired in Reborn |
| 45 | `MP_RMTOOL_GAMELOG` | ⬜ missing | Not wired in Reborn |
| 46 | `MP_RMTOOL_OPERLOG` | ⬜ missing | Not wired in Reborn |
| 47 | `MP_RMTOOL_STATISTICS` | ⬜ missing | Not wired in Reborn |
| 48 | `MP_RMTOOL_ADMIN` | ⬜ missing | Not wired in Reborn |
| 49 | `MP_RMTOOL_CHARACTER` | ⬜ missing | Not wired in Reborn |
| 50 | `MP_RMTOOL_ITEM` | ⬜ missing | Not wired in Reborn |
| 51 | `MP_WANTED` | ⬜ missing | Not wired in Reborn |
| 52 | `MP_JOURNAL` | ⬜ missing | Not wired in Reborn |
| 53 | `MP_SURYUN` | ⬜ missing | Not wired in Reborn |
| 54 | `MP_SOCIETYACT` | ⬜ missing | Not wired in Reborn |
| 55 | `MP_GUILD` | ✅ done | Create/info/member + guild chat |
| 56 | `MP_GUILD_WAR` | ⬜ missing | Not wired in Reborn |
| 57 | `MP_NOTE` | 🟡 partial | Mail/notes UI overlap |
| 58 | `MP_PARTYWAR` | ⬜ missing | Not wired in Reborn |
| 59 | `MP_GTOURNAMENT` | 🟡 partial | Bracket sim + register/claim (Phase 6) |
| 60 | `MP_JACKPOT` | ⬜ missing | Not wired in Reborn |
| 61 | `MP_GUILD_UNION` | ⬜ missing | Not wired in Reborn |
| 62 | `MP_SIEGEWAR` | 🟡 partial | Siege declare/tax/sim resolve (Phase 6) |
| 63 | `MP_SIEGEWAR_PROFIT` | ⬜ missing | Not wired in Reborn |
| 64 | `MP_WEATHER` | 🟡 partial | Client weather FX; limited server sync |
| 65 | `MP_HACKSHIELD` | 🟡 partial | 5 packet(s) in PacketType.fbs |
| 66 | `MP_NPROTECT` | 🟡 partial | 4 packet(s) in PacketType.fbs |
| 67 | `MP_RMTOOL_DELCHAR` | ⬜ missing | Not wired in Reborn |
| 68 | `MP_FAMILY` | ✅ done | Family + marriage actions (Phase 6) |
| 69 | `MP_FACIAL` | ⬜ missing | Not wired in Reborn |
| 70 | `MP_EMOTION` | ⬜ missing | Not wired in Reborn |
| 71 | `MP_DATEMATCHING` | ⬜ missing | Not wired in Reborn |
| 72 | `MP_DATE` | 🟡 partial | 7 packet(s) in PacketType.fbs |
| 73 | `MP_FARM` | ✅ done | Plant/water/harvest + DB (Phase 6) |
| 74 | `MP_RESIDENTREGIST` | ⬜ missing | Not wired in Reborn |
| 75 | `MP_TUTORIAL` | ⬜ missing | Not wired in Reborn |
| 76 | `MP_MARRIAGE` | ✅ done | Via Family system (propose/accept/divorce) |
| 77 | `MP_AUTONOTE` | ⬜ missing | Not wired in Reborn |
| 78 | `MP_PUNISH` | ⬜ missing | Not wired in Reborn |
| 79 | `MP_CHATROOM` | ⬜ missing | Not wired in Reborn |
| 80 | `MP_FISHING` | 🟡 partial | Cast + loot; minigame UI simplified |
| 81 | `MP_PET` | ✅ done | Feed/evolve/summon + DB (Phase 6) |
| 82 | `MP_SIEGERECALL` | ⬜ missing | Not wired in Reborn |
| 83 | `MP_SIEGEWARFARE` | 🟡 partial | Alias coverage via Siege tab |
| 84 | `MP_RECALLNPC` | ⬜ missing | Not wired in Reborn |
| 85 | `MP_COOK` | 🟡 partial | Cooking dialog offline |
| 86 | `MP_LIMITDUNGEON` | ⬜ missing | Not wired in Reborn |
| 87 | `MP_VEHICLE` | 🟡 partial | Mount dialog offline |
| 88 | `MP_HOUSE` | 🟡 partial | Buy/enter/place furniture; no 3D editor |
| 89 | `MP_TRIGGER` | ✅ done | Map triggers + client notify |
| 90 | `MP_DUNGEON` | ✅ done | Entrance + info sync |
| 91 | `MP_PCROOM` | ⬜ missing | Not wired in Reborn |
| 92 | `MP_CONSIGNMENT` | ✅ done | Auction house + search |

## Summary

- **Done:** 21
- **Partial:** 20
- **Missing:** 51
- **Weighted progress:** ~33%

Regenerate: `python3 tools/asset_pipeline/generate_parity_checklist.py`
