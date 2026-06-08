#pragma once
#include <glm/glm.hpp>
#include <unordered_map>

namespace luna::protocol {
struct AttackResult;
struct ChangeMapResponse;
struct PartyResponse;
struct StorageResponse;
struct FriendResponse;
struct GuildResponse;
struct TradeResponse;
struct ConsignmentResponse;
struct StreetStallResponse;
}

struct EntityInterp {
    glm::vec3 from{0.0f};
    glm::vec3 to{0.0f};
    float t = 1.0f;
    float duration = 0.12f;
};
#include <ui/Screen.hpp>
#include <ui/WindowManager.hpp>
#include <ui/LegacyHudOverlay.hpp>
#include <ui/ScriptDialogPanel.hpp>
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
#include <gameobjects/NavMeshSystem.hpp>
#include <gameobjects/ObjectBalloon.hpp>
#include <ui/dialogs/CharMakeDlg.hpp>
#include <ui/dialogs/WorldMapDlg.hpp>
#include <ui/dialogs/MiniMapDlg.hpp>
#include <ui/dialogs/HelperDlg.hpp>
#include <ui/dialogs/FadeDlg.hpp>
#include <ui/dialogs/PKManagerDlg.hpp>
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
    void SetGameDataDB(class GameDataDB* db) { gamedb_ = db; }
    WindowManager* GetWindowManager() { return &wm_; }

private:
    void SpawnRandomMonster();
    void SpawnMonstersFromMap();
    void InitializeWorld();
    void ChangeMap(uint32_t map_id);
    void OnChangeMapAck(const luna::protocol::ChangeMapResponse* resp);
    void ClearNetworkEntities();
    void ApplyPartyResponse(const luna::protocol::PartyResponse* resp);
    void ApplyStorageResponse(const luna::protocol::StorageResponse* resp);
    void SendPartyCreate();
    void SendPartyInvite(const std::string& name);
    void SendPartyLeave();
    void RequestStorageList();
    void SendStorageDeposit(uint8_t inv_slot, uint16_t count);
    void SendStorageWithdraw(uint8_t storage_slot, uint16_t count);
    void ApplyFriendResponse(const luna::protocol::FriendResponse* resp);
    void ApplyGuildResponse(const luna::protocol::GuildResponse* resp);
    void RequestFriendList();
    void SendFriendAdd(const std::string& name);
    void SendFriendDelete(uint32_t friend_id);
    void SendGuildCreate(const std::string& name);
    void RequestGuildInfo();
    void SendGuildInvite(const std::string& name);
    void SendGuildLeave();
    void ApplyTradeResponse(const luna::protocol::TradeResponse* resp);
    void SendTradeApply(const std::string& target);
    void SendTradeCancel();
    void SendTradeAddItem(uint8_t inv_slot);
    void SendTradeSetGold(uint32_t gold);
    void SendTradeConfirm();
    void SetupTradeNetworkCallbacks();
    void ApplyConsignmentResponse(const luna::protocol::ConsignmentResponse* resp,
        bool mine_only, bool bids_only);
    void ApplyStreetStallResponse(const luna::protocol::StreetStallResponse* resp);
    void SetupConsignmentNetworkCallbacks();
    void RequestConsignmentSearch(const std::string& query);
    void RequestConsignmentRefresh(bool mine, bool bids);
    void SendConsignmentList(uint8_t inv_slot, uint16_t count, uint32_t bid, uint32_t buyout);
    void SendConsignmentTrade(uint64_t listing_id, bool buyout, uint32_t amount);
    void SendConsignmentCancel(uint64_t listing_id);
    void SendStreetStallOpen(const std::string& title);
    void SendStreetStallAdd(uint8_t inv_slot, uint32_t price);
    void SendStreetStallBuy(uint32_t owner_id, uint8_t stall_slot);
    void SendStreetStallClose();
    void RequestStreetStallList();
    uint32_t GetSelectedCharId() const;
    void CastHotbarSkill(int slot);
    void ApplySkillDamage(uint32_t skill_id);
    void ApplyNetworkAttackResult(const luna::protocol::AttackResult* result);
    void RemoveNetworkEntity(uint32_t entity_id);
    void UpdateEntityInterpolation(float dt);
    void BeginEntityInterpolation(uint32_t entity_id, float x, float y, float z);
    void SendMovementUpdate(float dt);
    void DoCombat(float dt);
    void DoLevelUp();
    void RenderUI(UIRenderer& ui);

    TerrainRenderer* terrain_ = nullptr;
    PropRenderer* props_ = nullptr;
    ParticleRenderer* particles_ = nullptr;
    AudioManager* audio_ = nullptr;
    UIRenderer* ui_ = nullptr;
    EngineMap* map_ = nullptr;
    class GameDataDB* gamedb_ = nullptr;

    // Game Objects
    Hero hero_;
    std::vector<Monster> monsters_;
    NavMeshSystem navmesh_;
    FadeDlg fade_dlg_;
    MiniMapDlg minimap_dlg_;
    HelperDlg helper_dlg_;
    WorldMapDlg worldmap_dlg_;
    PKManagerDlg pk_dlg_;
    uint32_t pending_map_id_ = 0;
    bool changemap_pending_ = false;
    bool pending_consignment_mine_ = false;
    bool pending_consignment_bids_ = false;
    float move_send_timer_ = 0.0f;
    bool skill_damage_applied_ = false;
    std::unordered_map<uint32_t, EntityInterp> entity_interp_;
    
    ParticleSystem particleSys_;
    float prev_x_ = 0, prev_z_ = 0;

    // Sky & Environment
    EngineSky sky_;
    WeatherSystem weather_;
    
    WindowManager wm_;
    LegacyHudOverlay legacy_hud_;
    ScriptDialogPanel script_dialogs_;
    void InitScriptDialogs();

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
