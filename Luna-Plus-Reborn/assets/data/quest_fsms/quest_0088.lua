--[[
  Quest 88: The Missing Geologist
  Level Required: 12
  NPC Start: 53, NPC Complete: 54
  Prerequisites: [87]
  Rewards: EXP=964, Gold=825
]]

local fsm = require('fsm_engine')

local quest_88 = fsm:new({
    id = 88,
    name = "The Missing Geologist",
    level_required = 12,
    npc_start = 53,
    npc_complete = 54,
    prerequisites = {87},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 88: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 53,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 88: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 88: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 53,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 88: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 54,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 88: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 88: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 54,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 88: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 88: Rewards given")
                self:give_rewards({
                    exp = 964,
                    gold = 825,
                    items = {
                        { item_id = 36, count = 1 },
                        { item_id = 21000007, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_88