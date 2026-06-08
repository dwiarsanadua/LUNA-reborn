#include "ServerAuthMovement.hpp"
#include <algorithm>
#include <cmath>
#include <spdlog/spdlog.h>

void ServerAuthMovement::Init() {
    pending_commands_.clear();
    state_history_.clear();
    sequence_ = 0;
    correction_count_ = 0;
    spdlog::info("ServerAuthMovement: initialized");
}

MovementCommand ServerAuthMovement::CreateCommand(glm::vec3 target, float yaw, float speed) {
    MovementCommand cmd;
    cmd.sequence = sequence_++;
    cmd.target_pos = target;
    cmd.yaw = yaw;
    cmd.speed = speed;
    cmd.client_time = 0;
    pending_commands_.push_back(cmd);
    if ((int)pending_commands_.size() > MAX_HISTORY) pending_commands_.pop_front();
    return cmd;
}

bool ServerAuthMovement::ValidateMove(const MovementCommand& cmd, const MovementState& current_state,
                                       float max_speed, float max_teleport_dist, float dt) {
    if (CheckSpeedHack(cmd, max_speed)) {
        spdlog::warn("AuthMove: speed hack detected (cmd seq={})", cmd.sequence);
        return false;
    }
    if (CheckTeleportHack(current_state.position, cmd.target_pos, max_teleport_dist, dt)) {
        spdlog::warn("AuthMove: teleport hack detected (cmd seq={})", cmd.sequence);
        return false;
    }
    return true;
}

MovementState ServerAuthMovement::ComputeNewState(const MovementCommand& cmd, const MovementState& current_state, float dt) {
    MovementState new_state = current_state;
    glm::vec3 dir = cmd.target_pos - current_state.position;
    float dist = glm::length(dir);
    if (dist > 0.1f) {
        dir /= dist;
        float step = std::min(cmd.speed * dt, dist);
        new_state.position += dir * step;
        new_state.velocity = dir * cmd.speed;
    }
    new_state.yaw = cmd.yaw;
    new_state.speed = cmd.speed;
    new_state.timestamp++;
    return new_state;
}

void ServerAuthMovement::ApplyCorrection(const MovementState& server_state, uint32_t server_time) {
    (void)server_time;
    state_history_.push_back(current_state_);
    if (state_history_.size() > MAX_HISTORY) state_history_.pop_front();
    current_state_ = server_state;
    render_state_ = server_state;
    correction_count_++;
    
    // Clear old pending commands that are already processed by server
    while (!pending_commands_.empty() && pending_commands_.front().sequence < server_state.timestamp) {
        pending_commands_.pop_front();
    }
}

glm::vec3 ServerAuthMovement::GetRenderPosition() const {
    if (state_history_.size() < 2) return current_state_.position;

    auto& prev = state_history_[state_history_.size() - 2];
    auto& curr = state_history_.back();

    float t = (estimated_latency_ / 1000.0f) * 60.0f;
    t = std::clamp(t, 0.0f, 1.0f);

    return glm::mix(prev.position, curr.position, t);
}

bool ServerAuthMovement::CheckSpeedHack(const MovementCommand& cmd, float max_speed) const {
    return cmd.speed > max_speed * 1.5f;
}

bool ServerAuthMovement::CheckTeleportHack(const glm::vec3& from, const glm::vec3& to, float max_dist, float dt) const {
    float dist = glm::distance(from, to);
    if (dt > 0.001f) {
        float speed = dist / dt;
        return speed > max_dist * 2.0f;
    }
    return dist > max_dist;
}

bool ServerAuthMovement::CheckNoClip(const glm::vec3& pos, float terrain_height) const {
    return pos.y < terrain_height - 2.0f; // If player is below terrain, likely noclip
}
