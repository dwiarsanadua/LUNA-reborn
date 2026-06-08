--[[
  Quest 11: The Missing Brother
  Level Required: 14
  NPC Start: 15, NPC Complete: 19
  Rewards: EXP=1957, Gold=500
]]

local fsm = require('fsm_engine')

local quest_11 = fsm:new({
    id = 11,
    name = "The Missing Brother",
    level_required = 14,
    npc_start = 15,
    npc_complete = 19,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 11: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 15,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 11: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 11: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 15,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 11: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 19,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 11: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 11: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 19,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 11: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 11: Rewards given")
                self:give_rewards({
                    exp = 1957,
                    gold = 500,
                    items = {
                        { item_id = 5, count = 1 },
                        { item_id = 21000007, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_11