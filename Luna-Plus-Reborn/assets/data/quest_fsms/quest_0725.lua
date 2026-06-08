--[[
  Quest 725: Hunters Union Elimination Order
  Level Required: 73
  NPC Start: 316, NPC Complete: 316
  Prerequisites: [724]
  Rewards: EXP=1951286
]]

local fsm = require('fsm_engine')

local quest_725 = fsm:new({
    id = 725,
    name = "Hunters Union Elimination Order",
    level_required = 73,
    npc_start = 316,
    npc_complete = 316,
    prerequisites = {724},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 725: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 316,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 725: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 725: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 252,
                    count = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 725: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 259,
                    count = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 725: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 251,
                    count = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 725: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 316,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 725: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 725: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 316,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 725: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 725: Rewards given")
                self:give_rewards({
                    exp = 1951286,
                })
            end,
            transitions = {},
        },
    },
})

return quest_725