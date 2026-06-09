# ADAPTASI SPEC — Spesifikasi Teknis Adaptasi Per Layer

> Target: Memetakan SETIAP komponen Old ke implementasi Reborn
> Prioritas: Severity + Effort + Dependencies untuk setiap entry

---

## 1. Player Flow — Adaptasi

```
LAYER: PLAYER FLOW
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Komponen Old              Status    Rekomendasi Adaptasi
────────────────────────────────────────────────────────────────────────────────────────────
HTML notice viewer        ✅       NoticeView.cpp sudah ada — render HTML stripped
(CHtmlViewNotice)                  Saran: integrate WebView atau libcurl fetch

Save ID checkbox          🔴       Tambah CheckBox di LoginScreen:
                                     LoginScreen::init() {
                                         save_id_cb_ = new CheckBox("Save ID", x, y);
                                         save_id_cb_->OnClick([this](bool checked) {
                                             if (checked) Config::Set("login.saved_id", id_field_.text());
                                         });
                                     }

Auto-attack combo loop    ✅       ComboSystem.cpp sudah handle chain attack
(MAX_COMBO_NUM)

Pseudo-code untuk Save ID:
  class LoginScreen {
      void OnLoginSuccess() {
          if (save_id_cb_->IsChecked())
              ConfigManager::SetString("login.saved_id", id_field_.GetText());
          else
              ConfigManager::SetString("login.saved_id", "");
          ConfigManager::Save();
      }
      void OnEnter() {
          auto saved = ConfigManager::GetString("login.saved_id", "");
          if (!saved.empty()) { id_field_.SetText(saved); save_id_cb_->Check(); }
      }
  }
```

---

## 2. UI System — Adaptasi

```
LAYER: UI SYSTEM
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Komponen Old (.bin)       Status    Rekomendasi Adaptasi
────────────────────────────────────────────────────────────────────────────────────────────
213 .bin dialog files     73/213   UiScriptParser perlu parsing full .bin.txt
  (LunaPlus/Data/         (34%)    Setiap .bin → Widget tree:
  Interface/Windows/)               STATIC → Label, BUTTON → Button,
                                    EDIT → InputField, LISTBOX → ListBox,
                                    COMBOBOX → ComboBox, SCROLLBAR → ScrollBar,
                                    IMAGE → ScriptSprite, GAUGE → GaugeBar

Missing dialogs (top 10, by priority):
1. IdentificationDlg.bin  🔴       Item identification dialog
                                    2 days, depends on ItemSystem
2. ItemMallWarehouse.bin  🔴       Cash shop warehouse dialog
                                    2 days, depends on CashShopDialog
3. SiegeWarFlagDlg.bin    🔴       Siege flag placement dialog
                                    2 days, depends on SiegeSystem
4. ProgressDialog.bin     🔴       Progress bar dialog (crafting, moving)
                                    1 day, no deps
5. DissolveDialog.bin     🔴       Item dissolve dialog
                                    1 day, depends on ItemSystem
6. PetresDialog.bin       🔴       Pet resurrection dialog
                                    1 day, depends on PetDialog
7. QuickSlot.bin          🔴       Quick slot bar HUD
                                    1 day, no deps
8. BattleGuage.bin        🔴       Battle gauge HUD (HP/MP/SP bars)
                                    4 hrs, no deps
9. Channel.bin            🔴       Channel selection dialog
                                    1 day, no deps
10. SystemMsg.bin         🔴       System message popup dialog
                                    4 hrs, no deps

Pseudo-code pattern untuk dialog baru:
  class XxxDialog {
      Window* window_ = nullptr;
  public:
      void Open(WindowManager* wm) {
          window_ = wm->LoadFromScript("Xxx.bin.txt");
          if (!window_) CreateFallback();
      }
      void CreateFallback() {
          window_ = new Window("Title", x, y, w, h);
          window_->AddWidget<Button>("OK", cx, cy, cw, ch);
          // ... layout C++
      }
  };
```

---

## 3. Gameplay Constants — Adaptasi

```
LAYER: GAMEPLAY / HERO COMBAT
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Komponen Old              Status    Rekomendasi Adaptasi
────────────────────────────────────────────────────────────────────────────────────────────
Physical Attack           ✅       CombatSystem::CalcPhysicAttack()
(Weapon+Level)×           ✅       Formula Old exact — sudah diverifikasi
  (1+STR/1000)+(STR−30)

Physical Defense          ✅       CombatSystem::CalcPhysicDefense()
(Armor+Level)×            ✅       Formula Old exact
  (1+VIT/3000)+(VIT−40)/5

Critical Rate             ✅       CombatSystem::CalcCritRate()
45×DexRate+(BaseDEX−25)/5 ✅       Formula Old exact

Block Rate                ✅       CombatSystem::CalcBlockRate()
DEX/27 + ClassBonus       ✅       ClassBonus: Fighter=15, Rogue=10, Ranger=5, Mage=9

Accuracy/Evasion          ✅       CombatSystem::IsHit()
85 + Acc − Eva            ✅       Sistem accuracy/evasion penuh

Level Penalty             ✅       Hanya -1.5% per level jika atk < def

PvP Damage Reduction      ✅       PvP: ×0.35, GT: ×0.10, Siege: ×0.50

Heal Formula              ✅       CalcHealAmount()

Element System            ✅       Disabled (tidak ada di Old)

Aggro System              🟡       ThreatTable sudah ada.
  Avoid/Defense penalty             Perlu implementasi aggro penalty:
  aggro≥3: Avoid×=1−(n−2)²×0.01    jika aggroCount≥3 → avoid penalty
  aggro≥6: Def×=1−(n−5)^1.5×0.01   jika aggroCount≥6 → defense penalty
                                    3 days, depends on AISystem

Combo System              🟡       Perlu verifikasi konstanta MAX_COMBO_NUM
  MAX_COMBO_NUM = ?                dari Old Hero.h
  SKILL_COMBO_NUM = ?              2 days, depends on CombatSystem

Pseudo-code aggro penalty:
  // Di AISystem::Update() atau CombatSystem::HandleDamage()
  int aggroCount = GetAggroCount(monster_id); // jumlah entity aggro ke monster ini
  if (aggroCount >= 3) {
      float avoidPenalty = 1.0f - (aggroCount - 2) * (aggroCount - 2) * 0.01f;
      defender.evasion *= avoidPenalty;
  }
  if (aggroCount >= 6) {
      float defPenalty = 1.0f - std::pow((float)(aggroCount - 5), 1.5f) * 0.01f;
      def *= defPenalty;
  }
```

---

## 4. Network Protocol — Adaptasi

```
LAYER: NETWORK PROTOCOL
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Komponen Old              Status    Rekomendasi Adaptasi
────────────────────────────────────────────────────────────────────────────────────────────
94 MP_CATEGORY            246/2034  ~12% of Old packet types
(Protocol.h enum)         entries   ~30 game categories have .fbs schemas
55 MP_PROTOCOL enums      30 .fbs   Covers all major gameplay categories
                                    Admin/anti-cheat: not ported (removed)

Handler wiring (10 cat):  ✅      All wired (agents A-E phase2/3)

Sisa (non-game, not ported):
  MP_MONITORTOOL* (23)    🔴        Admin monitoring tools
  MP_HACKSHIELD* (2)      🔴        Anti-cheat (removed)
  MP_NPROTECT* (3)        🔴        Anti-cheat (removed)
  MP_CHEAT* (20+)         🔴        GM commands — port jika perlu
  MP_MORNITOR* (20+)      🔴        Server monitoring

Pseudo-code handler pattern (sudah ada di MapServer):
  void MapServer::HandleNpcSpeech(const uint8_t* payload, size_t len) {
      auto req = flatbuffers::GetRoot<NpcRequest>(payload);
      // ... process
      flatbuffers::FlatBufferBuilder fbb;
      auto resp = CreateNpcResponse(fbb, ...);
      fbb.Finish(resp);
      SendPacket(conn_id, PacketType_MP_NPC_SPEECH_ACK, fbb);
  }
```

---

## 5. Server Systems — Adaptasi

```
LAYER: SERVER SYSTEMS
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Komponen Old              Status    Rekomendasi Adaptasi
────────────────────────────────────────────────────────────────────────────────────────────
AgentServer (21 files)    10 files  Missing: NpcRecallMgr (L, 2d), PlustimeMgr (L, 2d)
                                    HackShield/NProtect removed (tidak bisa di-port)
MapServer (210 files)     ~40 files 16/25 subsystems done
                                    Missing: FSM engine integration (M, 2w)
                                             Dungeon full system (M, 1w)
                                             Trigger full system (M, 3d)
DistributeServer (12)     5 files   Missing: Billing (L, 1w)

Pseudo-code untuk subsystem yang belum:
  // FSMEngine — state machine untuk AI
  // Old: Finite State Machine/ folder dengan states:
  //   IDLE, PATROL, CHASE, ATTACK, FLEE, RETURN, DIE
  // Reborn: partial di AISystem.cpp
  // Perlu tambah FSMEngine::SetState(), OnStateEnter(), OnStateExit()
```

---

## 6-15. Ringkasan Adaptasi

```
LAYER: OTHER SYSTEMS
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Layer           Status    Rekomendasi
────────────────────────────────────────────────────────────────────────────────────────────
Database        ✅       Complete — 75 tables, 50+ query templates
Build           90%      Tambah target: PackingTool, NewPackingTool, MapEditor (1 week)
Error Handling  50%      Tambah NACK codes untuk setiap error path (2 weeks)
Security        60%      Server validation sudah, anti-cheat client via mitigation (3 weeks)
Concurrency     70%      Job system via asio::post, DB pool untuk PostgreSQL (1 week)
Localization    80%      Font CJK, sisa ~400 strings (1 week)
Audio           ✅       Complete
Physics         60%      Collision integration, vehicle/ragdoll (2 weeks)

PRIORITAS EKSEKUSI:
  P0: Aggro penalty implementation (3 days)
  P1: Missing dialogs (3 weeks)
  P2: Error NACK codes (2 weeks)
  P3: Server subsystems (4 weeks)
  P4: Font CJK + strings (1 week)
  P5: Build tools (1 week)
```

---

*End of ADAPTASI_SPEC.md — Prioritas + pseudo-code untuk setiap gap*

