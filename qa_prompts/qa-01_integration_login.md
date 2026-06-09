# QA-01 Rev — Integration: Login Flow + Packet Round-trip (DIPERBAIKI)

Lokasi: /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn
Build: cmake --build build/macos-debug -j$(sysctl -n hw.ncpu) --target test_runner

## Perbaikan dari versi sebelumnya
- ✅ API signatures sudah diverifikasi dari header aktual
- ✅ Login test menggunakan actual LoginHandler + AgentServer method
- ✅ Packet round-trip test menggunakan generated FlatBuffers API
- ✅ Quest test DITAMBAHKAN

## Test Scenarios

### Test 1: Login + Character List
```cpp
// Verifikasi: LoginHandler.hpp punya HandleLogin, HandleCharacterList
// BACA dulu: game/network/LoginHandler.hpp
// Gunakan method yang SUDAH ADA
```
1. Buat LoginRequest dengan FlatBuffers
2. Panggil method handle login
3. Test: result sukses atau gagal sesuai input
4. Test: server list tidak kosong

### Test 2: Packet Round-trip (10 types)
```cpp
// Untuk SETIAP packet type:
// 1. Buat builder + create function
// 2. Finish → simpan buffer pointer + size
// 3. GetRoot dari buffer
// 4. VERIFIKASI field-by-field
luna::protocol::Vec3 dir(1.0f, 0.0f, 0.0f);
auto move = luna::protocol::CreateMoveRequest(fbb, &dir, &dir,
    luna::protocol::MoveMode_Walk, 0, 0.0f);
fbb.Finish(move);
auto root = flatbuffers::GetRoot<luna::protocol::MoveRequest>(fbb.GetBufferPointer());
TEST("Move dir.x", std::abs(root->direction()->x() - 1.0f) < 0.001f);
```
- LoginRequest (6 field)
- MoveRequest (direction, target, mode, kyung_gong, speed)
- AttackRequest (target_id, skill_id, position)
- AttackResult (damage, crit, miss, block, hp_remaining)
- ChatMessage (sender, name, msg, channel, timestamp)
- GuildInfo (id, name, level, gp, guild_point, mark, master)
- NpcRequest (npc_id, character_id, action)
- NpcResponse (result, npc_id, action, shop_items)

### Test 3: Quest Lifecycle
```cpp
// QuestSystem method signatures (dari QuestSystem.hpp):
//   bool StartQuest(reg, player, quest_id);
//   bool CompleteQuest(reg, player, quest_id);
//   bool ClaimReward(reg, player, quest_id);
//   bool AbandonQuest(reg, player, quest_id);
//   void UpdateObjective(reg, player, quest_id, objective_id, progress);
//   bool CheckConditions(reg, player, quest_id);
```
1. Setup player dengan QuestLog component
2. StartQuest → Test: quest.state = InProgress
3. UpdateObjective(mob_kill, count=5) → Test: progress = 5
4. CompleteQuest → Test: quest.state = Completed
5. ClaimReward → Test: item diterima atau exp bertambah
6. AbandonQuest → Test: quest dihapus dari log

## ✅ Kembalikan: "QA-01 done: integration + quest tests, 0 failures"
