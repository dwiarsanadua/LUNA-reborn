#pragma once
#include <string>
#include <unordered_map>
#include <cstdio>
#include "rendering/FontManager.hpp"

class Localization {
public:
    enum Lang { EN, ID, KO, JA, ZH, _COUNT };

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

        // Korean
        instance().strings_[KO]["login.title"] = "\xB7\xEC\xB3\xA0 Plus Reborn";
        instance().strings_[KO]["login.select_account"] = "1-3: \xC1\xB8\xC1\xA4  Enter: \xB7\xCE\xC7\xCF\xC0\xCE";
        instance().strings_[KO]["login.error"] = "\xC0\xCC\xBF\xAA\xB8\xED \xB5\xC7\xC0\xCC \xC6\xF2\xC0\xCC \xB8\xB8\xC7\xC7\xC1\xF6 \xB8\xA2\xBD\xBA\xB4\xCF\xB4\xD9";
        instance().strings_[KO]["hud.gold"] = "\xC6\xF0\xB7\xAF";
        instance().strings_[KO]["charselect.title"] = "\xC4\xDC\xB8\xAF\xC5\xCD \xC1\xB8\xC1\xA4";
        instance().strings_[KO]["charselect.hint"] = "Enter: \xC7\xD5\xB5\xBF  C: \xC4\xDC\xB8\xAF\xC5\xCD \xC0\xDB\xBC\xBA  ESC: \xB5\xE9\xBE\xAE\xB0\xA1\xB1\xE2";

        // Japanese
        instance().strings_[JA]["login.title"] = "LUNA Plus Reborn";
        instance().strings_[JA]["login.select_account"] = "1-3: \xE9\x81\xB8\xE6\x8A\x9E  Enter: \xE3\x83\xAD\xE3\x82\xB0\xE3\x82\xA4\xE3\x83\xB3";
        instance().strings_[JA]["login.error"] = "\xE3\x82\xA2\xE3\x82\xAB\xE3\x82\xA6\xE3\x83\xB3\xE3\x83\x88\xE3\x81\xBE\xE3\x81\x9F\xE3\x81\xAF\xE3\x83\x91\xE3\x82\xB9\xE3\x83\xAF\xE3\x83\xBC\xE3\x83\x89\xE3\x81\x8C\xE9\x96\x93\xE9\x81\x95\xE3\x81\xA3\xE3\x81\xA6\xE3\x81\x84\xE3\x81\xBE\xE3\x81\x99";
        instance().strings_[JA]["hud.gold"] = "\xE3\x82\xB4\xE3\x83\xBC\xE3\x83\xAB\xE3\x83\x89";
        instance().strings_[JA]["charselect.title"] = "\xE3\x82\xAD\xE3\x83\xA3\xE3\x83\xA9\xE3\x82\xAF\xE3\x82\xBF\xE2\x80\x90\xE9\x81\xB8\xE6\x8A\x9E";
        instance().strings_[JA]["charselect.hint"] = "Enter: \xE5\x8F\x82\xE5\x8A\xA0  C: \xE4\xBD\x9C\xE6\x88\x90  ESC: \xE6\x88\xBB\xE3\x82\x8B";

        // Chinese
        instance().strings_[ZH]["login.title"] = "LUNA Plus Reborn";
        instance().strings_[ZH]["login.select_account"] = "1-3: \xE9\x80\x89\xE6\x8B\xA9  Enter: \xE7\x99\xBB\xE5\xBD\x95";
        instance().strings_[ZH]["login.error"] = "\xE8\xB4\xA6\xE6\x88\xB7\xE5\x90\x8D\xE6\x88\x96\xE5\xAF\x86\xE7\xA0\x81\xE9\x94\x99\xE8\xAF\xAF";
        instance().strings_[ZH]["hud.gold"] = "\xE9\x87\x91\xE5\xB8\x81";
        instance().strings_[ZH]["charselect.title"] = "\xE9\x80\x89\xE6\x8B\xA9\xE8\xA7\x92\xE8\x89\xB2";
        instance().strings_[ZH]["charselect.hint"] = "Enter: \xE8\xBF\x9B\xE5\x85\xA5  C: \xE5\x88\x9B\xE5\xBB\xBA  ESC: \xE8\xBF\x94\xE5\x9B\x9E";
    }

    static void SetLanguage(Lang lang) {
        instance().current_ = lang;
        Language fb_lang = Language::English;
        switch (lang) {
            case KO: fb_lang = Language::Korean; break;
            case JA: fb_lang = Language::Japanese; break;
            case ZH: fb_lang = Language::Chinese; break;
            default: break;
        }
        FontManager::Instance().SetLanguage(fb_lang);
    }

    static Lang GetLanguage() { return instance().current_; }
    static Language GetFontLanguage() { return FontManager::Instance().GetLanguage(); }

    static const char* Get(const char* key) {
        auto& inst = instance();
        auto it = inst.strings_[inst.current_].find(key);
        if (it != inst.strings_[inst.current_].end()) return it->second.c_str();
        it = inst.strings_[EN].find(key);
        if (it != inst.strings_[EN].end()) return it->second.c_str();
        return key;
    }

    static int LoadAdditionalStrings(const std::string& path);

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
