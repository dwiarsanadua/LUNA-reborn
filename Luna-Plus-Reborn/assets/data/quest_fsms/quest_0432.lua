--[[
  Quest 432: [Party][Quest]The secret document
  Level Required: 85
  NPC Start: 99, NPC Complete: 99
  Prerequisites: [431]
  Rewards: EXP=3397655
]]

local fsm = require('fsm_engine')

local quest_432 = fsm:new({
    id = 432,
    name = "[Party][Quest]The secret document",
    level_required = 85,
    npc_start = 99,
    npc_complete = 99,
    prerequisites = {431},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 432: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 99,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 432: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 432: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 278,
                    count = 4,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 432: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 279,
                    count = 4,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 432: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 281,
                    count = 4,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 432: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 284,
                    count = 4,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 432: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 99,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 432: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 432: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 99,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 432: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 432: Rewards given")
                self:give_rewards({
                    exp = 3397655,
                    items = {
                        { item_id = 309, count = 1 },
                        { item_id = 310, count = 1 },
                        { item_id = 311, count = 1 },
                        { item_id = 312, count = 1 },
                        { item_id = 309, count = 1 },
                        { item_id = 310, count = 1 },
                        { item_id = 311, count = 1 },
                        { item_id = 312, count = 1 },
                        { item_id = 30000303, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_432