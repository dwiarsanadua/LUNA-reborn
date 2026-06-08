--[[
  Quest 293: [Assassin Job Change] Among the Shadows
  Level Required: 75
  NPC Start: 11, NPC Complete: 39
]]

local fsm = require('fsm_engine')

local quest_293 = fsm:new({
    id = 293,
    name = "[Assassin Job Change] Among the Shadows",
    level_required = 75,
    npc_start = 11,
    npc_complete = 39,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 293: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 11,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 293: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 293: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 11,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 293: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 39,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 293: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 293: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 39,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 293: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 293: Rewards given")
                self:give_rewards({
                    items = {
                        { item_id = 30000474, count = 1 },
                        { item_id = 21000326, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_293