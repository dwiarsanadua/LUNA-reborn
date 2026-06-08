--[[
  Quest 590: [Quest] For Skills
  Level Required: 150
  NPC Start: 301, NPC Complete: 301
]]

local fsm = require('fsm_engine')

local quest_590 = fsm:new({
    id = 590,
    name = "[Quest] For Skills",
    level_required = 150,
    npc_start = 301,
    npc_complete = 301,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 590: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 301,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 590: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 590: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 301,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 590: NPC talk objective met")
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
                self:log("Quest 590: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 301,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 590: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 590: Rewards given")
                self:give_rewards({
                    items = {
                        { item_id = 21001099, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_590