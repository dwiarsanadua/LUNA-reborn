--[[
  Quest 852: Owner of the Wreck
  Level Required: 88
  NPC Start: 502, NPC Complete: 502
  Rewards: EXP=3243016
]]

local fsm = require('fsm_engine')

local quest_852 = fsm:new({
    id = 852,
    name = "Owner of the Wreck",
    level_required = 88,
    npc_start = 502,
    npc_complete = 502,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 852: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 502,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 852: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 852: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 755,
                    count = 4,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 852: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 756,
                    count = 4,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 852: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 757,
                    count = 4,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 852: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 758,
                    count = 4,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 852: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 502,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 852: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 852: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 502,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 852: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 852: Rewards given")
                self:give_rewards({
                    exp = 3243016,
                })
            end,
            transitions = {},
        },
    },
})

return quest_852