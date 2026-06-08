#pragma once
#include <cstdint>
#include <glm/glm.hpp>
#include <vector>
#include <deque>

struct MovementState {
    glm::vec3 position{0};
    glm::vec3 velocity{0};
    float yaw = 0;
    float speed = 0;
    uint32_t timestamp = 0;
    uint32_t server_time = 0;
};

struct MovementCommand {
    uint32_t sequence = 0;
    glm::vec3 target_pos{0};
    float yaw = 0;
    float speed = 0;
    uint32_t client_time = 0;
};

class ServerAuthMovement {
public:
    void Init();
    
    // Client-side: send movement command
    MovementCommand CreateCommand(glm::vec3 target, float yaw, float speed);
    
    // Server-side: validate and correct
    bool ValidateMove(const MovementCommand& cmd, const MovementState& current_state,
                      float max_speed, float max_teleport_dist, float dt);
    
    // Server-side: compute new position from command
    MovementState ComputeNewState(const MovementCommand& cmd, const MovementState& current_state, float dt);
    
    // Client-side: apply server correction with interpolation
    void ApplyCorrection(const MovementState& server_state, uint32_t server_time);
    
    // Client-side: get interpolated position for rendering
    glm::vec3 GetRenderPosition() const;
    
    // Anti-cheat checks
    bool CheckSpeedHack(const MovementCommand& cmd, float max_speed) const;
    bool CheckTeleportHack(const glm::vec3& from, const glm::vec3& to, float max_dist, float dt) const;
    bool CheckNoClip(const glm::vec3& pos, float terrain_height) const;
    
    // Stats
    int GetCommandCount() const { return (int)pending_commands_.size(); }
    int GetCorrectionCount() const { return correction_count_; }
    
    void SetLatency(float ms) { estimated_latency_ = ms; }

private:
    std::deque<MovementCommand> pending_commands_;
    std::deque<MovementState> state_history_;
    MovementState current_state_;
    MovementState render_state_;
    uint32_t sequence_ = 0;
    int correction_count_ = 0;
    float estimated_latency_ = 50.0f; // ms
    
    static constexpr int MAX_HISTORY = 120;
    static constexpr float MAX_SPEED = 8.0f;
    static constexpr float MAX_TELEPORT_DIST = 20.0f;
};
