--[[
  Quest 205: [Revenge] Alker Loss Prevention Insurance
  Level Required: 15
  NPC Start: 0, NPC Complete: 0
  Rewards: EXP=7025, Gold=1000
]]

local fsm = require('fsm_engine')

local quest_205 = fsm:new({
    id = 205,
    name = "[Revenge] Alker Loss Prevention Insurance",
    level_required = 15,
    npc_start = 0,
    npc_complete = 0,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 205: Awaiting acceptance")
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
                self:log("Quest 205: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 6,
                    count = 15,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 205: Kill objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 205: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 0,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 205: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 205: Rewards given")
                self:give_rewards({
                    exp = 7025,
                    gold = 1000,
                    items = {
                        { item_id = 30000824, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_205