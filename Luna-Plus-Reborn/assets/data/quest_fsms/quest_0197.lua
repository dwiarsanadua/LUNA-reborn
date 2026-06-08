--[[
  Quest 197: New Trade Goods
  Level Required: 44
  NPC Start: 62, NPC Complete: 62
  Prerequisites: [196]
  Rewards: EXP=144000, Gold=4500
]]

local fsm = require('fsm_engine')

local quest_197 = fsm:new({
    id = 197,
    name = "New Trade Goods",
    level_required = 44,
    npc_start = 62,
    npc_complete = 62,
    prerequisites = {196},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 197: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 62,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 197: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 197: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 77,
                    count = 4,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 197: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 17,
                    count = 4,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 197: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 62,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 197: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 197: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 62,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 197: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 197: Rewards given")
                self:give_rewards({
                    exp = 144000,
                    gold = 4500,
                    items = {
                        { item_id = 131, count = 10 },
                        { item_id = 132, count = 10 },
                        { item_id = 133, count = 10 },
                        { item_id = 134, count = 10 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_197