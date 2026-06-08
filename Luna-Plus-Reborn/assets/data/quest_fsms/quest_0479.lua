--[[
  Quest 479: [Quest] The Heart of the Girl [The 9th]
  Level Required: 30
  NPC Start: 4, NPC Complete: 8
  Prerequisites: [478]
  Rewards: EXP=51435, Gold=15000
]]

local fsm = require('fsm_engine')

local quest_479 = fsm:new({
    id = 479,
    name = "[Quest] The Heart of the Girl [The 9th]",
    level_required = 30,
    npc_start = 4,
    npc_complete = 8,
    prerequisites = {478},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 479: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 8,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 479: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 479: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 186,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 479: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 246,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 479: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 247,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 479: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 248,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 479: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 392,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 479: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 8,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 479: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 4,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 479: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 479: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 8,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 479: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 479: Rewards given")
                self:give_rewards({
                    exp = 51435,
                    gold = 15000,
                    items = {
                        { item_id = 331, count = 1 },
                        { item_id = 30000560, count = 1 },
                        { item_id = 30000536, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_479