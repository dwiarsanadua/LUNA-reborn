--[[
  Quest 236: [Revenge] That'll Show 'Em
  Level Required: 62
  NPC Start: 0, NPC Complete: 0
  Prerequisites: [235]
  Rewards: EXP=600000, Gold=50000
]]

local fsm = require('fsm_engine')

local quest_236 = fsm:new({
    id = 236,
    name = "[Revenge] That'll Show 'Em",
    level_required = 62,
    npc_start = 0,
    npc_complete = 0,
    prerequisites = {235},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 236: Awaiting acceptance")
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
                self:log("Quest 236: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 89,
                    count = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 236: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 104,
                    count = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 236: Kill objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 236: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 0,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 236: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 236: Rewards given")
                self:give_rewards({
                    exp = 600000,
                    gold = 50000,
                })
            end,
            transitions = {},
        },
    },
})

return quest_236