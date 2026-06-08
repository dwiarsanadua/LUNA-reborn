--[[
  Quest 137: [Repeatable] Stolen Ore
  Level Required: 22
  NPC Start: 55, NPC Complete: 55
  Prerequisites: [136]
  Rewards: EXP=13660, Gold=2940
]]

local fsm = require('fsm_engine')

local quest_137 = fsm:new({
    id = 137,
    name = "[Repeatable] Stolen Ore",
    level_required = 22,
    npc_start = 55,
    npc_complete = 55,
    prerequisites = {136},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 137: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 55,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 137: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 137: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 6,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 137: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 48,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 137: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 49,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 137: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 42,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 137: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 34,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 137: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 33,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 137: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 55,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 137: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 137: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 55,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 137: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 137: Rewards given")
                self:give_rewards({
                    exp = 13660,
                    gold = 2940,
                    items = {
                        { item_id = 47, count = 20 },
                        { item_id = 21000009, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_137