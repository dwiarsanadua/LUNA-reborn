--[[
  Quest 219: [Revenge] Giant Problems
  Level Required: 45
  NPC Start: 0, NPC Complete: 0
  Prerequisites: [218]
  Rewards: EXP=120000, Gold=15000
]]

local fsm = require('fsm_engine')

local quest_219 = fsm:new({
    id = 219,
    name = "[Revenge] Giant Problems",
    level_required = 45,
    npc_start = 0,
    npc_complete = 0,
    prerequisites = {218},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 219: Awaiting acceptance")
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
                self:log("Quest 219: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 39,
                    count = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 219: Kill objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 219: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 0,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 219: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 219: Rewards given")
                self:give_rewards({
                    exp = 120000,
                    gold = 15000,
                })
            end,
            transitions = {},
        },
    },
})

return quest_219