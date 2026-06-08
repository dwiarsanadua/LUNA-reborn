--[[
  Quest 42: Collecting bones
  Level Required: 50
  NPC Start: 44, NPC Complete: 44
  Rewards: EXP=291730, Gold=15463
]]

local fsm = require('fsm_engine')

local quest_42 = fsm:new({
    id = 42,
    name = "Collecting bones",
    level_required = 50,
    npc_start = 44,
    npc_complete = 44,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 42: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 44,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 42: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 42: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 13,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 42: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 61,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 42: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 62,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 42: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 44,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 42: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 42: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 44,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 42: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 42: Rewards given")
                self:give_rewards({
                    exp = 291730,
                    gold = 15463,
                    items = {
                        { item_id = 17, count = 1 },
                        { item_id = 17, count = 1 },
                        { item_id = 17, count = 1 },
                        { item_id = 17, count = 20 },
                        { item_id = 21000004, count = 15 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_42