--[[
  Quest 2080: Regional Survey
  Level Required: 75
  NPC Start: 7, NPC Complete: 54
  Prerequisites: [275]
]]

local fsm = require('fsm_engine')

local quest_2080 = fsm:new({
    id = 2080,
    name = "Regional Survey",
    level_required = 75,
    npc_start = 7,
    npc_complete = 54,
    prerequisites = {275},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 2080: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 7,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 2080: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 2080: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 48,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2080: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 49,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2080: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 33,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2080: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 42,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2080: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 4,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2080: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 6,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2080: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 51,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2080: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 57,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2080: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 58,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2080: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 56,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2080: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 32,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2080: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 77,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2080: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 87,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2080: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 92,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2080: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 40,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2080: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 38,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2080: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 97,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2080: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 96,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2080: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 98,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2080: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 37,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2080: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 94,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2080: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 95,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2080: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 65,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2080: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 66,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2080: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 16,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2080: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 63,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2080: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 28,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2080: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 64,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2080: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 7,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2080: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 54,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2080: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 2080: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 54,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 2080: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 2080: Rewards given")
                self:give_rewards({
                    items = {
                        { item_id = 178, count = 5 },
                        { item_id = 179, count = 5 },
                        { item_id = 180, count = 5 },
                        { item_id = 181, count = 5 },
                        { item_id = 182, count = 1 },
                        { item_id = 30000481, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_2080