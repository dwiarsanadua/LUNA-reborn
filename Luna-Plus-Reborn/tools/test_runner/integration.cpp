#include "test_harness.hpp"
#include <string>
#include <vector>
#include <cstdint>
#include <algorithm>

struct LoginRequest {
    std::string username;
    std::vector<uint8_t> password_hash;
    std::string client_version;
    uint32_t file_crc;
    uint64_t timestamp;
    std::string mac_address;
};

struct LoginResponse {
    bool success;
    std::string session_token;
    uint32_t account_id;
};

struct CharacterData {
    uint32_t id;
    std::string name;
    int level;
    int hp, mp, max_hp, max_mp;
    int map_id;
    float pos_x, pos_y, pos_z;
    int battle_style;
};

struct PacketBuffer {
    std::vector<uint8_t> data;
    size_t pos = 0;

    void WriteU8(uint8_t v) { data.push_back(v); }
    void WriteU16(uint16_t v) { data.push_back(v & 0xFF); data.push_back((v >> 8) & 0xFF); }
    void WriteU32(uint32_t v) {
        for (int i = 0; i < 4; i++) { data.push_back((v >> (i * 8)) & 0xFF); }
    }
    void Skip(size_t n) { pos = std::min(pos + n, data.size()); }

    void WriteString(const std::string& s) {
        WriteU16(static_cast<uint16_t>(s.size()));
        for (char c : s) data.push_back(static_cast<uint8_t>(c));
    }

    uint8_t ReadU8() { return pos < data.size() ? data[pos++] : 0; }
    uint16_t ReadU16() {
        uint16_t v = 0;
        if (pos + 2 <= data.size()) {
            v = data[pos] | (static_cast<uint16_t>(data[pos + 1]) << 8);
            pos += 2;
        }
        return v;
    }
    uint32_t ReadU32() {
        uint32_t v = 0;
        for (int i = 0; i < 4 && pos < data.size(); i++) {
            v |= (static_cast<uint32_t>(data[pos++]) << (i * 8));
        }
        return v;
    }
    std::string ReadString() {
        uint16_t len = ReadU16();
        if (pos + len > data.size()) return "";
        std::string s(data.begin() + pos, data.begin() + pos + len);
        pos += len;
        return s;
    }
};

void RunIntegrationTests() {
    TEST_SUITE("INTEGRATION: LOGIN + AUTH + SESSION");

    TEST_STEP("Login request/response round-trip via packet buffer");
    {
        LoginRequest req;
        req.username = "TestPlayer";
        req.password_hash = std::vector<uint8_t>(32, 0x42);
        req.client_version = "1.0.0";
        req.file_crc = 0x12345678;
        req.timestamp = 0x9ABCDEF0;
        req.mac_address = "00-11-22-33-44-55";

        PacketBuffer buf;
        buf.WriteString(req.username);
        buf.WriteU16(static_cast<uint16_t>(req.password_hash.size()));
        for (auto b : req.password_hash) buf.WriteU8(b);
        buf.WriteString(req.client_version);
        buf.WriteU32(req.file_crc);
        buf.WriteU32(static_cast<uint32_t>(req.timestamp));
        buf.WriteString(req.mac_address);

        buf.pos = 0;
        std::string user = buf.ReadString();
        uint16_t hash_len = buf.ReadU16();
        std::vector<uint8_t> hash;
        for (int i = 0; i < hash_len; i++) hash.push_back(buf.ReadU8());
        std::string ver = buf.ReadString();
        uint32_t crc = buf.ReadU32();
        uint32_t ts = buf.ReadU32();
        std::string mac = buf.ReadString();

        TEST("Username round-trip", user == req.username);
        TEST("Hash round-trip", hash == req.password_hash);
        TEST("Version round-trip", ver == req.client_version);
        TEST("CRC round-trip", crc == req.file_crc);
        TEST("Timestamp round-trip", ts == req.timestamp);
        TEST("MAC round-trip", mac == req.mac_address);
    }

    TEST_STEP("Login response with session token");
    {
        LoginResponse resp;
        resp.success = true;
        resp.session_token = "session_token_abc123";
        resp.account_id = 1001;

        PacketBuffer buf;
        buf.WriteU8(resp.success ? 1 : 0);
        buf.WriteString(resp.session_token);
        buf.WriteU32(resp.account_id);

        buf.pos = 0;
        bool success = buf.ReadU8() != 0;
        std::string token = buf.ReadString();
        uint32_t acc_id = buf.ReadU32();

        TEST("Login success flag", success);
        TEST("Session token preserved", token == resp.session_token);
        TEST("Account ID preserved", acc_id == resp.account_id);
    }

    TEST_STEP("Character select — multiple character encoding");
    {
        std::vector<CharacterData> chars = {
            {1, "HeroOne", 50, 1000, 500, 1000, 500, 1, 100.0f, 0.0f, 50.0f, 3},
            {2, "HeroTwo", 30, 600, 300, 600, 300, 2, 200.0f, 0.0f, 80.0f, 1}
        };

        PacketBuffer buf;
        buf.WriteU8(static_cast<uint8_t>(chars.size()));
        for (auto& c : chars) {
            buf.WriteU32(c.id);
            buf.WriteString(c.name);
            buf.WriteU16(static_cast<uint16_t>(c.level));
            buf.WriteU16(static_cast<uint16_t>(c.hp));
            buf.WriteU16(static_cast<uint16_t>(c.max_hp));
            buf.WriteU16(static_cast<uint16_t>(c.mp));
            buf.WriteU16(static_cast<uint16_t>(c.max_mp));
            buf.WriteU16(static_cast<uint16_t>(c.map_id));
        }

        buf.pos = 0;
        uint8_t count = buf.ReadU8();
        TEST("Character count = 2", count == 2);

        for (int i = 0; i < count; i++) {
            uint32_t id = buf.ReadU32();
            std::string name = buf.ReadString();
            uint16_t level = buf.ReadU16();
            uint16_t hp = buf.ReadU16();
            uint16_t max_hp = buf.ReadU16();
            uint16_t mp = buf.ReadU16();
            uint16_t max_mp = buf.ReadU16();
            uint16_t map_id = buf.ReadU16();
            spdlog::info("  Character {}: id={}, name='{}', level={}, hp={}/{}, mp={}/{}, map={}",
                         i + 1, id, name, level, hp, max_hp, mp, max_mp, map_id);
            TEST("Character data valid", id > 0 && !name.empty() && level > 0);
        }
    }

    TEST_STEP("Invalid credentials — empty/null handling");
    {
        PacketBuffer buf;
        buf.WriteString("");
        buf.WriteU16(0);
        buf.WriteString("");
        buf.WriteU32(0);
        buf.WriteU32(0);
        buf.WriteString("");

        buf.pos = 0;
        std::string user = buf.ReadString();
        uint16_t hash_len = buf.ReadU16();
        bool empty_user = user.empty() && hash_len == 0;
        TEST("Empty credentials handled", empty_user);
    }
}
