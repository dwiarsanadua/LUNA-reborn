--[[
  Quest 212: [Revenge] Stop Staring!
  Level Required: 31
  NPC Start: 0, NPC Complete: 0
  Rewards: EXP=163304, Gold=3000
]]

local fsm = require('fsm_engine')

local quest_212 = fsm:new({
    id = 212,
    name = "[Revenge] Stop Staring!",
    level_required = 31,
    npc_start = 0,
    npc_complete = 0,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 212: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "accept",
                    target = "IN_PROGRESS",
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 212: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 54,
                    count = 20,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 212: Kill objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 212: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 0,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 212: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 212: Rewards given")
                self:give_rewards({
                    exp = 163304,
                    gold = 3000,
                    items = {
                        { item_id = 21000095, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_212