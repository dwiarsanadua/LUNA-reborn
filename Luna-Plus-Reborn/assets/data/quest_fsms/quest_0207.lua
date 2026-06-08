--[[
  Quest 207: [Revenge] Patriarch Knows Best
  Level Required: 15
  NPC Start: 0, NPC Complete: 0
  Prerequisites: [206]
  Rewards: EXP=7025, Gold=1800
]]

local fsm = require('fsm_engine')

local quest_207 = fsm:new({
    id = 207,
    name = "[Revenge] Patriarch Knows Best",
    level_required = 15,
    npc_start = 0,
    npc_complete = 0,
    prerequisites = {206},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 207: Awaiting acceptance")
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
                self:log("Quest 207: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 42,
                    count = 10,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 207: Kill objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 207: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 0,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 207: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 207: Rewards given")
                self:give_rewards({
                    exp = 7025,
                    gold = 1800,
                    items = {
                        { item_id = 30000826, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_207