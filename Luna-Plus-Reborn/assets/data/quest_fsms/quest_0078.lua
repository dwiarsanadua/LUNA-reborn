--[[
  Quest 78: Food Shortage
  Level Required: 9
  NPC Start: 3, NPC Complete: 14
  Prerequisites: [10]
  Rewards: EXP=355, Gold=1325
]]

local fsm = require('fsm_engine')

local quest_78 = fsm:new({
    id = 78,
    name = "Food Shortage",
    level_required = 9,
    npc_start = 3,
    npc_complete = 14,
    prerequisites = {10},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 78: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 14,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 78: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 78: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 14,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 78: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 3,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 78: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 78: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 14,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 78: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 78: Rewards given")
                self:give_rewards({
                    exp = 355,
                    gold = 1325,
                    items = {
                        { item_id = 28, count = 1 },
                        { item_id = 21000006, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_78