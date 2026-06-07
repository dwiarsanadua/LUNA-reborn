#include "ScreenManager.hpp"

void ScreenManager::Init(GameState* state, NetworkClient* network) {
    state_ = state;
    network_ = network;
}

void ScreenManager::Register(const std::string& name, std::unique_ptr<Screen> screen) {
    screen->Init(state_, network_);
    screens_[name] = std::move(screen);
}

void ScreenManager::SwitchTo(const std::string& name) {
    auto it = screens_.find(name);
    if (it == screens_.end()) return;
    if (current_) { current_->Exit(); }
    current_ = it->second.get();
    current_name_ = name;
    current_->Enter();
}

void ScreenManager::Update(float dt) {
    if (current_) current_->Update(dt);
}

void ScreenManager::Render(UIRenderer& ui) {
    if (current_) current_->Render(ui);
}

void ScreenManager::Render(UIRenderer& ui, const glm::mat4& view, const glm::mat4& proj) {
    if (current_) current_->Render(ui, view, proj);
}

bool ScreenManager::HandleKey(int key, int scancode, int action, int mods) {
    return current_ ? current_->HandleKey(key, scancode, action, mods) : false;
}

bool ScreenManager::HandleChar(unsigned int codepoint) {
    return current_ ? current_->HandleChar(codepoint) : false;
}

bool ScreenManager::HandlePacket(uint16_t type, const std::vector<uint8_t>& payload) {
    return current_ ? current_->HandlePacket(type, payload) : false;
}
