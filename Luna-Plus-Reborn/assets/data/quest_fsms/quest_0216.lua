--[[
  Quest 216: [Revenge] Let Slip the Dogs of War
  Level Required: 41
  NPC Start: 0, NPC Complete: 0
  Rewards: EXP=50000, Gold=7000
]]

local fsm = require('fsm_engine')

local quest_216 = fsm:new({
    id = 216,
    name = "[Revenge] Let Slip the Dogs of War",
    level_required = 41,
    npc_start = 0,
    npc_complete = 0,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 216: Awaiting acceptance")
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
                self:log("Quest 216: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 43,
                    count = 40,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 216: Kill objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 216: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 0,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 216: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 216: Rewards given")
                self:give_rewards({
                    exp = 50000,
                    gold = 7000,
                })
            end,
            transitions = {},
        },
    },
})

return quest_216