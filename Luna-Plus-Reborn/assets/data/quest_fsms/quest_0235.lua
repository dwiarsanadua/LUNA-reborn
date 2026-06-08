--[[
  Quest 235: [Revenge] Take That (Dude)!
  Level Required: 62
  NPC Start: 0, NPC Complete: 0
  Rewards: EXP=600000, Gold=50000
]]

local fsm = require('fsm_engine')

local quest_235 = fsm:new({
    id = 235,
    name = "[Revenge] Take That (Dude)!",
    level_required = 62,
    npc_start = 0,
    npc_complete = 0,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 235: Awaiting acceptance")
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
                self:log("Quest 235: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 103,
                    count = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 235: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 108,
                    count = 40,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 235: Kill objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 235: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 0,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 235: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 235: Rewards given")
                self:give_rewards({
                    exp = 600000,
                    gold = 50000,
                })
            end,
            transitions = {},
        },
    },
})

return quest_235