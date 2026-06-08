--[[
  Quest 354: [Party] Survivors!
  Level Required: 37
  NPC Start: 59, NPC Complete: 89
  Prerequisites: [353]
  Rewards: EXP=69077, Gold=4000
]]

local fsm = require('fsm_engine')

local quest_354 = fsm:new({
    id = 354,
    name = "[Party] Survivors!",
    level_required = 37,
    npc_start = 59,
    npc_complete = 89,
    prerequisites = {353},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 354: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 59,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 354: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 354: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 59,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 354: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 87,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 354: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 88,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 354: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 89,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 354: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 354: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 89,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 354: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 354: Rewards given")
                self:give_rewards({
                    exp = 69077,
                    gold = 4000,
                    items = {
                        { item_id = 266, count = 1 },
                        { item_id = 267, count = 1 },
                        { item_id = 268, count = 1 },
                        { item_id = 269, count = 1 },
                        { item_id = 270, count = 1 },
                        { item_id = 271, count = 1 },
                        { item_id = 21000096, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_354