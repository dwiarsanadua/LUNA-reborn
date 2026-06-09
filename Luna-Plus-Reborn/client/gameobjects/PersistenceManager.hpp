#pragma once
#include <string>
#include <vector>
#include <cstdint>
#include <functional>
#include <sqlite3.h>

struct GameState;

enum class LoadErrorType : uint8_t {
    None = 0,
    CharacterListFailed,
    CharacterSelectFailed,
    GameEnterFailed,
    DbLoadFailed,
    NetworkTimeout,
    ServerRejected,
};

class PersistenceManager {
public:
    PersistenceManager() = default;
    ~PersistenceManager();

    bool Init(const std::string& db_path);
    void Shutdown();

    bool SaveGameState(const GameState& state);
    bool LoadGameState(GameState& state);

    bool SaveCharacter(GameState& state);
    bool SaveInventory(GameState& state);
    bool SaveQuest(GameState& state);

    void Update(float dt);
    void SetAutoSaveInterval(float sec) { auto_save_interval_ = sec; }
    bool GetAutoSaveDirty() const { return dirty_; }
    void ClearAutoSaveDirty() { dirty_ = false; }

    void SetLoadError(LoadErrorType type, const std::string& message);
    void ClearLoadError();
    LoadErrorType GetLoadErrorType() const { return load_error_type_; }
    const std::string& GetLoadErrorMessage() const { return load_error_message_; }
    bool HasLoadError() const { return load_error_type_ != LoadErrorType::None; }

    void SetRetryCallback(std::function<void()> callback);
    void TriggerRetry();
    void SetOnShowErrorDialog(std::function<void(const std::string&, std::function<void()>)> callback);

    const char* GetLoadErrorDescription(LoadErrorType type) const;

private:
    sqlite3* db_ = nullptr;
    float auto_save_timer_ = 0;
    float auto_save_interval_ = 60.0f;
    bool dirty_ = false;

    LoadErrorType load_error_type_ = LoadErrorType::None;
    std::string load_error_message_;
    std::function<void()> retry_callback_;
    std::function<void(const std::string&, std::function<void()>)> show_error_dialog_;

    bool CreateTables();
    bool EnsureCharacterRow(GameState& state);
};
