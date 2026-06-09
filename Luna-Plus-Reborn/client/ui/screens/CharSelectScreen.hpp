#pragma once
#include <ui/Screen.hpp>
#include <ui/dialogs/CharMakeDlg.hpp>
#include <string>
#include <vector>

struct CharSlotInfo {
    bool exists = false;
    uint32_t entity_id = 0;
    std::string name;
    int level = 0;
    int char_class = 0;
    std::string model_path;
};

class CharSelectScreen : public Screen {
public:
    void Init(GameState* state, NetworkClient* network) override;
    void Enter() override;
    void Exit() override;
    void Update(float dt) override;
    void Render(UIRenderer& ui) override;
    bool HandleKey(int key, int scancode, int action, int mods) override;
    bool HandleChar(unsigned int codepoint) override;
    bool HandlePacket(uint16_t type, const std::vector<uint8_t>& payload) override;
    void SetSceneClearer(std::function<void(uint32_t)> f) { set_clear_color_ = f; }

    void RequestCharList();
    void SendCharSelect(int slot);

private:
    std::function<void(uint32_t)> set_clear_color_;
    CharMakeDlg charmake_dlg_;
    bool charmake_open_ = false;

    std::vector<CharSlotInfo> slots_;
    int selected_slot_ = 0;
    bool waiting_for_list_ = true;
};
