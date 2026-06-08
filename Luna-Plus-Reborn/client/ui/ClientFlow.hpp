#pragma once
#include <cstdint>
#include <vector>

struct GameState;
class ScreenManager;
class NetworkClient;
class EngineMap;

namespace ClientFlow {

void Init(ScreenManager* screens, EngineMap* map);

void StartOffline(GameState& state);
void OnLoginSuccess(GameState& state);
void BeginEnterGame(GameState& state);
void OnEnterWorldData(GameState& state, uint32_t map_id, float x, float y, float z);

void Update(GameState& state, float dt);

bool OnPacket(uint16_t type, const std::vector<uint8_t>& payload,
              GameState& state, NetworkClient& network);

}
