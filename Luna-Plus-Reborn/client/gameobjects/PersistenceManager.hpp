#pragma once
#include <string>
#include <vector>
#include <cstdint>
#include <sqlite3.h>

struct GameState;

class PersistenceManager {
public:
    PersistenceManager() = default;
    ~PersistenceManager();

    bool Init(const std::string& db_path);
    void Shutdown();

    // Save/Load full game state
    bool SaveGameState(const GameState& state);
    bool LoadGameState(GameState& state);

    // Individual saves
    bool SaveCharacter(GameState& state);
    bool SaveInventory(GameState& state);
    bool SaveQuest(GameState& state);

    // Auto-save
    void Update(float dt);
    void SetAutoSaveInterval(float sec) { auto_save_interval_ = sec; }
    bool GetAutoSaveDirty() const { return dirty_; }
    void ClearAutoSaveDirty() { dirty_ = false; }

private:
    sqlite3* db_ = nullptr;
    float auto_save_timer_ = 0;
    float auto_save_interval_ = 60.0f;
    bool dirty_ = false;

    bool CreateTables();
    bool EnsureCharacterRow(GameState& state);
};
