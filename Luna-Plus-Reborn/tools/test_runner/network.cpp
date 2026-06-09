#include "test_harness.hpp"
#include <flatbuffers/flatbuffers.h>
#include <Login_generated.h>
#include <Character_generated.h>
#include <Movement_generated.h>
#include <Combat_generated.h>
#include <Chat_generated.h>
#include <Guild_generated.h>
#include <NPC_generated.h>
#include <PacketType_generated.h>


void RunNetworkTests() {
    TEST_SUITE("NETWORK PROTOCOL");

    // ─── Login packet ───
    TEST_STEP("LoginRequest creation");
    {
        flatbuffers::FlatBufferBuilder fbb;
        auto username = fbb.CreateString("TestPlayer");
        std::vector<uint8_t> pw_hash(32, 0x42);
        auto hash_vec = fbb.CreateVector(pw_hash);
        auto version = fbb.CreateString("1.0.0");
        auto mac = fbb.CreateString("00-11-22-33-44-55");

        auto req = luna::protocol::CreateLoginRequest(fbb, username, hash_vec,
                                                       version, 0x12345678, 0x9ABCDEF0, mac);
        fbb.Finish(req);

        auto root = flatbuffers::GetRoot<luna::protocol::LoginRequest>(fbb.GetBufferPointer());
        TEST("Login username", root->username()->str() == "TestPlayer");
        TEST("Login hash size = 32", root->password_hash()->size() == 32);
        TEST("Login version", root->client_version()->str() == "1.0.0");
        TEST("Login mac", root->mac_address()->str() == "00-11-22-33-44-55");
        TEST("Login crc = 0x12345678", root->file_crc() == 0x12345678);
        TEST("Login timestamp = 0x9ABCDEF0", root->client_timestamp() == 0x9ABCDEF0);
        spdlog::info("    LoginRequest: user='{}' ver='{}' mac='{}'",
                     root->username()->str(), root->client_version()->str(), root->mac_address()->str());
    }

    // ─── LoginResponse ───
    TEST_STEP("LoginResponse");
    {
        flatbuffers::FlatBufferBuilder fbb;
        auto token = fbb.CreateString("session_token_123");
        auto resp = luna::protocol::CreateLoginResponse(fbb, luna::protocol::LoginResult_Success, token, 0);
        fbb.Finish(resp);
        auto root = flatbuffers::GetRoot<luna::protocol::LoginResponse>(fbb.GetBufferPointer());
        TEST("Login Success", root->result() == luna::protocol::LoginResult_Success);
        TEST("Session token non-empty", root->session_token()->size() > 0);
        spdlog::info("    LoginResponse: result={} token='{}'",
                     static_cast<int>(root->result()), root->session_token()->str());
    }

    // ─── Movement packet (Vec3 is struct, not table) ───
    TEST_STEP("MoveRequest creation");
    {
        flatbuffers::FlatBufferBuilder fbb;
        luna::protocol::Vec3 dir(1.0f, 0.0f, 0.0f);
        luna::protocol::Vec3 target(100.0f, 0.0f, 50.0f);

        auto req = luna::protocol::CreateMoveRequest(fbb, &dir, &target,
                                                       luna::protocol::MoveMode_Walk, 0, 0.0f);
        fbb.Finish(req);

        auto root = flatbuffers::GetRoot<luna::protocol::MoveRequest>(fbb.GetBufferPointer());
        TEST("Move direction.x = 1.0", std::abs(root->direction()->x() - 1.0f) < 0.001f);
        TEST("Move target.x = 100.0", std::abs(root->target_position()->x() - 100.0f) < 0.001f);
        TEST("Move mode = Walk", root->move_mode() == luna::protocol::MoveMode_Walk);
        TEST("KyungGong = 0", root->kyung_gong_idx() == 0);
        TEST("Added speed = 0", std::abs(root->added_move_speed()) < 0.001f);
        spdlog::info("    MoveRequest: dir=({:.1f},{:.1f},{:.1f}) mode={}",
                     root->direction()->x(), root->direction()->y(), root->direction()->z(),
                     static_cast<int>(root->move_mode()));
    }

    // ─── Combat packet ───
    TEST_STEP("AttackRequest + AttackResult");
    {
        flatbuffers::FlatBufferBuilder fbb;
        luna::protocol::Vec3 pos(5.0f, 0.0f, 3.0f);
        auto req = luna::protocol::CreateAttackRequest(fbb, 1001, 7, &pos);
        fbb.Finish(req);

        auto root = flatbuffers::GetRoot<luna::protocol::AttackRequest>(fbb.GetBufferPointer());
        TEST("Attack target = 1001", root->target_id() == 1001);
        TEST("Attack skill = 7", root->skill_id() == 7);
        TEST("Attack position.x = 5", std::abs(root->position()->x() - 5.0f) < 0.001f);

        // AttackResult
        flatbuffers::FlatBufferBuilder fbb2;
        auto result = luna::protocol::CreateAttackResult(fbb2, 1, 1001, 250,
            luna::protocol::DamageType_Magic, true, false, false, 750, 0);
        fbb2.Finish(result);

        auto res = flatbuffers::GetRoot<luna::protocol::AttackResult>(fbb2.GetBufferPointer());
        TEST("Result damage = 250", res->damage() == 250);
        TEST("is_critical = true", res->is_critical());
        TEST("is_miss = false", !res->is_miss());
        TEST("is_blocked = false", !res->is_blocked());
        TEST("hp_remaining = 750", res->target_hp_remaining() == 750);
        spdlog::info("    AttackResult: attacker={} target={} dmg={} crit={}",
                     res->attacker_id(), res->target_id(), res->damage(), res->is_critical());
    }

    // ─── Chat packet ───
    TEST_STEP("ChatMessage");
    {
        flatbuffers::FlatBufferBuilder fbb;
        auto chat = luna::protocol::CreateChatMessageDirect(fbb, 100, "Player1", "Hello world!",
                                                              luna::protocol::ChatChannel_All, 12345678ULL);
        fbb.Finish(chat);
        auto root = flatbuffers::GetRoot<luna::protocol::ChatMessage>(fbb.GetBufferPointer());
        TEST("Chat sender = 100", root->sender_id() == 100);
        TEST("Chat name = Player1", root->sender_name()->str() == "Player1");
        TEST("Chat message", root->message()->str() == "Hello world!");
        TEST("Chat channel = All", root->channel() == luna::protocol::ChatChannel_All);
        TEST("Chat timestamp", root->timestamp() == 12345678ULL);
        spdlog::info("    Chat: id={} '{}' said '{}'", root->sender_id(),
                     root->sender_name()->str(), root->message()->str());

        // GuildChat (separate builder)
        flatbuffers::FlatBufferBuilder fbb2;
        auto guild = luna::protocol::CreateGuildChatMessageDirect(fbb2, 100, "Player1", 5001, "Hello guild!", 87654321ULL);
        fbb2.Finish(guild);
        auto g = flatbuffers::GetRoot<luna::protocol::GuildChatMessage>(fbb2.GetBufferPointer());
        TEST("GuildChat guild_id = 5001", g->guild_id() == 5001);
    }

    // ─── Guild packet ───
    TEST_STEP("GuildInfo");
    {
        flatbuffers::FlatBufferBuilder fbb;
        std::vector<flatbuffers::Offset<luna::protocol::GuildMemberInfo>> members;
        auto m = luna::protocol::CreateGuildMemberInfoDirect(fbb, 42, "Member1", 30, 2, true);
        members.push_back(m);
        auto mvec = fbb.CreateVector(members);
        auto mark = fbb.CreateString("base64_mark");
        auto name = fbb.CreateString("TestGuild");
        auto info = luna::protocol::CreateGuildInfo(fbb, 1, name, 5, 10000, 5000, mark, 42, mvec);
        fbb.Finish(info);

        auto root = flatbuffers::GetRoot<luna::protocol::GuildInfo>(fbb.GetBufferPointer());
        TEST("Guild id = 1", root->guild_id() == 1);
        TEST("Guild name", root->name()->str() == "TestGuild");
        TEST("Guild level = 5", root->level() == 5);
        TEST("Guild gp = 10000", root->gp() == 10000);
        TEST("Guild guild_point = 5000", root->guild_point() == 5000);
        TEST("Guild master = 42", root->master_id() == 42);
        TEST("Guild members = 1", root->members()->size() == 1);
        spdlog::info("    Guild: id={} name='{}' lv={} gp={} members={}",
                     root->guild_id(), root->name()->str(), root->level(),
                     root->gp(), root->members()->size());
    }

    // ─── NPC packet ───
    TEST_STEP("NpcRequest + NpcResponse");
    {
        flatbuffers::FlatBufferBuilder fbb;
        auto req = luna::protocol::CreateNpcRequest(fbb, 501, 1001, luna::protocol::NpcAction_Shop, 0);
        fbb.Finish(req);
        auto root = flatbuffers::GetRoot<luna::protocol::NpcRequest>(fbb.GetBufferPointer());
        TEST("NPC id = 501", root->npc_id() == 501);
        TEST("NPC action = Shop", root->action() == luna::protocol::NpcAction_Shop);

        // NpcResponse with items (separate builder)
        flatbuffers::FlatBufferBuilder fbb3;
        auto item = luna::protocol::CreateNpcShopItem(fbb3, 10001, 500, 10, false);
        std::vector<flatbuffers::Offset<luna::protocol::NpcShopItem>> items_vec = {item};
        auto shop_vec = fbb3.CreateVector(items_vec);
        auto dialog = fbb3.CreateString("Welcome!");
        auto resp = luna::protocol::CreateNpcResponse(fbb3, 0, 501,
            luna::protocol::NpcAction_Shop, dialog, shop_vec, 0, 0, 0);
        fbb3.Finish(resp);
        auto r = flatbuffers::GetRoot<luna::protocol::NpcResponse>(fbb3.GetBufferPointer());
        TEST("NPC result = 0", r->result() == 0);
        TEST("NPC shop items > 0", r->shop_items() && r->shop_items()->size() > 0);
        if (r->shop_items() && r->shop_items()->size() > 0) {
            TEST("NPC item id = 10001", (*r->shop_items())[0]->item_id() == 10001);
        }
        spdlog::info("    NPC: id={} action={} items={} dialog='{}'",
                     r->npc_id(), static_cast<int>(r->action()),
                     r->shop_items() ? r->shop_items()->size() : 0,
                     r->dialog_text() ? r->dialog_text()->str() : "(null)");
    }

    // ─── CharacterList round-trip (Login + Character List test) ───
    TEST_STEP("CharacterListRequest + CharacterListResponse");
    {
        flatbuffers::FlatBufferBuilder fbb;
        auto token = fbb.CreateString("session_token_abc");
        auto req = luna::protocol::CreateCharacterListRequest(fbb, token);
        fbb.Finish(req);
        auto root = flatbuffers::GetRoot<luna::protocol::CharacterListRequest>(fbb.GetBufferPointer());
        TEST("CharList session_token", root->session_token()->str() == "session_token_abc");

        flatbuffers::FlatBufferBuilder fbb2;
        std::vector<flatbuffers::Offset<luna::protocol::CharacterInfo>> chars;
        auto c1 = luna::protocol::CreateCharacterInfoDirect(fbb2, 101, "Hero1", 50, 1, 0, 1001, nullptr, 500, 500);
        chars.push_back(c1);
        auto c2 = luna::protocol::CreateCharacterInfoDirect(fbb2, 102, "Hero2", 30, 2, 1, 1002, nullptr, 300, 300);
        chars.push_back(c2);
        auto list = fbb2.CreateVector(chars);
        auto resp = luna::protocol::CreateCharacterListResponse(fbb2, list, 6);
        fbb2.Finish(resp);

        auto res = flatbuffers::GetRoot<luna::protocol::CharacterListResponse>(fbb2.GetBufferPointer());
        TEST("CharList max_slots = 6", res->max_slots() == 6);
        TEST("CharList count = 2", res->characters()->size() == 2);
        if (res->characters() && res->characters()->size() >= 2) {
            auto first = (*res->characters())[0];
            TEST("Char1 id = 101", first->id() == 101);
            TEST("Char1 name = Hero1", first->name()->str() == "Hero1");
            TEST("Char1 level = 50", first->level() == 50);
            auto second = (*res->characters())[1];
            TEST("Char2 id = 102", second->id() == 102);
            TEST("Char2 name = Hero2", second->name()->str() == "Hero2");
            TEST("Char2 level = 30", second->level() == 30);
        }
        spdlog::info("    CharacterList: {} chars, max_slots={}",
                     res->characters() ? res->characters()->size() : 0, res->max_slots());
    }

    // ─── PacketType enum values ───
    TEST_STEP("PacketType enum coverage");
    {
        TEST("Login SYN = 1537", luna::protocol::PacketType_MP_USERCONN_LOGIN_SYN == 1537);
        TEST("NPC SPEECH_SYN = 16800", luna::protocol::PacketType_MP_NPC_SPEECH_SYN == 16800);
        TEST("VEHICLE SUMMON_SYN = 22016", luna::protocol::PacketType_MP_VEHICLE_SUMMON_SYN == 22016);
        TEST("VEHICLE PASSENGER_ACK = 22035", luna::protocol::PacketType_MP_VEHICLE_PASSENGER_ACK == 22035);
        spdlog::info("    Vehicle packet range: 22016-22035 (20 types)");
    }
}
