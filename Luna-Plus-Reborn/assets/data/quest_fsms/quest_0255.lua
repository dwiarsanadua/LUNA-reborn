--[[
  Quest 255: The Slate of Gods
  Level Required: 150
  NPC Start: 68, NPC Complete: 68
]]

local fsm = require('fsm_engine')

local quest_255 = fsm:new({
    id = 255,
    name = "The Slate of Gods",
    level_required = 150,
    npc_start = 68,
    npc_complete = 68,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 255: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 68,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 255: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 255: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 68,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 255: NPC talk objective met")
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
                self:log("Quest 255: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 68,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 255: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 255: Rewards given")
                self:give_rewards({
                    items = {
                        { item_id = 30000451, count = 1 },
                        { item_id = 21000152, count = 1 },
                        { item_id = 21000010, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_255