--[[
  Quest 2005: [Daily] Procurement 3
  Level Required: 50
  NPC Start: 411, NPC Complete: 411
  Rewards: EXP=138983
]]

local fsm = require('fsm_engine')

local quest_2005 = fsm:new({
    id = 2005,
    name = "[Daily] Procurement 3",
    level_required = 50,
    npc_start = 411,
    npc_complete = 411,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 2005: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 411,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 2005: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 2005: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 411,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2005: NPC talk objective met")
                    end
                },
                {
                    trigger = "any",
                    target = "COMPLETE",
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 2005: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 411,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 2005: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 2005: Rewards given")
                self:give_rewards({
                    exp = 138983,
                    items = {
                        { item_id = 30000704, count = 10 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_2005