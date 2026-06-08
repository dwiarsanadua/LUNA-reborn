--[[
  Quest 449: [Job Change] A Sword
  Level Required: 105
  NPC Start: 125, NPC Complete: 125
  Prerequisites: [448]
  Rewards: EXP=28495320, Gold=1300000
]]

local fsm = require('fsm_engine')

local quest_449 = fsm:new({
    id = 449,
    name = "[Job Change] A Sword",
    level_required = 105,
    npc_start = 125,
    npc_complete = 125,
    prerequisites = {448},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 449: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 125,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 449: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 449: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 294,
                    count = 100,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 449: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 295,
                    count = 100,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 449: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 296,
                    count = 50,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 449: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 294,
                    count = 3,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 449: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 295,
                    count = 3,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 449: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 296,
                    count = 3,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 449: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 125,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 449: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 449: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 125,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 449: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 449: Rewards given")
                self:give_rewards({
                    exp = 28495320,
                    gold = 1300000,
                    items = {
                        { item_id = 325, count = 1 },
                        { item_id = 30000540, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_449