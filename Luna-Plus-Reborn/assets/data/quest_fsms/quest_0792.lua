--[[
  Quest 792: Vendetta
  Level Required: 57
  NPC Start: 43, NPC Complete: 43
  Rewards: EXP=318693, Gold=13316
]]

local fsm = require('fsm_engine')

local quest_792 = fsm:new({
    id = 792,
    name = "Vendetta",
    level_required = 57,
    npc_start = 43,
    npc_complete = 43,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 792: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 43,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 792: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 792: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 35,
                    count = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 792: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 43,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 792: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 792: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 43,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 792: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 792: Rewards given")
                self:give_rewards({
                    exp = 318693,
                    gold = 13316,
                    items = {
                        { item_id = 21001505, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_792