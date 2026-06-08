--[[
  Quest 234: [Revenge] U Trollin'
  Level Required: 58
  NPC Start: 0, NPC Complete: 0
  Prerequisites: [233]
  Rewards: EXP=500000, Gold=45000
]]

local fsm = require('fsm_engine')

local quest_234 = fsm:new({
    id = 234,
    name = "[Revenge] U Trollin'",
    level_required = 58,
    npc_start = 0,
    npc_complete = 0,
    prerequisites = {233},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 234: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "accept",
                    target = "IN_PROGRESS",
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 234: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 36,
                    count = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 234: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 102,
                    count = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 234: Kill objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 234: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 0,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 234: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 234: Rewards given")
                self:give_rewards({
                    exp = 500000,
                    gold = 45000,
                })
            end,
            transitions = {},
        },
    },
})

return quest_234