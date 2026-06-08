--[[
  Quest 780: Tall Girl?
  Level Required: 34
  NPC Start: 28, NPC Complete: 28
  Prerequisites: [779]
  Rewards: EXP=49916, Gold=5803
]]

local fsm = require('fsm_engine')

local quest_780 = fsm:new({
    id = 780,
    name = "Tall Girl?",
    level_required = 34,
    npc_start = 28,
    npc_complete = 28,
    prerequisites = {779},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 780: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 28,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 780: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 780: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 26,
                    count = 20,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 780: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 28,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 780: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 780: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 28,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 780: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 780: Rewards given")
                self:give_rewards({
                    exp = 49916,
                    gold = 5803,
                })
            end,
            transitions = {},
        },
    },
})

return quest_780