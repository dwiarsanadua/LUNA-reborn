--[[
  Quest 232: [Revenge] Smells Like Vengeance
  Level Required: 55
  NPC Start: 0, NPC Complete: 0
  Prerequisites: [231]
  Rewards: EXP=250000, Gold=30000
]]

local fsm = require('fsm_engine')

local quest_232 = fsm:new({
    id = 232,
    name = "[Revenge] Smells Like Vengeance",
    level_required = 55,
    npc_start = 0,
    npc_complete = 0,
    prerequisites = {231},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 232: Awaiting acceptance")
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
                self:log("Quest 232: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 40,
                    count = 50,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 232: Kill objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 232: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 0,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 232: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 232: Rewards given")
                self:give_rewards({
                    exp = 250000,
                    gold = 30000,
                })
            end,
            transitions = {},
        },
    },
})

return quest_232