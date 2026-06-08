--[[
  Quest 263: Curious Chemistry
  Level Required: 150
  NPC Start: 57, NPC Complete: 57
  Rewards: EXP=60000, Gold=6000
]]

local fsm = require('fsm_engine')

local quest_263 = fsm:new({
    id = 263,
    name = "Curious Chemistry",
    level_required = 150,
    npc_start = 57,
    npc_complete = 57,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 263: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 57,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 263: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 263: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 53,
                    count = 2,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 263: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 54,
                    count = 2,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 263: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 58,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 263: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 57,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 263: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 11,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 263: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 57,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 263: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 263: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 57,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 263: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 263: Rewards given")
                self:give_rewards({
                    exp = 60000,
                    gold = 6000,
                    items = {
                        { item_id = 165, count = 1 },
                        { item_id = 166, count = 1 },
                        { item_id = 165, count = 30 },
                        { item_id = 166, count = 30 },
                        { item_id = 167, count = 1 },
                        { item_id = 168, count = 1 },
                        { item_id = 167, count = 1 },
                        { item_id = 168, count = 1 },
                        { item_id = 171, count = 1 },
                        { item_id = 169, count = 1 },
                        { item_id = 167, count = 1 },
                        { item_id = 169, count = 1 },
                        { item_id = 172, count = 1 },
                        { item_id = 170, count = 1 },
                        { item_id = 167, count = 1 },
                        { item_id = 170, count = 1 },
                        { item_id = 173, count = 1 },
                        { item_id = 171, count = 1 },
                        { item_id = 21000160, count = 1 },
                        { item_id = 21000010, count = 2 },
                        { item_id = 172, count = 1 },
                        { item_id = 21000161, count = 1 },
                        { item_id = 21000010, count = 2 },
                        { item_id = 173, count = 1 },
                        { item_id = 21000163, count = 1 },
                        { item_id = 21000010, count = 2 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_263