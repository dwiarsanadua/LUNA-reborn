--[[
  Quest 292: [Treasure Hunter Job Change] Greed is Good
  Level Required: 75
  NPC Start: 6, NPC Complete: 66
]]

local fsm = require('fsm_engine')

local quest_292 = fsm:new({
    id = 292,
    name = "[Treasure Hunter Job Change] Greed is Good",
    level_required = 75,
    npc_start = 6,
    npc_complete = 66,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 292: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 6,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 292: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 292: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 6,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 292: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 66,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 292: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 292: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 66,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 292: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 292: Rewards given")
                self:give_rewards({
                    items = {
                        { item_id = 30000473, count = 1 },
                        { item_id = 21000326, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_292