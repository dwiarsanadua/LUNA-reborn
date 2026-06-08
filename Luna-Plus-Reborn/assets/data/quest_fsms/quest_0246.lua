--[[
  Quest 246: Simple Supply and Demand
  Level Required: 40
  NPC Start: 66, NPC Complete: 66
  Rewards: EXP=120375
]]

local fsm = require('fsm_engine')

local quest_246 = fsm:new({
    id = 246,
    name = "Simple Supply and Demand",
    level_required = 40,
    npc_start = 66,
    npc_complete = 66,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 246: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 66,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 246: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 246: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 60,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 246: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 66,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 246: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 246: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 66,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 246: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 246: Rewards given")
                self:give_rewards({
                    exp = 120375,
                    items = {
                        { item_id = 155, count = 1 },
                        { item_id = 155, count = 30 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_246