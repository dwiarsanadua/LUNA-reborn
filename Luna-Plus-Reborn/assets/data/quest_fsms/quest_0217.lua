--[[
  Quest 217: [Revenge] Winged Justice
  Level Required: 41
  NPC Start: 0, NPC Complete: 0
  Prerequisites: [216]
  Rewards: EXP=65000, Gold=9000
]]

local fsm = require('fsm_engine')

local quest_217 = fsm:new({
    id = 217,
    name = "[Revenge] Winged Justice",
    level_required = 41,
    npc_start = 0,
    npc_complete = 0,
    prerequisites = {216},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 217: Awaiting acceptance")
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
                self:log("Quest 217: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 60,
                    count = 40,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 217: Kill objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 217: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 0,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 217: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 217: Rewards given")
                self:give_rewards({
                    exp = 65000,
                    gold = 9000,
                })
            end,
            transitions = {},
        },
    },
})

return quest_217