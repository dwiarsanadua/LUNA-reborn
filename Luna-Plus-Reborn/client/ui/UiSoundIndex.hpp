#pragma once
#include <string>

class UiSoundIndex {
public:
    static void Init(const std::string& path = "assets/data/SoundList.bin.txt");
    static void Shutdown();

    static std::string Resolve(int sound_id);

private:
    static bool ready_;
};
