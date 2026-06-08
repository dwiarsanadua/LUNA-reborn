--[[
  Quest 366: [Repeatable][Party] Insufficient Materials
  Level Required: 33
  NPC Start: 21, NPC Complete: 21
  Prerequisites: [365]
  Rewards: EXP=62260, Gold=6000
]]

local fsm = require('fsm_engine')

local quest_366 = fsm:new({
    id = 366,
    name = "[Repeatable][Party] Insufficient Materials",
    level_required = 33,
    npc_start = 21,
    npc_complete = 21,
    prerequisites = {365},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 366: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 21,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 366: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 366: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 155,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 366: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 21,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 366: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 366: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 21,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 366: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 366: Rewards given")
                self:give_rewards({
                    exp = 62260,
                    gold = 6000,
                    items = {
                        { item_id = 292, count = 1 },
                        { item_id = 292, count = 20 },
                        { item_id = 21000095, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_366