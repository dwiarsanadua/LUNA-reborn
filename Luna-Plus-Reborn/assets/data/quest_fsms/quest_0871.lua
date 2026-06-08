--[[
  Quest 871: Key to my Storage?!
  Level Required: 92
  NPC Start: 91, NPC Complete: 91
  Rewards: EXP=5405129, Gold=61188
]]

local fsm = require('fsm_engine')

local quest_871 = fsm:new({
    id = 871,
    name = "Key to my Storage?!",
    level_required = 92,
    npc_start = 91,
    npc_complete = 91,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 871: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 91,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 871: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 871: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 289,
                    count = 100,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 871: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 91,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 871: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 871: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 91,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 871: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 871: Rewards given")
                self:give_rewards({
                    exp = 5405129,
                    gold = 61188,
                })
            end,
            transitions = {},
        },
    },
})

return quest_871