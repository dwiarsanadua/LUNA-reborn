#pragma once
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>
#include <functional>
#include <memory>

class LuaEngine;

enum class FSMState : uint8_t {
    IDLE = 0,           // NOT_STARTED
    RUNNING = 1,        // IN_PROGRESS
    CONDITION_CHECK = 2, // COMPLETE (objectives done, waiting turn-in)
    REWARD = 3,         // turning in
    COMPLETE = 4        // REWARDED (terminal)
};

enum class FSMTrigger : uint8_t {
    KILL = 0,
    ITEM = 1,
    LEVEL = 2,
    TALK = 3,
    TIMER = 4,
    CUSTOM = 5
};

struct FSMTransition {
    FSMState from;
    FSMState to;
    FSMTrigger trigger;
    std::string condition_script;
    std::string action_script;
};

struct FSMInstance {
    uint32_t id;
    std::string name;
    FSMState current_state = FSMState::IDLE;
    std::unordered_map<std::string, int32_t> variables;
    std::vector<FSMTransition> transitions;
    float timer = 0.0f;
    float timer_threshold = 0.0f;
    bool completed = false;

    void AddTransition(FSMState from, FSMState to, FSMTrigger trigger,
                       const std::string& condition = "",
                       const std::string& action = "");
    bool EvaluateTransition(FSMTrigger trigger, LuaEngine* lua);

    // Quest state helpers
    bool IsInProgress() const { return current_state == FSMState::RUNNING; }
    bool IsComplete() const { return current_state >= FSMState::CONDITION_CHECK && !completed; }
};

struct QuestConditionDesc {
    std::string type;   // "kill", "collect", "talk", "level"
    uint32_t target_id = 0;
    uint16_t count = 1;
};

class FSMEngine {
public:
    FSMEngine();
    ~FSMEngine();

    void SetLuaEngine(LuaEngine* lua);

    uint32_t CreateInstance(const std::string& name);
    uint32_t CreateQuestInstance(const std::string& name,
                                  const std::vector<QuestConditionDesc>& conditions,
                                  uint32_t npc_start_id, uint32_t npc_complete_id);
    bool RemoveInstance(uint32_t id);
    FSMInstance* GetInstance(uint32_t id);

    bool SendTrigger(uint32_t instance_id, FSMTrigger trigger);
    void Update(uint32_t instance_id, float dt);

    bool IsCompleted(uint32_t instance_id) const;
    FSMState GetState(uint32_t instance_id) const;

    void SetVariable(uint32_t instance_id, const std::string& key, int32_t value);
    int32_t GetVariable(uint32_t instance_id, const std::string& key) const;

private:
    std::unordered_map<uint32_t, FSMInstance> instances_;
    uint32_t next_id_ = 1;
    LuaEngine* lua_ = nullptr;
};
