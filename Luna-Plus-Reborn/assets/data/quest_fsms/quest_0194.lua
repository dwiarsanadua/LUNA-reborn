--[[
  Quest 194: [Thief Job Change] Silent But Deadly
  Level Required: 40
  NPC Start: 5, NPC Complete: 107
  Rewards: EXP=192000, Gold=40000
]]

local fsm = require('fsm_engine')

local quest_194 = fsm:new({
    id = 194,
    name = "[Thief Job Change] Silent But Deadly",
    level_required = 40,
    npc_start = 5,
    npc_complete = 107,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 194: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 11,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 194: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 194: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 57,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 194: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 44,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 194: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 43,
                    count = 2,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 194: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 60,
                    count = 2,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 194: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 11,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 194: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 5,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 194: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 33,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 194: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 107,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 194: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 36,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 194: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 194: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 107,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 194: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 194: Rewards given")
                self:give_rewards({
                    exp = 192000,
                    gold = 40000,
                    items = {
                        { item_id = 11007257, count = 1 },
                        { item_id = 124, count = 1 },
                        { item_id = 125, count = 1 },
                        { item_id = 125, count = 1 },
                        { item_id = 126, count = 1 },
                        { item_id = 126, count = 1 },
                        { item_id = 127, count = 5 },
                        { item_id = 11007257, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_194