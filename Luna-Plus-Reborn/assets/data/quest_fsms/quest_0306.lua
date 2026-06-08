--[[
  Quest 306: [C Class] Furious Lizardman Elimination
  Level Required: 39
  NPC Start: 79, NPC Complete: 79
  Prerequisites: [305]
  Rewards: EXP=141077, Gold=3600
]]

local fsm = require('fsm_engine')

local quest_306 = fsm:new({
    id = 306,
    name = "[C Class] Furious Lizardman Elimination",
    level_required = 39,
    npc_start = 79,
    npc_complete = 79,
    prerequisites = {305},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 306: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 79,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 306: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 306: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 58,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 306: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 79,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 306: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 306: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 79,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 306: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 306: Rewards given")
                self:give_rewards({
                    exp = 141077,
                    gold = 3600,
                    items = {
                        { item_id = 211, count = 50 },
                        { item_id = 21000096, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_306