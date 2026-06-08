--[[
  Quest 880: Nera Castle Death Insurance
  Level Required: 94
  NPC Start: 0, NPC Complete: 0
  Rewards: EXP=6450627, Gold=61188
]]

local fsm = require('fsm_engine')

local quest_880 = fsm:new({
    id = 880,
    name = "Nera Castle Death Insurance",
    level_required = 94,
    npc_start = 0,
    npc_complete = 0,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 880: Awaiting acceptance")
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
                self:log("Quest 880: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 289,
                    count = 100,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 880: Kill objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 880: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 0,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 880: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 880: Rewards given")
                self:give_rewards({
                    exp = 6450627,
                    gold = 61188,
                })
            end,
            transitions = {},
        },
    },
})

return quest_880