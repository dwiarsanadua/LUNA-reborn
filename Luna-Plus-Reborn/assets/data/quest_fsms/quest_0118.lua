--[[
  Quest 118: [Repeatable] Bad Moon Rising
  Level Required: 23
  NPC Start: 26, NPC Complete: 26
  Prerequisites: [117]
  Rewards: EXP=10563, Gold=2060
]]

local fsm = require('fsm_engine')

local quest_118 = fsm:new({
    id = 118,
    name = "[Repeatable] Bad Moon Rising",
    level_required = 23,
    npc_start = 26,
    npc_complete = 26,
    prerequisites = {117},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 118: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 26,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 118: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 118: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 11,
                    count = 15,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 118: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 26,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 118: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 118: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 26,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 118: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 118: Rewards given")
                self:give_rewards({
                    exp = 10563,
                    gold = 2060,
                    items = {
                        { item_id = 21000009, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_118