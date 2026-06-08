--[[
  Quest 191: [Sorcerer Job Change] Real Ultimate Power
  Level Required: 40
  NPC Start: 7, NPC Complete: 104
  Rewards: EXP=192000, Gold=40000
]]

local fsm = require('fsm_engine')

local quest_191 = fsm:new({
    id = 191,
    name = "[Sorcerer Job Change] Real Ultimate Power",
    level_required = 40,
    npc_start = 7,
    npc_complete = 104,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 191: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 7,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 191: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 191: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 56,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 191: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 32,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 191: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 44,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 191: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 30,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 191: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 7,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 191: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 34,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 191: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 104,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 191: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 191: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 104,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 191: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 191: Rewards given")
                self:give_rewards({
                    exp = 192000,
                    gold = 40000,
                    items = {
                        { item_id = 11007260, count = 1 },
                        { item_id = 111, count = 10 },
                        { item_id = 112, count = 1 },
                        { item_id = 112, count = 1 },
                        { item_id = 30000299, count = 1 },
                        { item_id = 30000300, count = 5 },
                        { item_id = 113, count = 1 },
                        { item_id = 114, count = 1 },
                        { item_id = 11007260, count = 1 },
                        { item_id = 114, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_191