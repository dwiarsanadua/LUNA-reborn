--[[
  Quest 193: [Archer Job Change] Find Your Mark
  Level Required: 40
  NPC Start: 11, NPC Complete: 106
  Rewards: EXP=192000, Gold=40000
]]

local fsm = require('fsm_engine')

local quest_193 = fsm:new({
    id = 193,
    name = "[Archer Job Change] Find Your Mark",
    level_required = 40,
    npc_start = 11,
    npc_complete = 106,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 193: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 11,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 193: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 193: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 51,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 193: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 58,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 193: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 30,
                    count = 2,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 193: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 60,
                    count = 2,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 193: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 11,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 193: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 106,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 193: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 33,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 193: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 193: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 106,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 193: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 193: Rewards given")
                self:give_rewards({
                    exp = 192000,
                    gold = 40000,
                    items = {
                        { item_id = 11007258, count = 1 },
                        { item_id = 121, count = 1 },
                        { item_id = 122, count = 1 },
                        { item_id = 123, count = 1 },
                        { item_id = 130, count = 1 },
                        { item_id = 120, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_193