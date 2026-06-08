--[[
  Quest 218: [Revenge] Stone Cold
  Level Required: 45
  NPC Start: 0, NPC Complete: 0
  Rewards: EXP=100000, Gold=13000
]]

local fsm = require('fsm_engine')

local quest_218 = fsm:new({
    id = 218,
    name = "[Revenge] Stone Cold",
    level_required = 45,
    npc_start = 0,
    npc_complete = 0,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 218: Awaiting acceptance")
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
                self:log("Quest 218: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 38,
                    count = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 218: Kill objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 218: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 0,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 218: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 218: Rewards given")
                self:give_rewards({
                    exp = 100000,
                    gold = 13000,
                })
            end,
            transitions = {},
        },
    },
})

return quest_218