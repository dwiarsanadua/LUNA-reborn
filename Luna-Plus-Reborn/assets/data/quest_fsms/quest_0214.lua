--[[
  Quest 214: [Revenge] Unleash the Fury
  Level Required: 36
  NPC Start: 0, NPC Complete: 0
  Rewards: EXP=165850, Gold=4500
]]

local fsm = require('fsm_engine')

local quest_214 = fsm:new({
    id = 214,
    name = "[Revenge] Unleash the Fury",
    level_required = 36,
    npc_start = 0,
    npc_complete = 0,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 214: Awaiting acceptance")
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
                self:log("Quest 214: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 56,
                    count = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 214: Kill objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 214: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 0,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 214: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 214: Rewards given")
                self:give_rewards({
                    exp = 165850,
                    gold = 4500,
                    items = {
                        { item_id = 21000097, count = 8 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_214