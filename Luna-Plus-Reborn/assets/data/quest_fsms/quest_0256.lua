--[[
  Quest 256: The Slate of Gods
  Level Required: 150
  NPC Start: 69, NPC Complete: 69
]]

local fsm = require('fsm_engine')

local quest_256 = fsm:new({
    id = 256,
    name = "The Slate of Gods",
    level_required = 150,
    npc_start = 69,
    npc_complete = 69,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 256: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 69,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 256: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 256: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 69,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 256: NPC talk objective met")
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
                self:log("Quest 256: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 69,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 256: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 256: Rewards given")
                self:give_rewards({
                    items = {
                        { item_id = 21000153, count = 1 },
                        { item_id = 21000010, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_256