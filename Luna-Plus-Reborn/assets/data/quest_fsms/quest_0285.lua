--[[
  Quest 285: [Emblem of Influence] Pure Profit
  Level Required: 70
  NPC Start: 6, NPC Complete: 62
  Rewards: EXP=1050000, Gold=40000
]]

local fsm = require('fsm_engine')

local quest_285 = fsm:new({
    id = 285,
    name = "[Emblem of Influence] Pure Profit",
    level_required = 70,
    npc_start = 6,
    npc_complete = 62,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 285: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 6,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 285: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 285: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 40,
                    count = 4,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 285: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 88,
                    count = 4,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 285: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 6,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 285: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 62,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 285: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 285: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 62,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 285: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 285: Rewards given")
                self:give_rewards({
                    exp = 1050000,
                    gold = 40000,
                    items = {
                        { item_id = 199, count = 1 },
                        { item_id = 199, count = 1 },
                        { item_id = 200, count = 1 },
                        { item_id = 200, count = 1 },
                        { item_id = 201, count = 1 },
                        { item_id = 201, count = 1 },
                        { item_id = 202, count = 1 },
                        { item_id = 202, count = 1 },
                        { item_id = 199, count = 5 },
                        { item_id = 200, count = 5 },
                        { item_id = 201, count = 15 },
                        { item_id = 202, count = 3 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_285