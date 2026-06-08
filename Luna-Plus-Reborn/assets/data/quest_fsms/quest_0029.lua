--[[
  Quest 29: Harpy Egg Hunt
  Level Required: 35
  NPC Start: 30, NPC Complete: 30
  Rewards: EXP=85578, Gold=4970
]]

local fsm = require('fsm_engine')

local quest_29 = fsm:new({
    id = 29,
    name = "Harpy Egg Hunt",
    level_required = 35,
    npc_start = 30,
    npc_complete = 30,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 29: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 30,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 29: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 29: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 51,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 29: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 56,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 29: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 29: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 29: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 30,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 29: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 29: Rewards given")
                self:give_rewards({
                    exp = 85578,
                    gold = 4970,
                    items = {
                        { item_id = 11, count = 15 },
                        { item_id = 30000824, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_29