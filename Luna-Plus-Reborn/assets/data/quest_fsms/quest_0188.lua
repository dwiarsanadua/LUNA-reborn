--[[
  Quest 188: [Mercenary Job Change] Merciless
  Level Required: 40
  NPC Start: 10, NPC Complete: 101
  Rewards: EXP=192000, Gold=40000
]]

local fsm = require('fsm_engine')

local quest_188 = fsm:new({
    id = 188,
    name = "[Mercenary Job Change] Merciless",
    level_required = 40,
    npc_start = 10,
    npc_complete = 101,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 188: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 10,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 188: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 188: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 26,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 188: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 58,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 188: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 60,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 188: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 10,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 188: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 55,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 188: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 29,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 188: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 37,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 188: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 101,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 188: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 188: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 101,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 188: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 188: Rewards given")
                self:give_rewards({
                    exp = 192000,
                    gold = 40000,
                    items = {
                        { item_id = 11007256, count = 1 },
                        { item_id = 99, count = 1 },
                        { item_id = 100, count = 1 },
                        { item_id = 101, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_188