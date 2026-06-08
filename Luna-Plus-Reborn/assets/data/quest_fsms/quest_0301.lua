--[[
  Quest 301: [C Class] Furious Orc Elimination
  Level Required: 37
  NPC Start: 79, NPC Complete: 79
  Prerequisites: [299]
  Rewards: EXP=115128, Gold=3200
]]

local fsm = require('fsm_engine')

local quest_301 = fsm:new({
    id = 301,
    name = "[C Class] Furious Orc Elimination",
    level_required = 37,
    npc_start = 79,
    npc_complete = 79,
    prerequisites = {299},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 301: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 79,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 301: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 301: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 56,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 301: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 79,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 301: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 301: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 79,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 301: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 301: Rewards given")
                self:give_rewards({
                    exp = 115128,
                    gold = 3200,
                    items = {
                        { item_id = 206, count = 50 },
                        { item_id = 30000499, count = 1 },
                        { item_id = 21000096, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_301