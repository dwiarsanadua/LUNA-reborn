--[[
  Quest 119: Magical Creatures
  Level Required: 24
  NPC Start: 24, NPC Complete: 24
  Rewards: EXP=16538, Gold=2850
]]

local fsm = require('fsm_engine')

local quest_119 = fsm:new({
    id = 119,
    name = "Magical Creatures",
    level_required = 24,
    npc_start = 24,
    npc_complete = 24,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 119: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 24,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 119: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 119: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 12,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 119: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 52,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 119: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 24,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 119: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 119: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 24,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 119: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 119: Rewards given")
                self:give_rewards({
                    exp = 16538,
                    gold = 2850,
                    items = {
                        { item_id = 56, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_119