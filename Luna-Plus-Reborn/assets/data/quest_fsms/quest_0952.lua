--[[
  Quest 952: Surprise Bite
  Level Required: 108
  NPC Start: 542, NPC Complete: 542
  Rewards: EXP=15805326, Gold=101632
]]

local fsm = require('fsm_engine')

local quest_952 = fsm:new({
    id = 952,
    name = "Surprise Bite",
    level_required = 108,
    npc_start = 542,
    npc_complete = 542,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 952: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 542,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 952: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 952: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 325,
                    count = 100,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 952: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 542,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 952: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 952: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 542,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 952: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 952: Rewards given")
                self:give_rewards({
                    exp = 15805326,
                    gold = 101632,
                })
            end,
            transitions = {},
        },
    },
})

return quest_952