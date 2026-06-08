--[[
  Quest 28: Orcs' Rebellion
  Level Required: 43
  NPC Start: 29, NPC Complete: 32
  Rewards: EXP=696, Gold=530
]]

local fsm = require('fsm_engine')

local quest_28 = fsm:new({
    id = 28,
    name = "Orcs' Rebellion",
    level_required = 43,
    npc_start = 29,
    npc_complete = 32,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 28: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 29,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 28: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 28: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 29,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 28: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 32,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 28: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 28: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 32,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 28: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 28: Rewards given")
                self:give_rewards({
                    exp = 696,
                    gold = 530,
                    items = {
                        { item_id = 21000097, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_28