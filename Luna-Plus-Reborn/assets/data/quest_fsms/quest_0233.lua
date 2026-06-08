--[[
  Quest 233: [Revenge] Injury For Insult
  Level Required: 58
  NPC Start: 0, NPC Complete: 0
  Rewards: EXP=400000, Gold=40000
]]

local fsm = require('fsm_engine')

local quest_233 = fsm:new({
    id = 233,
    name = "[Revenge] Injury For Insult",
    level_required = 58,
    npc_start = 0,
    npc_complete = 0,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 233: Awaiting acceptance")
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
                self:log("Quest 233: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 109,
                    count = 50,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 233: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 76,
                    count = 50,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 233: Kill objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 233: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 0,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 233: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 233: Rewards given")
                self:give_rewards({
                    exp = 400000,
                    gold = 40000,
                })
            end,
            transitions = {},
        },
    },
})

return quest_233