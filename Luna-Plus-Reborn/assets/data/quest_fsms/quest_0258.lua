--[[
  Quest 258: Parts of a power generator
  Level Required: 150
  NPC Start: 25, NPC Complete: 25
]]

local fsm = require('fsm_engine')

local quest_258 = fsm:new({
    id = 258,
    name = "Parts of a power generator",
    level_required = 150,
    npc_start = 25,
    npc_complete = 25,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 258: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 25,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 258: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 258: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 25,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 258: NPC talk objective met")
                    end
                },
                {
                    trigger = "any",
                    target = "COMPLETE",
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 258: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 25,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 258: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 258: Rewards given")
                self:give_rewards({
                    items = {
                        { item_id = 21000157, count = 1 },
                        { item_id = 21000010, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_258