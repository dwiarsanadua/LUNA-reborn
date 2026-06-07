#pragma once
#include <string>
#include <unordered_map>
#include <cstdio>

class Localization {
public:
    enum Lang { EN, ID, _COUNT };

    static void Init() {
        instance();
        // English
        instance().strings_[EN]["login.title"] = "LUNA Plus Reborn";
        instance().strings_[EN]["login.version"] = "v1.1.0";
        instance().strings_[EN]["login.select_account"] = "1-3: Select   Enter: Login";
        instance().strings_[EN]["login.error"] = "Invalid credentials";
        instance().strings_[EN]["login.connecting"] = "Connecting...";
        instance().strings_[EN]["charselect.title"] = "SELECT CHARACTER";
        instance().strings_[EN]["charselect.no_chars"] = "No characters. Press C to create";
        instance().strings_[EN]["charselect.hint"] = "Enter: Play   C: Create   ESC: Back";
        instance().strings_[EN]["hud.hp"] = "HP";
        instance().strings_[EN]["hud.mp"] = "MP";
        instance().strings_[EN]["hud.exp"] = "EXP";
        instance().strings_[EN]["hud.gold"] = "Gold";
        instance().strings_[EN]["hud.kills"] = "Kills";
        instance().strings_[EN]["hud.atk"] = "ATK";
        instance().strings_[EN]["hud.def"] = "DEF";
        instance().strings_[EN]["hud.map"] = "Map";
        instance().strings_[EN]["hud.server"] = "Server";
        instance().strings_[EN]["hud.connected"] = "Connected";
        instance().strings_[EN]["hud.disconnected"] = "Disconnected";
        instance().strings_[EN]["hud.fps"] = "FPS";
        instance().strings_[EN]["hotbar.hint"] = "WASD:Move  SPACE:Spawn  I:Inv  K:Skills  J:Quests  T:Chat";
        instance().strings_[EN]["inv.title"] = "INVENTORY";
        instance().strings_[EN]["inv.close"] = "I: Close";
        instance().strings_[EN]["skill.title"] = "SKILLS";
        instance().strings_[EN]["skill.close"] = "K: Close";
        instance().strings_[EN]["quest.title"] = "QUEST JOURNAL";
        instance().strings_[EN]["quest.close"] = "J: Close";
        instance().strings_[EN]["quest.tracker"] = "QUEST TRACKER";
        instance().strings_[EN]["npc.title"] = "NPC DIALOG";
        instance().strings_[EN]["npc.close"] = "Press N to close";
        instance().strings_[EN]["party.title"] = "PARTY";
        instance().strings_[EN]["guild.title"] = "GUILD";
        instance().strings_[EN]["friends.title"] = "FRIENDS";
        instance().strings_[EN]["target.title"] = "TARGET";
        instance().strings_[EN]["minimap.title"] = "MINIMAP";
        instance().strings_[EN]["chat.placeholder"] = "Chat...";
        instance().strings_[EN]["equip.title"] = "EQUIPMENT";
        instance().strings_[EN]["loading.tip"] = "Loading game data...";

        // Indonesian
        instance().strings_[ID]["login.title"] = "LUNA Plus Reborn";
        instance().strings_[ID]["login.version"] = "v1.1.0";
        instance().strings_[ID]["login.select_account"] = "1-3: Pilih   Enter: Masuk";
        instance().strings_[ID]["login.error"] = "Kata sandi salah";
        instance().strings_[ID]["login.connecting"] = "Menghubungkan...";
        instance().strings_[ID]["charselect.title"] = "PILIH KARAKTER";
        instance().strings_[ID]["charselect.no_chars"] = "Tidak ada karakter. Tekan C untuk buat";
        instance().strings_[ID]["charselect.hint"] = "Enter: Main   C: Buat   ESC: Kembali";
        instance().strings_[ID]["hud.hp"] = "HP";
        instance().strings_[ID]["hud.mp"] = "MP";
        instance().strings_[ID]["hud.exp"] = "EXP";
        instance().strings_[ID]["hud.gold"] = "Emas";
        instance().strings_[ID]["hud.kills"] = "Bunuh";
        instance().strings_[ID]["hud.atk"] = "ATK";
        instance().strings_[ID]["hud.def"] = "DEF";
        instance().strings_[ID]["hud.map"] = "Peta";
        instance().strings_[ID]["hud.server"] = "Server";
        instance().strings_[ID]["hud.connected"] = "Terhubung";
        instance().strings_[ID]["hud.disconnected"] = "Terputus";
        instance().strings_[ID]["hud.fps"] = "FPS";
        instance().strings_[ID]["hotbar.hint"] = "WASD:Gerak  SPACE:Munculkan  I:Inv  K:Skill  J:Quest  T:Chat";
        instance().strings_[ID]["inv.title"] = "INVENTARIS";
        instance().strings_[ID]["inv.close"] = "I: Tutup";
        instance().strings_[ID]["skill.title"] = "SKILL";
        instance().strings_[ID]["skill.close"] = "K: Tutup";
        instance().strings_[ID]["quest.title"] = "BUKU QUEST";
        instance().strings_[ID]["quest.close"] = "J: Tutup";
        instance().strings_[ID]["quest.tracker"] = "PELACAK QUEST";
        instance().strings_[ID]["npc.title"] = "DIALOG NPC";
        instance().strings_[ID]["npc.close"] = "Tekan N untuk tutup";
        instance().strings_[ID]["party.title"] = "PARTY";
        instance().strings_[ID]["guild.title"] = "GUILD";
        instance().strings_[ID]["friends.title"] = "TEMAN";
        instance().strings_[ID]["target.title"] = "SASARAN";
        instance().strings_[ID]["minimap.title"] = "PETA";
        instance().strings_[ID]["chat.placeholder"] = "Chat...";
        instance().strings_[ID]["equip.title"] = "PERLENGKAPAN";
        instance().strings_[ID]["loading.tip"] = "Memuat data game...";
    }

    static void SetLanguage(Lang lang) { instance().current_ = lang; }
    static Lang GetLanguage() { return instance().current_; }

    static const char* Get(const char* key) {
        auto& inst = instance();
        auto it = inst.strings_[inst.current_].find(key);
        if (it != inst.strings_[inst.current_].end()) return it->second.c_str();
        // Fallback to English
        it = inst.strings_[EN].find(key);
        if (it != inst.strings_[EN].end()) return it->second.c_str();
        return key;
    }

private:
    static Localization& instance() {
        static Localization loc;
        return loc;
    }
    Localization() = default;
    Lang current_ = EN;
    std::unordered_map<std::string, std::string> strings_[_COUNT];
};

#define TR(key) Localization::Get(key)
