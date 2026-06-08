--[[
  Quest 259: A study of Contaminated water
  Level Required: 150
  NPC Start: 58, NPC Complete: 58
]]

local fsm = require('fsm_engine')

local quest_259 = fsm:new({
    id = 259,
    name = "A study of Contaminated water",
    level_required = 150,
    npc_start = 58,
    npc_complete = 58,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 259: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 58,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 259: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 259: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 11,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 259: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 26,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 259: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 32,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 259: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 58,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 259: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 259: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 58,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 259: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 259: Rewards given")
                self:give_rewards({
                    items = {
                        { item_id = 30000453, count = 1 },
                        { item_id = 30000454, count = 1 },
                        { item_id = 30000455, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_259