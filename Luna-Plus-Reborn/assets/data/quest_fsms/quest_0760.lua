--[[
  Quest 760: [Quest] Third Training Exercised
  Level Required: 0
  NPC Start: 402, NPC Complete: 409
  Prerequisites: [759]
  Rewards: EXP=100000, Gold=20000
]]

local fsm = require('fsm_engine')

local quest_760 = fsm:new({
    id = 760,
    name = "[Quest] Third Training Exercised",
    level_required = 0,
    npc_start = 402,
    npc_complete = 409,
    prerequisites = {759},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 760: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 402,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 760: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 760: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 32,
                    count = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 760: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 402,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 760: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 409,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 760: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 760: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 409,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 760: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 760: Rewards given")
                self:give_rewards({
                    exp = 100000,
                    gold = 20000,
                    items = {
                        { item_id = 13000040, count = 1 },
                        { item_id = 12000288, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_760