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
