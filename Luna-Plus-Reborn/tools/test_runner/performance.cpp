#include "test_harness.hpp"
#include <ecs/components/CharacterStats.hpp>
#include <ecs/components/Transform.hpp>
#include <ecs/systems/CombatSystem.hpp>
#include <entt/entt.hpp>
#include <flatbuffers/flatbuffers.h>
#include <Login_generated.h>
#include <sqlite3.h>
#include <chrono>

template<typename F>
void Benchmark(const char* name, int iterations, F&& fn) {
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < iterations; i++) fn();
    auto end = std::chrono::high_resolution_clock::now();
    auto total_us = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    double avg_us = (double)total_us / iterations;
    double ops_per_sec = 1'000'000.0 / avg_us;
    spdlog::info("  BENCHMARK[{}] {} iters: total={}ms, avg={:.3f}us, {:.0f} ops/sec",
                 name, iterations, total_us / 1000, avg_us, ops_per_sec);
    if (avg_us > 100) spdlog::warn("  SLOW: expected < 100us, got {:.3f}us", avg_us);
}

void RunPerformanceTests() {
    TEST_SUITE("PERFORMANCE BENCHMARKS");

    CharacterStats atk, def;
    atk.level = 50; atk.strength = 100; atk.weapon_attack = 30;
    atk.dexterity = 60; atk.base_dexterity = 20; atk.class_id = 1;
    def.level = 48; def.armor_defense = 100; def.constitution = 60;
    def.dexterity = 40; def.base_dexterity = 20; def.class_id = 2; def.shield_defense = 5;

    TEST_STEP("Benchmark 1: CalculateDamage — 10,000 iter");
    {
        int total_dmg = 0;
        Benchmark("CalculateDamage", 10000, [&]() {
            auto r = CombatSystem::CalculateDamage(atk, def, 0, 1, 0, 0, 1.0f, CombatContext::Normal);
            total_dmg += r.damage;
        });
        TEST("Benchmark ran (total damage calculated)", total_dmg > 0);
    }

    TEST_STEP("Benchmark 2: FlatBuffers encode+decode LoginRequest — 10,000 iter");
    {
        int total_size = 0;
        Benchmark("FlatBuffers LoginRequest", 10000, [&]() {
            flatbuffers::FlatBufferBuilder fbb;
            auto name = fbb.CreateString("BenchmarkPlayer");
            std::vector<uint8_t> hash(32, 0x42);
            auto h = fbb.CreateVector(hash);
            auto ver = fbb.CreateString("1.0.0");
            auto mac = fbb.CreateString("00-00-00-00-00-00");
            auto req = luna::protocol::CreateLoginRequest(fbb, name, h, ver, 0, 0, mac);
            fbb.Finish(req);
            auto root = flatbuffers::GetRoot<luna::protocol::LoginRequest>(fbb.GetBufferPointer());
            total_size += root->username()->size();
        });
        TEST("Benchmark ran (total chars read)", total_size > 0);
    }

    TEST_STEP("Benchmark 3: ECS entity create/destroy — 10,000 iter (100 entities each)");
    {
        int total_entities = 0;
        Benchmark("ECS create+destroy", 10000, [&]() {
            entt::registry reg;
            for (int i = 0; i < 100; i++) {
                auto e = reg.create();
                reg.emplace<CharacterStats>(e);
                reg.emplace<Transform>(e);
                total_entities++;
            }
            reg.clear();
        });
        TEST("ECS benchmark created entities", total_entities > 0);
    }

    TEST_STEP("Benchmark 4: SQLite INSERT — 10,000 iter");
    {
        sqlite3* db = nullptr;
        sqlite3_open(":memory:", &db);
        sqlite3_exec(db, "CREATE TABLE perf (id INTEGER PRIMARY KEY, name TEXT, value INTEGER)", 0, 0, 0);
        sqlite3_stmt* stmt = nullptr;
        sqlite3_prepare_v2(db, "INSERT INTO perf VALUES (?, ?, ?)", -1, &stmt, 0);

        int row_count = 0;
        Benchmark("SQLite INSERT", 10000, [&]() {
            static int counter = 0;
            counter++;
            sqlite3_bind_int(stmt, 1, counter);
            sqlite3_bind_text(stmt, 2, "perf_test", -1, SQLITE_STATIC);
            sqlite3_bind_int(stmt, 3, counter * 10);
            sqlite3_step(stmt);
            sqlite3_reset(stmt);
            row_count++;
        });
        sqlite3_finalize(stmt);
        TEST("INSERT benchmark inserted rows", row_count == 10000);
    }

    TEST_STEP("Benchmark 5: SQLite SELECT by PK — 10,000 iter");
    {
        sqlite3* db = nullptr;
        sqlite3_open(":memory:", &db);
        sqlite3_exec(db, "CREATE TABLE perf (id INTEGER PRIMARY KEY, name TEXT, value INTEGER)", 0, 0, 0);
        sqlite3_stmt* istmt = nullptr;
        sqlite3_prepare_v2(db, "INSERT INTO perf VALUES (?, ?, ?)", -1, &istmt, 0);
        for (int i = 1; i <= 10000; i++) {
            sqlite3_bind_int(istmt, 1, i);
            sqlite3_bind_text(istmt, 2, "perf_test", -1, SQLITE_STATIC);
            sqlite3_bind_int(istmt, 3, i * 10);
            sqlite3_step(istmt);
            sqlite3_reset(istmt);
        }
        sqlite3_finalize(istmt);

        sqlite3_stmt* stmt = nullptr;
        sqlite3_prepare_v2(db, "SELECT * FROM perf WHERE id = ?", -1, &stmt, 0);
        int rows_read = 0;
        Benchmark("SQLite SELECT by PK", 10000, [&]() {
            static int counter = 0;
            counter = (counter % 10000) + 1;
            sqlite3_bind_int(stmt, 1, counter);
            sqlite3_step(stmt);
            sqlite3_reset(stmt);
            rows_read++;
        });
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        TEST("SELECT benchmark read rows", rows_read == 10000);
    }
}
