--[[
  Quest 874: Thorny One
  Level Required: 93
  NPC Start: 91, NPC Complete: 91
  Rewards: EXP=5911530, Gold=61188
]]

local fsm = require('fsm_engine')

local quest_874 = fsm:new({
    id = 874,
    name = "Thorny One",
    level_required = 93,
    npc_start = 91,
    npc_complete = 91,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 874: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 91,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 874: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 874: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 288,
                    count = 50,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 874: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 289,
                    count = 50,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 874: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 91,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 874: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 874: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 91,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 874: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 874: Rewards given")
                self:give_rewards({
                    exp = 5911530,
                    gold = 61188,
                })
            end,
            transitions = {},
        },
    },
})

return quest_874