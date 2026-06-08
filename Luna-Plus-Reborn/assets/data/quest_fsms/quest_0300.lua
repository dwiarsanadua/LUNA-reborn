--[[
  Quest 300: [C Class] Orc Archer Elimination
  Level Required: 36
  NPC Start: 79, NPC Complete: 79
  Prerequisites: [299]
  Rewards: EXP=103656, Gold=3000
]]

local fsm = require('fsm_engine')

local quest_300 = fsm:new({
    id = 300,
    name = "[C Class] Orc Archer Elimination",
    level_required = 36,
    npc_start = 79,
    npc_complete = 79,
    prerequisites = {299},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 300: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 79,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 300: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 300: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 51,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 300: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 79,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 300: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 300: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 79,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 300: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 300: Rewards given")
                self:give_rewards({
                    exp = 103656,
                    gold = 3000,
                    items = {
                        { item_id = 205, count = 50 },
                        { item_id = 21000096, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_300