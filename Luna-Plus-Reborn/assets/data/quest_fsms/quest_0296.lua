--[[
  Quest 296: [Warlock Job Change] Absolute Power
  Level Required: 75
  NPC Start: 7, NPC Complete: 24
]]

local fsm = require('fsm_engine')

local quest_296 = fsm:new({
    id = 296,
    name = "[Warlock Job Change] Absolute Power",
    level_required = 75,
    npc_start = 7,
    npc_complete = 24,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 296: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 7,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 296: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 296: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 7,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 296: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 24,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 296: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 296: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 24,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 296: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 296: Rewards given")
                self:give_rewards({
                    items = {
                        { item_id = 30000477, count = 1 },
                        { item_id = 30000496, count = 1 },
                        { item_id = 21000326, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_296