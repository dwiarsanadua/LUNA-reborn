--[[
  Quest 209: [Revenge] Zakandia Insurance
  Level Required: 17
  NPC Start: 0, NPC Complete: 0
  Prerequisites: [208]
  Rewards: EXP=11097, Gold=2000
]]

local fsm = require('fsm_engine')

local quest_209 = fsm:new({
    id = 209,
    name = "[Revenge] Zakandia Insurance",
    level_required = 17,
    npc_start = 0,
    npc_complete = 0,
    prerequisites = {208},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 209: Awaiting acceptance")
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
                self:log("Quest 209: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 4,
                    count = 20,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 209: Kill objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 209: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 0,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 209: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 209: Rewards given")
                self:give_rewards({
                    exp = 11097,
                    gold = 2000,
                })
            end,
            transitions = {},
        },
    },
})

return quest_209