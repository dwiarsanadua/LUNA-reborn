--[[
  Quest 99: Defensive Measures
  Level Required: 19
  NPC Start: 15, NPC Complete: 15
  Prerequisites: [103]
  Rewards: EXP=7646, Gold=22939
]]

local fsm = require('fsm_engine')

local quest_99 = fsm:new({
    id = 99,
    name = "Defensive Measures",
    level_required = 19,
    npc_start = 15,
    npc_complete = 15,
    prerequisites = {103},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 99: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 15,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 99: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 99: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 42,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 99: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 15,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 99: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 99: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 15,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 99: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 99: Rewards given")
                self:give_rewards({
                    exp = 7646,
                    gold = 22939,
                    items = {
                        { item_id = 42, count = 5 },
                        { item_id = 21000009, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_99