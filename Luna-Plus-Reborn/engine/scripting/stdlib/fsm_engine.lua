-- Quest FSM engine for MapScriptRuntime (Lua 5.1 / LuaJIT compatible)
local M = {}

local function copy_table(t)
    local o = {}
    if t then
        for k, v in pairs(t) do
            o[k] = v
        end
    end
    return o
end

function M.new(def)
    local self = copy_table(def)
    self._state = (def.states and def.states[1] and def.states[1].name) or "NOT_STARTED"
    self._states = {}
    for _, st in ipairs(def.states or {}) do
        self._states[st.name] = st
    end

    function self:log(msg)
        print("[Quest " .. tostring(self.id or "?") .. "] " .. tostring(msg))
    end

    function self:give_rewards(rewards)
        rewards = rewards or {}
        local parts = {}
        if rewards.exp then table.insert(parts, "exp=" .. tostring(rewards.exp)) end
        if rewards.gold then table.insert(parts, "gold=" .. tostring(rewards.gold)) end
        if rewards.items then table.insert(parts, "items=" .. tostring(#rewards.items)) end
        self:log("Rewards: " .. table.concat(parts, ", "))
    end

    function self:get_state()
        return self._state
    end

    function self:_enter(state_name)
        self._state = state_name
        local st = self._states[state_name]
        if st and st.on_enter then
            st.on_enter(self)
        end
    end

    function self:dispatch(event, ctx)
        ctx = ctx or {}
        local st = self._states[self._state]
        if not st or not st.transitions then return false end
        for _, tr in ipairs(st.transitions) do
            if tr.trigger == event or tr.trigger == "any" then
                local ok = true
                if event == "kill" and tr.target_id then
                    ok = ctx.target_id == tr.target_id
                elseif event == "collect" and tr.item_id then
                    ok = ctx.item_id == tr.item_id
                elseif event == "npc_talk" and tr.npc_id then
                    ok = ctx.npc_id == tr.npc_id
                end
                if ok and tr.target then
                    if tr.action then tr.action(self) end
                    self:_enter(tr.target)
                    return true
                end
            end
        end
        return false
    end

    self:_enter(self._state)
    return self
end

return M
