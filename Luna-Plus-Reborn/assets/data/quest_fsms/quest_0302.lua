--[[
  Quest 302: [C Class] Furious Orc Elimination
  Level Required: 37
  NPC Start: 79, NPC Complete: 79
  Prerequisites: [301]
  Rewards: EXP=115128, Gold=3200
]]

local fsm = require('fsm_engine')

local quest_302 = fsm:new({
    id = 302,
    name = "[C Class] Furious Orc Elimination",
    level_required = 37,
    npc_start = 79,
    npc_complete = 79,
    prerequisites = {301},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 302: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 79,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 302: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 302: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 56,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 302: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 79,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 302: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 302: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 79,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 302: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 302: Rewards given")
                self:give_rewards({
                    exp = 115128,
                    gold = 3200,
                    items = {
                        { item_id = 207, count = 50 },
                        { item_id = 21000096, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_302