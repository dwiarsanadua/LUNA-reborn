--[[
  Quest 868: The Oath of Friendship
  Level Required: 91
  NPC Start: 93, NPC Complete: 128
  Rewards: EXP=6487977, Gold=61188
]]

local fsm = require('fsm_engine')

local quest_868 = fsm:new({
    id = 868,
    name = "The Oath of Friendship",
    level_required = 91,
    npc_start = 93,
    npc_complete = 128,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 868: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 93,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 868: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 868: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 287,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 868: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 93,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 868: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 128,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 868: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 868: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 128,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 868: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 868: Rewards given")
                self:give_rewards({
                    exp = 6487977,
                    gold = 61188,
                    items = {
                        { item_id = 399, count = 1 },
                        { item_id = 399, count = 50 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_868