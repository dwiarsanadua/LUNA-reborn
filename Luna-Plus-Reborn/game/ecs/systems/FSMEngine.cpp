#include "FSMEngine.hpp"
#include "../../../engine/scripting/LuaEngine.h"
#include <spdlog/spdlog.h>
#include <algorithm>

void FSMInstance::AddTransition(FSMState from, FSMState to, FSMTrigger trigger,
                                const std::string& condition,
                                const std::string& action) {
    transitions.push_back({from, to, trigger, condition, action});
}

bool FSMInstance::EvaluateTransition(FSMTrigger trigger, LuaEngine* lua) {
    for (auto& t : transitions) {
        if (t.from == current_state && t.trigger == trigger) {
            if (!t.condition_script.empty() && lua) {
                std::string script = "return (" + t.condition_script + ")";
                lua->ExecuteString(script);
                int32_t result = lua->GetGlobalInt("__fsm_condition_result");
                if (result == 0) continue;
            }
            if (!t.action_script.empty() && lua) {
                lua->ExecuteString(t.action_script);
            }
            current_state = t.to;
            spdlog::debug("FSM '{}': transition {:d} -> {:d}", name,
                          static_cast<int>(t.from), static_cast<int>(t.to));
            if (current_state == FSMState::COMPLETE) {
                completed = true;
            }
            return true;
        }
    }
    return false;
}

FSMEngine::FSMEngine() {}
FSMEngine::~FSMEngine() {}

void FSMEngine::SetLuaEngine(LuaEngine* lua) { lua_ = lua; }

uint32_t FSMEngine::CreateInstance(const std::string& name) {
    uint32_t id = next_id_++;
    FSMInstance inst;
    inst.id = id;
    inst.name = name;
    inst.current_state = FSMState::IDLE;
    inst.completed = false;

    inst.AddTransition(FSMState::IDLE, FSMState::RUNNING, FSMTrigger::TIMER);
    inst.AddTransition(FSMState::RUNNING, FSMState::CONDITION_CHECK, FSMTrigger::KILL);
    inst.AddTransition(FSMState::RUNNING, FSMState::CONDITION_CHECK, FSMTrigger::ITEM);
    inst.AddTransition(FSMState::RUNNING, FSMState::CONDITION_CHECK, FSMTrigger::LEVEL);
    inst.AddTransition(FSMState::RUNNING, FSMState::CONDITION_CHECK, FSMTrigger::TALK);
    inst.AddTransition(FSMState::RUNNING, FSMState::CONDITION_CHECK, FSMTrigger::TIMER);
    inst.AddTransition(FSMState::CONDITION_CHECK, FSMState::REWARD, FSMTrigger::CUSTOM, "true");
    inst.AddTransition(FSMState::CONDITION_CHECK, FSMState::RUNNING, FSMTrigger::CUSTOM);
    inst.AddTransition(FSMState::REWARD, FSMState::COMPLETE, FSMTrigger::CUSTOM, "true");
    inst.AddTransition(FSMState::REWARD, FSMState::RUNNING, FSMTrigger::CUSTOM);

    instances_[id] = inst;
    spdlog::info("FSMEngine: created instance '{}' (id={})", name, id);
    return id;
}

// ═══════════════════════════════════════════════════════════════════════
//  CreateQuestInstance — sets up FSM transitions matching quest conditions
//  States: IDLE(NOT_STARTED) → RUNNING(IN_PROGRESS) → CONDITION_CHECK(COMPLETE)
//          → REWARD → COMPLETE(REWARDED)
// ═══════════════════════════════════════════════════════════════════════

uint32_t FSMEngine::CreateQuestInstance(const std::string& name,
                                         const std::vector<QuestConditionDesc>& conditions,
                                         uint32_t npc_start_id, uint32_t npc_complete_id) {
    uint32_t id = next_id_++;
    FSMInstance inst;
    inst.id = id;
    inst.name = name;
    inst.current_state = FSMState::IDLE; // NOT_STARTED
    inst.completed = false;

    // NOT_STARTED(IDLE) → IN_PROGRESS(RUNNING) via TALK to start NPC
    inst.AddTransition(FSMState::IDLE, FSMState::RUNNING, FSMTrigger::TALK,
                       "", "variables['accepted'] = 1");

    // IN_PROGRESS(RUNNING) → COMPLETE(CONDITION_CHECK) via each condition type
    for (auto& c : conditions) {
        FSMTrigger trig = FSMTrigger::CUSTOM;
        if (c.type == "kill") trig = FSMTrigger::KILL;
        else if (c.type == "collect") trig = FSMTrigger::ITEM;
        else if (c.type == "talk") trig = FSMTrigger::TALK;
        else if (c.type == "level") trig = FSMTrigger::LEVEL;

        // Set variable for target tracking
        inst.variables[c.type + "_target_" + std::to_string(c.target_id)] = c.count;
        inst.variables[c.type + "_current_" + std::to_string(c.target_id)] = 0;

        inst.AddTransition(FSMState::RUNNING, FSMState::CONDITION_CHECK, trig,
                           "", "variables['" + c.type + "_done'] = 1");
    }

    // COMPLETE(CONDITION_CHECK) → REWARD via TALK to complete NPC
    inst.AddTransition(FSMState::CONDITION_CHECK, FSMState::REWARD, FSMTrigger::TALK,
                       "", "variables['reward_ready'] = 1");

    // REWARD → COMPLETE(REWARDED) — auto-transition after reward given
    inst.AddTransition(FSMState::REWARD, FSMState::COMPLETE, FSMTrigger::CUSTOM, "true",
                       "variables['rewarded'] = 1");

    // Fallback: RUNNING → CONDITION_CHECK on any progress trigger
    inst.AddTransition(FSMState::RUNNING, FSMState::CONDITION_CHECK, FSMTrigger::CUSTOM,
                       "variables['all_done'] == 1", "");

    instances_[id] = inst;
    spdlog::info("FSMEngine: created quest FSM '{}' (id={}) with {} conditions", name, id, conditions.size());
    return id;
}

bool FSMEngine::RemoveInstance(uint32_t id) {
    auto it = instances_.find(id);
    if (it == instances_.end()) return false;
    instances_.erase(it);
    return true;
}

FSMInstance* FSMEngine::GetInstance(uint32_t id) {
    auto it = instances_.find(id);
    return it != instances_.end() ? &it->second : nullptr;
}

bool FSMEngine::SendTrigger(uint32_t instance_id, FSMTrigger trigger) {
    auto* inst = GetInstance(instance_id);
    if (!inst || inst->completed) return false;
    return inst->EvaluateTransition(trigger, lua_);
}

void FSMEngine::Update(uint32_t instance_id, float dt) {
    auto* inst = GetInstance(instance_id);
    if (!inst || inst->completed) return;

    if (inst->current_state == FSMState::IDLE) {
        inst->timer += dt;
        if (inst->timer >= inst->timer_threshold) {
            SendTrigger(instance_id, FSMTrigger::TIMER);
        }
        return;
    }

    if (inst->current_state == FSMState::RUNNING) {
        inst->timer += dt;
        if (inst->timer_threshold > 0.0f && inst->timer >= inst->timer_threshold) {
            SendTrigger(instance_id, FSMTrigger::TIMER);
            inst->timer = 0.0f;
        }
    }
}

bool FSMEngine::IsCompleted(uint32_t instance_id) const {
    auto it = instances_.find(instance_id);
    return it != instances_.end() && it->second.completed;
}

FSMState FSMEngine::GetState(uint32_t instance_id) const {
    auto it = instances_.find(instance_id);
    return it != instances_.end() ? it->second.current_state : FSMState::IDLE;
}

void FSMEngine::SetVariable(uint32_t instance_id, const std::string& key, int32_t value) {
    auto* inst = GetInstance(instance_id);
    if (inst) inst->variables[key] = value;
}

int32_t FSMEngine::GetVariable(uint32_t instance_id, const std::string& key) const {
    auto it = instances_.find(instance_id);
    if (it == instances_.end()) return 0;
    auto vit = it->second.variables.find(key);
    return vit != it->second.variables.end() ? vit->second : 0;
}
