--[[
  Quest 937: Checking in with Sue
  Level Required: 103
  NPC Start: 541, NPC Complete: 542
  Rewards: EXP=13661759, Gold=81769
]]

local fsm = require('fsm_engine')

local quest_937 = fsm:new({
    id = 937,
    name = "Checking in with Sue",
    level_required = 103,
    npc_start = 541,
    npc_complete = 542,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 937: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 541,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 937: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 937: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 332,
                    count = 100,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 937: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 541,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 937: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 542,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 937: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 937: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 542,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 937: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 937: Rewards given")
                self:give_rewards({
                    exp = 13661759,
                    gold = 81769,
                })
            end,
            transitions = {},
        },
    },
})

return quest_937