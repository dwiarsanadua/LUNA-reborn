--[[
  Quest 171: [Repeatable] Defensive Measures
  Level Required: 19
  NPC Start: 15, NPC Complete: 15
  Prerequisites: [99]
  Rewards: EXP=7646, Gold=1995
]]

local fsm = require('fsm_engine')

local quest_171 = fsm:new({
    id = 171,
    name = "[Repeatable] Defensive Measures",
    level_required = 19,
    npc_start = 15,
    npc_complete = 15,
    prerequisites = {99},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 171: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 15,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 171: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 171: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 42,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 171: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 15,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 171: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 171: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 15,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 171: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 171: Rewards given")
                self:give_rewards({
                    exp = 7646,
                    gold = 1995,
                    items = {
                        { item_id = 42, count = 1 },
                        { item_id = 42, count = 5 },
                        { item_id = 21000009, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_171