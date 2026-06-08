--[[
  Quest 150: Blacksmith's Burden
  Level Required: 13
  NPC Start: 5, NPC Complete: 5
  Rewards: EXP=3171, Gold=2000
]]

local fsm = require('fsm_engine')

local quest_150 = fsm:new({
    id = 150,
    name = "Blacksmith's Burden",
    level_required = 13,
    npc_start = 5,
    npc_complete = 5,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 150: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 5,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 150: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 150: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 19,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 150: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 5,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 150: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 150: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 5,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 150: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 150: Rewards given")
                self:give_rewards({
                    exp = 3171,
                    gold = 2000,
                    items = {
                        { item_id = 67, count = 1 },
                        { item_id = 67, count = 30 },
                        { item_id = 21000007, count = 5 },
                        { item_id = 21000282, count = 2 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_150