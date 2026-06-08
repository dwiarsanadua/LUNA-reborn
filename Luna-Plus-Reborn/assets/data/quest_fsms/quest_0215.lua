--[[
  Quest 215: [Revenge] Hell Hath No Fury...
  Level Required: 36
  NPC Start: 0, NPC Complete: 0
  Prerequisites: [214]
  Rewards: EXP=165850, Gold=5000
]]

local fsm = require('fsm_engine')

local quest_215 = fsm:new({
    id = 215,
    name = "[Revenge] Hell Hath No Fury...",
    level_required = 36,
    npc_start = 0,
    npc_complete = 0,
    prerequisites = {214},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 215: Awaiting acceptance")
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
                self:log("Quest 215: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 32,
                    count = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 215: Kill objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 215: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 0,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 215: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 215: Rewards given")
                self:give_rewards({
                    exp = 165850,
                    gold = 5000,
                    items = {
                        { item_id = 21000097, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_215