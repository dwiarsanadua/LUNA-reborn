--[[
  Quest 2081: The Remains of the Temple
  Level Required: 75
  NPC Start: 9, NPC Complete: 55
  Prerequisites: [276]
]]

local fsm = require('fsm_engine')

local quest_2081 = fsm:new({
    id = 2081,
    name = "The Remains of the Temple",
    level_required = 75,
    npc_start = 9,
    npc_complete = 55,
    prerequisites = {276},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 2081: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 9,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 2081: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 2081: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 64,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2081: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 9,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2081: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 55,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2081: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 28,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2081: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 32,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2081: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 34,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2081: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 24,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2081: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 2081: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 55,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 2081: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 2081: Rewards given")
                self:give_rewards({
                    items = {
                        { item_id = 30000506, count = 1 },
                        { item_id = 30000507, count = 1 },
                        { item_id = 30000505, count = 1 },
                        { item_id = 183, count = 15 },
                        { item_id = 184, count = 1 },
                        { item_id = 30000482, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_2081