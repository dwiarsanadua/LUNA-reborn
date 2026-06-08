--[[
  Quest 283: [Emblem of Honor] Crash Cleanup
  Level Required: 70
  NPC Start: 19, NPC Complete: 36
  Rewards: EXP=420000, Gold=30000
]]

local fsm = require('fsm_engine')

local quest_283 = fsm:new({
    id = 283,
    name = "[Emblem of Honor] Crash Cleanup",
    level_required = 70,
    npc_start = 19,
    npc_complete = 36,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 283: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 19,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 283: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 283: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 13,
                    count = 2,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 283: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 61,
                    count = 2,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 283: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 62,
                    count = 2,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 283: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 46,
                    count = 2,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 283: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 35,
                    count = 2,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 283: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 19,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 283: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 36,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 283: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 283: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 36,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 283: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 283: Rewards given")
                self:give_rewards({
                    exp = 420000,
                    gold = 30000,
                    items = {
                        { item_id = 193, count = 1 },
                        { item_id = 193, count = 1 },
                        { item_id = 194, count = 1 },
                        { item_id = 194, count = 1 },
                        { item_id = 194, count = 1 },
                        { item_id = 195, count = 1 },
                        { item_id = 195, count = 1 },
                        { item_id = 194, count = 15 },
                        { item_id = 195, count = 15 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_283