--[[
  Quest 187: [Infantryman Job Change] Sworn to Protect
  Level Required: 40
  NPC Start: 10, NPC Complete: 100
  Rewards: EXP=192000, Gold=40000
]]

local fsm = require('fsm_engine')

local quest_187 = fsm:new({
    id = 187,
    name = "[Infantryman Job Change] Sworn to Protect",
    level_required = 40,
    npc_start = 10,
    npc_complete = 100,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 187: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 10,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 187: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 187: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 32,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 187: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 55,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 187: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 43,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 187: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 10,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 187: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 31,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 187: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 100,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 187: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 29,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 187: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 36,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 187: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 187: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 100,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 187: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 187: Rewards given")
                self:give_rewards({
                    exp = 192000,
                    gold = 40000,
                    items = {
                        { item_id = 11007255, count = 1 },
                        { item_id = 95, count = 1 },
                        { item_id = 95, count = 1 },
                        { item_id = 96, count = 1 },
                        { item_id = 97, count = 1 },
                        { item_id = 98, count = 1 },
                        { item_id = 11007255, count = 1 },
                        { item_id = 98, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_187