--[[
  Quest 447: [Job Change] New Power
  Level Required: 105
  NPC Start: 98, NPC Complete: 125
  Rewards: EXP=7521990, Gold=500000
]]

local fsm = require('fsm_engine')

local quest_447 = fsm:new({
    id = 447,
    name = "[Job Change] New Power",
    level_required = 105,
    npc_start = 98,
    npc_complete = 125,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 447: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 122,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 447: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 447: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 278,
                    count = 4,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 447: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 279,
                    count = 4,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 447: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 281,
                    count = 4,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 447: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 284,
                    count = 4,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 447: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 122,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 447: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 125,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 447: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 98,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 447: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 447: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 125,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 447: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 447: Rewards given")
                self:give_rewards({
                    exp = 7521990,
                    gold = 500000,
                    items = {
                        { item_id = 320, count = 1 },
                        { item_id = 321, count = 1 },
                        { item_id = 322, count = 1 },
                        { item_id = 323, count = 1 },
                        { item_id = 321, count = 1 },
                        { item_id = 322, count = 1 },
                        { item_id = 323, count = 1 },
                        { item_id = 320, count = 1 },
                        { item_id = 30000303, count = 3 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_447