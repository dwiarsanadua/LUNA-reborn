--[[
  Quest 74: Delivering The New Weapons
  Level Required: 4
  NPC Start: 1, NPC Complete: 48
  Rewards: EXP=50, Gold=795
]]

local fsm = require('fsm_engine')

local quest_74 = fsm:new({
    id = 74,
    name = "Delivering The New Weapons",
    level_required = 4,
    npc_start = 1,
    npc_complete = 48,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 74: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 1,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 74: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 74: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 74: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 47,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 74: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 48,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 74: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 74: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 48,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 74: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 74: Rewards given")
                self:give_rewards({
                    exp = 50,
                    gold = 795,
                    items = {
                        { item_id = 26, count = 1 },
                        { item_id = 27, count = 1 },
                        { item_id = 26, count = 1 },
                        { item_id = 27, count = 1 },
                        { item_id = 21000006, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_74