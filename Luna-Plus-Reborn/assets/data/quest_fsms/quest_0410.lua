--[[
  Quest 410: [repeatable][quest] A study of a mutation
  Level Required: 76
  NPC Start: 92, NPC Complete: 92
  Prerequisites: [409]
  Rewards: EXP=1912436
]]

local fsm = require('fsm_engine')

local quest_410 = fsm:new({
    id = 410,
    name = "[repeatable][quest] A study of a mutation",
    level_required = 76,
    npc_start = 92,
    npc_complete = 92,
    prerequisites = {409},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 410: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 92,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 410: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 410: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 260,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 410: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 92,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 410: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 410: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 92,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 410: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 410: Rewards given")
                self:give_rewards({
                    exp = 1912436,
                    items = {
                        { item_id = 302, count = 20 },
                        { item_id = 30000511, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_410