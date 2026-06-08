--[[
  Quest 448: [Job Change] Finding a Clue
  Level Required: 105
  NPC Start: 122, NPC Complete: 122
  Prerequisites: [447]
  Rewards: EXP=18785340, Gold=1000000
]]

local fsm = require('fsm_engine')

local quest_448 = fsm:new({
    id = 448,
    name = "[Job Change] Finding a Clue",
    level_required = 105,
    npc_start = 122,
    npc_complete = 122,
    prerequisites = {447},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 448: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 122,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 448: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 448: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 290,
                    count = 50,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 448: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 291,
                    count = 50,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 448: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 292,
                    count = 50,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 448: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 293,
                    count = 50,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 448: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 290,
                    count = 5,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 448: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 291,
                    count = 5,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 448: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 292,
                    count = 5,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 448: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 293,
                    count = 5,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 448: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 122,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 448: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 448: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 122,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 448: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 448: Rewards given")
                self:give_rewards({
                    exp = 18785340,
                    gold = 1000000,
                    items = {
                        { item_id = 324, count = 1 },
                        { item_id = 324, count = 1 },
                        { item_id = 324, count = 1 },
                        { item_id = 324, count = 1 },
                        { item_id = 324, count = 1 },
                        { item_id = 30000539, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_448