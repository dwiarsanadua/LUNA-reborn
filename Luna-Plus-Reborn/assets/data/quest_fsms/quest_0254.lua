--[[
  Quest 254: The Slate of Gods
  Level Required: 150
  NPC Start: 67, NPC Complete: 67
]]

local fsm = require('fsm_engine')

local quest_254 = fsm:new({
    id = 254,
    name = "The Slate of Gods",
    level_required = 150,
    npc_start = 67,
    npc_complete = 67,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 254: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 67,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 254: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 254: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 67,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 254: NPC talk objective met")
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
                self:log("Quest 254: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 67,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 254: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 254: Rewards given")
                self:give_rewards({
                    items = {
                        { item_id = 30000450, count = 1 },
                        { item_id = 21000151, count = 1 },
                        { item_id = 21000010, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_254