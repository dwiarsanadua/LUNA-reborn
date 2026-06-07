#pragma once
#include <ui/Screen.hpp>
#include <ui/WindowManager.hpp>
#include <ui/dialogs/InventoryDialog.hpp>
#include <ui/dialogs/SkillDialog.hpp>
#include <ui/dialogs/QuestDialog.hpp>
#include <ui/dialogs/NPCDialog.hpp>
#include <ui/dialogs/ChatPanel.hpp>
#include <ui/dialogs/PartyDialog.hpp>
#include <ui/dialogs/GuildDialog.hpp>
#include <ui/dialogs/FriendDialog.hpp>
#include <ui/dialogs/OptionsDialog.hpp>
#include <ui/dialogs/StorageDialog.hpp>
#include <ui/dialogs/FishingDialog.hpp>
#include <ui/dialogs/TradeDialog.hpp>
#include <ui/dialogs/ConsignmentDialog.hpp>
#include <gameobjects/ConsignmentSystem.hpp>
#include <ui/dialogs/FamilyDialog.hpp>
#include <gameobjects/FamilySystem.hpp>
#include <gameobjects/SiegeSystem.hpp>
#include <ui/dialogs/HousingDialog.hpp>
#include <ui/dialogs/DungeonDialog.hpp>
#include <gameobjects/DungeonSystem.hpp>
#include <ui/dialogs/CostumeDialog.hpp>
#include <gameobjects/CostumeSystem.hpp>
#include <ui/dialogs/CharacterDialog.hpp>
#include <ui/dialogs/PetDialog.hpp>
#include <ui/dialogs/CookingDialog.hpp>
#include <ui/dialogs/MountDialog.hpp>
#include <ui/dialogs/TournamentDialog.hpp>
#include <ui/dialogs/AvatarDialog.hpp>
#include <ui/dialogs/UpgradeDialog.hpp>
#include <ui/dialogs/MacroDialog.hpp>
#include <gameobjects/Pet.hpp>
#include <gameobjects/TradingSystem.hpp>
#include <gameobjects/TelemetrySystem.hpp>
#include <gameobjects/PatchSystem.hpp>
#include <gameobjects/EconomyMonitor.hpp>
#include <ui/dialogs/MailDialog.hpp>
#include <ui/dialogs/KeyBindDialog.hpp>
#include <ui/dialogs/ChatRoomDialog.hpp>
#include <ui/dialogs/CashShopDialog.hpp>
#include <gameobjects/CashShopSystem.hpp>
#include <gameobjects/OperationsSystem.hpp>
#include <rendering/TerrainRenderer.hpp>
#include <rendering/PropRenderer.hpp>
#include <rendering/ParticleRenderer.hpp>
#include <ecs/systems/ParticleSystem.hpp>
#include <audio/AudioManager.hpp>
#include <effects/EffectManager.hpp>
#include <gameobjects/DurabilitySystem.hpp>
#include <gameobjects/BuffSystem.hpp>
#include <gameobjects/FarmSystem.hpp>
#include <gameobjects/Hero.hpp>
#include <gameobjects/Monster.hpp>
#include <gameobjects/ObjectBalloon.hpp>
#include <engine/EngineMap.hpp>
#include <engine/EngineSky.hpp>
#include <effects/WeatherSystem.hpp>

class GameScreen : public Screen {
public:
    GameScreen() = default;
    void Init(GameState* state, NetworkClient* network) override;
    void Enter() override;
    void Exit() override;
    void Update(float dt) override;
    void Render(UIRenderer& ui) override;
    void Render(UIRenderer& ui, const glm::mat4& view, const glm::mat4& proj) override;
    bool HandleKey(int key, int scancode, int action, int mods) override;
    bool HandleChar(unsigned int codepoint) override;
    bool HandlePacket(uint16_t type, const std::vector<uint8_t>& payload) override;

    void SetTerrain(TerrainRenderer* t) { terrain_ = t; }
    void SetProps(PropRenderer* p) { props_ = p; }
    void SetParticles(ParticleRenderer* p) { particles_ = p; }
    void SetAudio(AudioManager* a) { audio_ = a; }
    void SetMap(EngineMap* m) { map_ = m; }
    void SetUI(UIRenderer* u) { ui_ = u; }
    WindowManager* GetWindowManager() { return &wm_; }

private:
    void SpawnRandomMonster();
    void DoCombat(float dt);
    void DoLevelUp();
    void RenderUI(UIRenderer& ui);

    TerrainRenderer* terrain_ = nullptr;
    PropRenderer* props_ = nullptr;
    ParticleRenderer* particles_ = nullptr;
    AudioManager* audio_ = nullptr;
    UIRenderer* ui_ = nullptr;
    EngineMap* map_ = nullptr;
    
    // Game Objects
    Hero hero_;
    std::vector<Monster> monsters_;
    
    ParticleSystem particleSys_;
    float prev_x_ = 0, prev_z_ = 0;

    // Sky & Environment
    EngineSky sky_;
    WeatherSystem weather_;
    
    // Scripted Window Manager
    WindowManager wm_;

    // Durability
    DurabilitySystem durability_;
    BuffSystem buff_sys_;

    // Farming
    FarmSystem farm_;
    
    // Effects
    EffectManager effect_mgr_;
    
    // Dialogs
    InventoryDialog inv_dlg_;
    SkillDialog skill_dlg_;
    QuestDialog quest_dlg_;
    NPCDialog npc_dlg_;
    ChatPanel chat_panel_;
    PartyDialog party_dlg_;
    GuildDialog guild_dlg_;
    FriendDialog friend_dlg_;
    OptionsDialog options_dlg_;
    StorageDialog storage_dlg_;
    FishingDialog fishing_dlg_;
    TradeDialog trade_dlg_;
    ConsignmentSystem consignment_;
    ConsignmentDialog consignment_dlg_;
    FamilySystem family_;
    FamilyDialog family_dlg_;
    SiegeSystem siege_;
    HousingDialog housing_dlg_;
    DungeonSystem dungeon_sys_;
    DungeonDialog dungeon_dlg_;
    CostumeSystem costume_;
    CostumeDialog costume_dlg_;
    TradingSystem trading_;
    CashShopSystem cash_shop_;
    CashShopDialog cash_shop_dlg_;
    OperationsSystem ops_;
    TelemetrySystem telemetry_;
    PatchSystem patcher_;
    EconomyMonitor economy_;
    MailDialog mail_dlg_;
    KeyBindDialog keybind_dlg_;
    ChatRoomDialog chatroom_dlg_;
    CharacterDialog char_dlg_;
    Pet pet_;
    PetDialog pet_dlg_;
    CookingDialog cooking_dlg_;
    MountDialog mount_dlg_;
    TournamentSystem tournament_sys_;
    TournamentDialog tournament_dlg_;
    AvatarDialog avatar_dlg_;
    UpgradeDialog upgrade_dlg_;
    MacroDialog macro_dlg_;
};
