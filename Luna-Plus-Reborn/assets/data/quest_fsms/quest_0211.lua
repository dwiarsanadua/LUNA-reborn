--[[
  Quest 211: [Revenge] A Dish Served Cold
  Level Required: 23
  NPC Start: 0, NPC Complete: 0
  Prerequisites: [210]
  Rewards: EXP=33168, Gold=2700
]]

local fsm = require('fsm_engine')

local quest_211 = fsm:new({
    id = 211,
    name = "[Revenge] A Dish Served Cold",
    level_required = 23,
    npc_start = 0,
    npc_complete = 0,
    prerequisites = {210},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 211: Awaiting acceptance")
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
                self:log("Quest 211: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 24,
                    count = 20,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 211: Kill objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 211: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 0,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 211: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 211: Rewards given")
                self:give_rewards({
                    exp = 33168,
                    gold = 2700,
                    items = {
                        { item_id = 21000010, count = 3 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_211