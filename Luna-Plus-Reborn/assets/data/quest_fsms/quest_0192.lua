--[[
  Quest 192: [Monk Job Change] Magic and Might
  Level Required: 40
  NPC Start: 7, NPC Complete: 105
  Rewards: EXP=192000, Gold=40000
]]

local fsm = require('fsm_engine')

local quest_192 = fsm:new({
    id = 192,
    name = "[Monk Job Change] Magic and Might",
    level_required = 40,
    npc_start = 7,
    npc_complete = 105,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 192: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 7,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 192: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 192: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 44,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 192: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 55,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 192: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 43,
                    count = 3,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 192: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 7,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 192: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 10,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 192: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 34,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 192: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 36,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 192: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 105,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 192: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 192: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 105,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 192: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 192: Rewards given")
                self:give_rewards({
                    exp = 192000,
                    gold = 40000,
                    items = {
                        { item_id = 11007260, count = 1 },
                        { item_id = 115, count = 5 },
                        { item_id = 11007259, count = 1 },
                        { item_id = 116, count = 1 },
                        { item_id = 117, count = 1 },
                        { item_id = 118, count = 1 },
                        { item_id = 119, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_192