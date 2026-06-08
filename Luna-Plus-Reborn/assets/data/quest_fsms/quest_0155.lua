--[[
  Quest 155: Does a Body Good
  Level Required: 28
  NPC Start: 28, NPC Complete: 28
  Prerequisites: [127]
  Rewards: EXP=29722, Gold=2670
]]

local fsm = require('fsm_engine')

local quest_155 = fsm:new({
    id = 155,
    name = "Does a Body Good",
    level_required = 28,
    npc_start = 28,
    npc_complete = 28,
    prerequisites = {127},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 155: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 28,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 155: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 155: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 24,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 155: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 28,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 155: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 155: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 28,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 155: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 155: Rewards given")
                self:give_rewards({
                    exp = 29722,
                    gold = 2670,
                    items = {
                        { item_id = 69, count = 1 },
                        { item_id = 21000009, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_155