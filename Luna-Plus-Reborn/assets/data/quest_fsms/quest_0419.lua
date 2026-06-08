--[[
  Quest 419: Home Improvement
  Level Required: 23
  NPC Start: 25, NPC Complete: 26
  Rewards: EXP=6338, Gold=300
]]

local fsm = require('fsm_engine')

local quest_419 = fsm:new({
    id = 419,
    name = "Home Improvement",
    level_required = 23,
    npc_start = 25,
    npc_complete = 26,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 419: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 26,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 419: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 419: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 26,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 419: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 25,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 419: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 419: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 26,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 419: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 419: Rewards given")
                self:give_rewards({
                    exp = 6338,
                    gold = 300,
                    items = {
                        { item_id = 21000009, count = 3 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_419