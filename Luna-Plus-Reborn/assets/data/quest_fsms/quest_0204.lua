--[[
  Quest 204: Floating Stone Supply
  Level Required: 24
  NPC Start: 5, NPC Complete: 54
  Rewards: EXP=88336, Gold=9240
]]

local fsm = require('fsm_engine')

local quest_204 = fsm:new({
    id = 204,
    name = "Floating Stone Supply",
    level_required = 24,
    npc_start = 5,
    npc_complete = 54,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 204: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 7,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 204: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 204: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 12,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 204: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 15,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 204: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 14,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 204: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 9,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 204: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 7,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 204: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 54,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 204: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 5,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 204: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 8,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 204: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 49,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 204: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 11,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 204: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 204: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 54,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 204: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 204: Rewards given")
                self:give_rewards({
                    exp = 88336,
                    gold = 9240,
                    items = {
                        { item_id = 85, count = 1 },
                        { item_id = 85, count = 10 },
                        { item_id = 86, count = 1 },
                        { item_id = 86, count = 1 },
                        { item_id = 87, count = 1 },
                        { item_id = 13000126, count = 1 },
                        { item_id = 87, count = 10 },
                        { item_id = 86, count = 1 },
                        { item_id = 88, count = 1 },
                        { item_id = 13000125, count = 1 },
                        { item_id = 88, count = 3 },
                        { item_id = 86, count = 1 },
                        { item_id = 89, count = 1 },
                        { item_id = 89, count = 2 },
                        { item_id = 13000123, count = 1 },
                        { item_id = 86, count = 1 },
                        { item_id = 90, count = 1 },
                        { item_id = 90, count = 15 },
                        { item_id = 13000124, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_204