--[[
  Quest 190: [Priest Job Change] Finding Your Marbles
  Level Required: 40
  NPC Start: 7, NPC Complete: 102
  Rewards: EXP=192000, Gold=40000
]]

local fsm = require('fsm_engine')

local quest_190 = fsm:new({
    id = 190,
    name = "[Priest Job Change] Finding Your Marbles",
    level_required = 40,
    npc_start = 7,
    npc_complete = 102,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 190: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 7,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 190: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 190: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 54,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 190: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 10,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 190: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 26,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 190: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 55,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 190: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 51,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 190: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 56,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 190: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 57,
                    count = 3,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 190: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 58,
                    count = 3,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 190: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 32,
                    count = 3,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 190: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 7,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 190: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 24,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 190: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 32,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 190: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 102,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 190: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 9,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 190: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 190: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 102,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 190: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 190: Rewards given")
                self:give_rewards({
                    exp = 192000,
                    gold = 40000,
                    items = {
                        { item_id = 30000290, count = 1 },
                        { item_id = 30000292, count = 1 },
                        { item_id = 30000294, count = 1 },
                        { item_id = 30000291, count = 1 },
                        { item_id = 30000293, count = 1 },
                        { item_id = 30000295, count = 1 },
                        { item_id = 108, count = 1 },
                        { item_id = 109, count = 1 },
                        { item_id = 110, count = 1 },
                        { item_id = 107, count = 1 },
                        { item_id = 154, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_190