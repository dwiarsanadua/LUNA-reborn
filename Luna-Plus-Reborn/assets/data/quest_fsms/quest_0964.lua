--[[
  Quest 964: [Dungeon Quest]Evil Towers(Beginner)
  Level Required: 30
  NPC Start: 545, NPC Complete: 545
  Rewards: EXP=30861, Gold=2994
]]

local fsm = require('fsm_engine')

local quest_964 = fsm:new({
    id = 964,
    name = "[Dungeon Quest]Evil Towers(Beginner)",
    level_required = 30,
    npc_start = 545,
    npc_complete = 545,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 964: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 545,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 964: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 964: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 461,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 964: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 462,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 964: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 545,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 964: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 964: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 545,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 964: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 964: Rewards given")
                self:give_rewards({
                    exp = 30861,
                    gold = 2994,
                    items = {
                        { item_id = 21000248, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_964