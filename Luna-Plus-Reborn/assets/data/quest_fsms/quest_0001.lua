--[[
  Quest 1: Rien's Tips
  Level Required: 1
  NPC Start: 2, NPC Complete: 2
  Rewards: EXP=7, Gold=780
]]

local fsm = require('fsm_engine')

local quest_1 = fsm:new({
    id = 1,
    name = "Rien's Tips",
    level_required = 1,
    npc_start = 2,
    npc_complete = 2,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 1: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 2,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 1: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 1: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 3,
                    count = 10,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 1: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 18,
                    count = 8,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 1: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 2,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 1: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 1: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 2,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 1: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 1: Rewards given")
                self:give_rewards({
                    exp = 7,
                    gold = 780,
                    items = {
                        { item_id = 21000001, count = 5 },
                        { item_id = 21000006, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_1