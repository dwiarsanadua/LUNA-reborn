#pragma once
#include <string>
#include <vector>
#include <cstdint>
#include <sqlite3.h>

struct CharItem {
    uint32_t item_id = 0;
    uint16_t count = 0;
    uint8_t slot = 0;
    uint8_t enchant = 0;
};

struct CharInfo {
    uint32_t id = 0;
    std::string account_id;
    std::string name;
    uint16_t level = 1;
    uint16_t char_class = 0;
    uint8_t gender = 0;
    uint16_t map_id = 51;
    float pos_x = 0, pos_y = 0, pos_z = 0;
    int32_t hp = 500, max_hp = 500;
    int32_t mp = 100, max_mp = 100;
    uint32_t gold = 0;
    uint32_t exp = 0;
    std::vector<CharItem> items;
};

class CharacterDB {
public:
    CharacterDB();
    ~CharacterDB();
    bool Init(const std::string& path);

    std::vector<CharInfo> GetCharacters(const std::string& account_id);
    CharInfo GetCharacter(uint32_t char_id);
    uint32_t CreateCharacter(const CharInfo& info);
    bool SavePosition(uint32_t char_id, float x, float y, float z);
    bool SaveHP(uint32_t char_id, int32_t hp, int32_t mp);
    bool SaveItems(uint32_t char_id, const std::vector<CharItem>& items);
    std::vector<CharItem> LoadItems(uint32_t char_id);

private:
    sqlite3* db_ = nullptr;
};
