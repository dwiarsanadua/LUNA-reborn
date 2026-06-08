--[[
  Quest 163: A Blacksmith's Burden Never Ends
  Level Required: 37
  NPC Start: 5, NPC Complete: 7
  Rewards: EXP=105918, Gold=3190
]]

local fsm = require('fsm_engine')

local quest_163 = fsm:new({
    id = 163,
    name = "A Blacksmith's Burden Never Ends",
    level_required = 37,
    npc_start = 5,
    npc_complete = 7,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 163: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 5,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 163: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 163: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 58,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 163: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 32,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 163: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 5,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 163: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 7,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 163: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 163: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 7,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 163: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 163: Rewards given")
                self:give_rewards({
                    exp = 105918,
                    gold = 3190,
                    items = {
                        { item_id = 77, count = 15 },
                        { item_id = 78, count = 1 },
                        { item_id = 79, count = 1 },
                        { item_id = 21000095, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_163