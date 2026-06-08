--[[
  Quest 875: Truth of the Crown
  Level Required: 93
  NPC Start: 92, NPC Complete: 92
  Rewards: EXP=7778329
]]

local fsm = require('fsm_engine')

local quest_875 = fsm:new({
    id = 875,
    name = "Truth of the Crown",
    level_required = 93,
    npc_start = 92,
    npc_complete = 92,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 875: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 92,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 875: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 875: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 289,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 875: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 92,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 875: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 875: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 92,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 875: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 875: Rewards given")
                self:give_rewards({
                    exp = 7778329,
                    items = {
                        { item_id = 411, count = 1 },
                        { item_id = 411, count = 50 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_875