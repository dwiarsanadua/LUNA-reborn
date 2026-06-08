--[[
  Quest 248: The Red Harpy Statue
  Level Required: 42
  NPC Start: 66, NPC Complete: 66
  Prerequisites: [247]
  Rewards: EXP=108703, Gold=10000
]]

local fsm = require('fsm_engine')

local quest_248 = fsm:new({
    id = 248,
    name = "The Red Harpy Statue",
    level_required = 42,
    npc_start = 66,
    npc_complete = 66,
    prerequisites = {247},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 248: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 66,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 248: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 248: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 77,
                    count = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 248: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 77,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 248: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 66,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 248: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 248: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 66,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 248: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 248: Rewards given")
                self:give_rewards({
                    exp = 108703,
                    gold = 10000,
                    items = {
                        { item_id = 158, count = 1 },
                        { item_id = 159, count = 1 },
                        { item_id = 158, count = 1 },
                        { item_id = 30000432, count = 1 },
                        { item_id = 30000435, count = 20 },
                        { item_id = 159, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_248