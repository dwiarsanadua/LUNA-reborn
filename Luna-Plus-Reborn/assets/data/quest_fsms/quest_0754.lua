--[[
  Quest 754: [Quest]The Joy of Cooking
  Level Required: 20
  NPC Start: 64, NPC Complete: 510
]]

local fsm = require('fsm_engine')

local quest_754 = fsm:new({
    id = 754,
    name = "[Quest]The Joy of Cooking",
    level_required = 20,
    npc_start = 64,
    npc_complete = 510,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 754: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 64,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 754: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 754: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 64,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 754: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 510,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 754: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 754: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 510,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 754: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 754: Rewards given")
                self:give_rewards({
                    items = {
                        { item_id = 30000608, count = 1 },
                        { item_id = 30000609, count = 2 },
                        { item_id = 21000622, count = 2 },
                        { item_id = 21000306, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_754