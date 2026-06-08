--[[
  Quest 445: Evidence of the Elimination
  Level Required: 55
  NPC Start: 119, NPC Complete: 119
  Prerequisites: [444]
  Rewards: EXP=361052, Gold=219600
]]

local fsm = require('fsm_engine')

local quest_445 = fsm:new({
    id = 445,
    name = "Evidence of the Elimination",
    level_required = 55,
    npc_start = 119,
    npc_complete = 119,
    prerequisites = {444},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 445: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 119,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 445: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 445: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 37,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 445: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 94,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 445: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 95,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 445: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 119,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 445: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 445: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 119,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 445: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 445: Rewards given")
                self:give_rewards({
                    exp = 361052,
                    gold = 219600,
                    items = {
                        { item_id = 318, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_445