--[[
  Quest 367: [Repeatable][Party] Shadow Menace
  Level Required: 40
  NPC Start: 57, NPC Complete: 57
  Rewards: EXP=95297, Gold=8000
]]

local fsm = require('fsm_engine')

local quest_367 = fsm:new({
    id = 367,
    name = "[Repeatable][Party] Shadow Menace",
    level_required = 40,
    npc_start = 57,
    npc_complete = 57,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 367: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 57,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 367: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 367: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 159,
                    count = 60,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 367: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 57,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 367: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 367: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 57,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 367: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 367: Rewards given")
                self:give_rewards({
                    exp = 95297,
                    gold = 8000,
                })
            end,
            transitions = {},
        },
    },
})

return quest_367