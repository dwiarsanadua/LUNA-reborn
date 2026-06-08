--[[
  Quest 251: Collecting Red Ore
  Level Required: 150
  NPC Start: 55, NPC Complete: 55
  Prerequisites: [250]
  Rewards: EXP=77678, Gold=2000
]]

local fsm = require('fsm_engine')

local quest_251 = fsm:new({
    id = 251,
    name = "Collecting Red Ore",
    level_required = 150,
    npc_start = 55,
    npc_complete = 55,
    prerequisites = {250},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 251: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 55,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 251: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 251: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 10,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 251: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 127,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 251: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 55,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 251: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 251: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 55,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 251: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 251: Rewards given")
                self:give_rewards({
                    exp = 77678,
                    gold = 2000,
                    items = {
                        { item_id = 163, count = 1 },
                        { item_id = 30000055, count = 30 },
                        { item_id = 30000053, count = 30 },
                        { item_id = 30000084, count = 30 },
                        { item_id = 163, count = 1 },
                        { item_id = 11007297, count = 1 },
                        { item_id = 164, count = 10 },
                        { item_id = 21000164, count = 1 },
                        { item_id = 21000010, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_251