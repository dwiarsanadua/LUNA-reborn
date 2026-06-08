--[[
  Quest 2085: The Tragedy of the Top
  Level Required: 75
  NPC Start: 6, NPC Complete: 62
  Prerequisites: [286]
]]

local fsm = require('fsm_engine')

local quest_2085 = fsm:new({
    id = 2085,
    name = "The Tragedy of the Top",
    level_required = 75,
    npc_start = 6,
    npc_complete = 62,
    prerequisites = {286},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 2085: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 6,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 2085: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 2085: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 40,
                    count = 4,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2085: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 88,
                    count = 4,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2085: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 36,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2085: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 102,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2085: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 6,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2085: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 62,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2085: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 2085: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 62,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 2085: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 2085: Rewards given")
                self:give_rewards({
                    items = {
                        { item_id = 199, count = 1 },
                        { item_id = 199, count = 1 },
                        { item_id = 200, count = 1 },
                        { item_id = 200, count = 1 },
                        { item_id = 201, count = 1 },
                        { item_id = 201, count = 1 },
                        { item_id = 202, count = 1 },
                        { item_id = 202, count = 1 },
                        { item_id = 199, count = 5 },
                        { item_id = 200, count = 5 },
                        { item_id = 201, count = 15 },
                        { item_id = 202, count = 3 },
                        { item_id = 203, count = 1 },
                        { item_id = 203, count = 1 },
                        { item_id = 203, count = 25 },
                        { item_id = 30000486, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_2085