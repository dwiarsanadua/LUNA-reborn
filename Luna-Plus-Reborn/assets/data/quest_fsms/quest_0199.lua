--[[
  Quest 199: Giant Threat
  Level Required: 45
  NPC Start: 10, NPC Complete: 10
  Rewards: EXP=149026, Gold=10000
]]

local fsm = require('fsm_engine')

local quest_199 = fsm:new({
    id = 199,
    name = "Giant Threat",
    level_required = 45,
    npc_start = 10,
    npc_complete = 10,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 199: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 10,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 199: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 199: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 39,
                    count = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 199: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 96,
                    count = 20,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 199: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 10,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 199: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 199: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 10,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 199: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 199: Rewards given")
                self:give_rewards({
                    exp = 149026,
                    gold = 10000,
                })
            end,
            transitions = {},
        },
    },
})

return quest_199