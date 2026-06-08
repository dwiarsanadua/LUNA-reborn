--[[
  Quest 22: Agricultural Assistance
  Level Required: 25
  NPC Start: 23, NPC Complete: 26
  Rewards: EXP=9663, Gold=500
]]

local fsm = require('fsm_engine')

local quest_22 = fsm:new({
    id = 22,
    name = "Agricultural Assistance",
    level_required = 25,
    npc_start = 23,
    npc_complete = 26,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 22: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 23,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 22: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 22: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 23,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 22: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 26,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 22: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 22: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 26,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 22: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 22: Rewards given")
                self:give_rewards({
                    exp = 9663,
                    gold = 500,
                    items = {
                        { item_id = 21000008, count = 5 },
                        { item_id = 21000010, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_22