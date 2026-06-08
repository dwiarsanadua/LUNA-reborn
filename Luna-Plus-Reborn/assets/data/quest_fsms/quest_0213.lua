--[[
  Quest 213: [Revenge] Green Eyed Monster
  Level Required: 31
  NPC Start: 0, NPC Complete: 0
  Prerequisites: [212]
  Rewards: EXP=163304, Gold=4000
]]

local fsm = require('fsm_engine')

local quest_213 = fsm:new({
    id = 213,
    name = "[Revenge] Green Eyed Monster",
    level_required = 31,
    npc_start = 0,
    npc_complete = 0,
    prerequisites = {212},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 213: Awaiting acceptance")
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
                self:log("Quest 213: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 10,
                    count = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 213: Kill objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 213: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 0,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 213: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 213: Rewards given")
                self:give_rewards({
                    exp = 163304,
                    gold = 4000,
                    items = {
                        { item_id = 21000095, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_213