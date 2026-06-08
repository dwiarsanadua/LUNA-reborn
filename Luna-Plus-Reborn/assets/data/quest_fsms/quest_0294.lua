--[[
  Quest 294: [Rune Walker Job Change] Spirit Feather
  Level Required: 75
  NPC Start: 33, NPC Complete: 40
]]

local fsm = require('fsm_engine')

local quest_294 = fsm:new({
    id = 294,
    name = "[Rune Walker Job Change] Spirit Feather",
    level_required = 75,
    npc_start = 33,
    npc_complete = 40,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 294: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 33,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 294: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 294: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 33,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 294: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 40,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 294: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 294: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 40,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 294: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 294: Rewards given")
                self:give_rewards({
                    items = {
                        { item_id = 30000475, count = 1 },
                        { item_id = 21000326, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_294