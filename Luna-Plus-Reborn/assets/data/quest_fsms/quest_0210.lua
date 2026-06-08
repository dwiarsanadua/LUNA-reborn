--[[
  Quest 210: [Revenge] Lost Money
  Level Required: 23
  NPC Start: 0, NPC Complete: 0
  Rewards: EXP=33168, Gold=2200
]]

local fsm = require('fsm_engine')

local quest_210 = fsm:new({
    id = 210,
    name = "[Revenge] Lost Money",
    level_required = 23,
    npc_start = 0,
    npc_complete = 0,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 210: Awaiting acceptance")
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
                self:log("Quest 210: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 15,
                    count = 20,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 210: Kill objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 210: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 0,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 210: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 210: Rewards given")
                self:give_rewards({
                    exp = 33168,
                    gold = 2200,
                    items = {
                        { item_id = 21000010, count = 3 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_210