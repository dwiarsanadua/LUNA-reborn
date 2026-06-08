--[[
  Quest 2084: Opening the Way
  Level Required: 75
  NPC Start: 19, NPC Complete: 77
  Prerequisites: [284]
]]

local fsm = require('fsm_engine')

local quest_2084 = fsm:new({
    id = 2084,
    name = "Opening the Way",
    level_required = 75,
    npc_start = 19,
    npc_complete = 77,
    prerequisites = {284},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 2084: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 19,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 2084: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 2084: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 13,
                    count = 2,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2084: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 61,
                    count = 2,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2084: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 62,
                    count = 2,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2084: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 46,
                    count = 2,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2084: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 35,
                    count = 2,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2084: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 64,
                    count = 100,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2084: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 19,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2084: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 36,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2084: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 73,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2084: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 76,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2084: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 77,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2084: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 2084: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 77,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 2084: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 2084: Rewards given")
                self:give_rewards({
                    items = {
                        { item_id = 193, count = 1 },
                        { item_id = 193, count = 1 },
                        { item_id = 194, count = 1 },
                        { item_id = 194, count = 1 },
                        { item_id = 194, count = 1 },
                        { item_id = 195, count = 1 },
                        { item_id = 195, count = 1 },
                        { item_id = 194, count = 15 },
                        { item_id = 195, count = 15 },
                        { item_id = 196, count = 1 },
                        { item_id = 197, count = 1 },
                        { item_id = 198, count = 1 },
                        { item_id = 196, count = 1 },
                        { item_id = 197, count = 1 },
                        { item_id = 198, count = 1 },
                        { item_id = 30000485, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_2084