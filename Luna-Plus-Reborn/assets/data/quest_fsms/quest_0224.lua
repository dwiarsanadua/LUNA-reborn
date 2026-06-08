--[[
  Quest 224: [Party] Evil Omen
  Level Required: 25
  NPC Start: 64, NPC Complete: 64
  Prerequisites: [223]
  Rewards: EXP=100982, Gold=8610
]]

local fsm = require('fsm_engine')

local quest_224 = fsm:new({
    id = 224,
    name = "[Party] Evil Omen",
    level_required = 25,
    npc_start = 64,
    npc_complete = 64,
    prerequisites = {223},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 224: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 64,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 224: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 224: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 114,
                    count = 5,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 224: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 115,
                    count = 5,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 224: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 116,
                    count = 5,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 224: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 133,
                    count = 5,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 224: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 134,
                    count = 5,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 224: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 64,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 224: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 224: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 64,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 224: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 224: Rewards given")
                self:give_rewards({
                    exp = 100982,
                    gold = 8610,
                    items = {
                        { item_id = 141, count = 1 },
                        { item_id = 142, count = 1 },
                        { item_id = 143, count = 1 },
                        { item_id = 144, count = 1 },
                        { item_id = 145, count = 1 },
                        { item_id = 21000010, count = 5 },
                        { item_id = 21001524, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_224