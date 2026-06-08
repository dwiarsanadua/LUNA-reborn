--[[
  Quest 362: [Special] Compound Water Research
  Level Required: 150
  NPC Start: 58, NPC Complete: 58
  Prerequisites: [358]
]]

local fsm = require('fsm_engine')

local quest_362 = fsm:new({
    id = 362,
    name = "[Special] Compound Water Research",
    level_required = 150,
    npc_start = 58,
    npc_complete = 58,
    prerequisites = {358},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 362: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 58,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 362: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 362: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 11,
                    count = 3,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 362: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 26,
                    count = 3,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 362: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 32,
                    count = 3,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 362: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 58,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 362: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 362: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 58,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 362: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 362: Rewards given")
                self:give_rewards({
                    items = {
                        { item_id = 284, count = 1 },
                        { item_id = 288, count = 1 },
                        { item_id = 286, count = 1 },
                        { item_id = 284, count = 1 },
                        { item_id = 286, count = 1 },
                        { item_id = 288, count = 1 },
                        { item_id = 30000456, count = 1 },
                        { item_id = 30000457, count = 1 },
                        { item_id = 30000458, count = 1 },
                        { item_id = 21000010, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_362