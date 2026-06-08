--[[
  Quest 905: Adrian's Errand
  Level Required: 81
  NPC Start: 90, NPC Complete: 90
  Rewards: EXP=2862942
]]

local fsm = require('fsm_engine')

local quest_905 = fsm:new({
    id = 905,
    name = "Adrian's Errand",
    level_required = 81,
    npc_start = 90,
    npc_complete = 90,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 905: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 90,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 905: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 905: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 74,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 905: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 90,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 905: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 905: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 90,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 905: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 905: Rewards given")
                self:give_rewards({
                    exp = 2862942,
                    items = {
                        { item_id = 394, count = 1 },
                        { item_id = 394, count = 30 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_905