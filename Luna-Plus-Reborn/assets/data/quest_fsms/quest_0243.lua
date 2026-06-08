--[[
  Quest 243: [Repeatable] A Heated Situation
  Level Required: 30
  NPC Start: 29, NPC Complete: 29
  Rewards: EXP=30861, Gold=5000
]]

local fsm = require('fsm_engine')

local quest_243 = fsm:new({
    id = 243,
    name = "[Repeatable] A Heated Situation",
    level_required = 30,
    npc_start = 29,
    npc_complete = 29,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 243: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 29,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 243: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 243: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 55,
                    count = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 243: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 29,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 243: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 243: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 29,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 243: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 243: Rewards given")
                self:give_rewards({
                    exp = 30861,
                    gold = 5000,
                    items = {
                        { item_id = 21000248, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_243