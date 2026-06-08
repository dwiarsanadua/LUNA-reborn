--[[
  Quest 2079: Helping Hands
  Level Required: 75
  NPC Start: 6, NPC Complete: 36
  Prerequisites: [274]
]]

local fsm = require('fsm_engine')

local quest_2079 = fsm:new({
    id = 2079,
    name = "Helping Hands",
    level_required = 75,
    npc_start = 6,
    npc_complete = 36,
    prerequisites = {274},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 2079: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 10,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 2079: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 2079: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 98,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2079: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 10,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2079: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 6,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2079: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 13,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2079: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 29,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2079: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 31,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2079: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 36,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2079: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 2079: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 36,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 2079: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 2079: Rewards given")
                self:give_rewards({
                    items = {
                        { item_id = 21000005, count = 100 },
                        { item_id = 174, count = 1 },
                        { item_id = 175, count = 1 },
                        { item_id = 176, count = 1 },
                        { item_id = 176, count = 1 },
                        { item_id = 174, count = 1 },
                        { item_id = 175, count = 1 },
                        { item_id = 177, count = 1 },
                        { item_id = 177, count = 1 },
                        { item_id = 30000480, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_2079