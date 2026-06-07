#pragma once
#include "Screen.hpp"
#include <rendering/UIRenderer.hpp>
#include <glm/glm.hpp>
#include <memory>
#include <unordered_map>
#include <string>

struct GameState;
class NetworkClient;

class ScreenManager {
public:
    void Init(GameState* state, NetworkClient* network);
    void Register(const std::string& name, std::unique_ptr<Screen> screen);
    void SwitchTo(const std::string& name);
    void Update(float dt);
    void Render(UIRenderer& ui);
    void Render(UIRenderer& ui, const glm::mat4& view, const glm::mat4& proj);
    bool HandleKey(int key, int scancode, int action, int mods);
    bool HandleChar(unsigned int codepoint);
    bool HandlePacket(uint16_t type, const std::vector<uint8_t>& payload);
    Screen* Current() { return current_; }
    const std::string& CurrentName() { return current_name_; }
private:
    std::unordered_map<std::string, std::unique_ptr<Screen>> screens_;
    Screen* current_ = nullptr;
    std::string current_name_;
    GameState* state_ = nullptr;
    NetworkClient* network_ = nullptr;
};
