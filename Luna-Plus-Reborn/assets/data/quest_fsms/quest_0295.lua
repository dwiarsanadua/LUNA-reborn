--[[
  Quest 295: [Bishop Job Change] Healing Magic
  Level Required: 75
  NPC Start: 9, NPC Complete: 32
]]

local fsm = require('fsm_engine')

local quest_295 = fsm:new({
    id = 295,
    name = "[Bishop Job Change] Healing Magic",
    level_required = 75,
    npc_start = 9,
    npc_complete = 32,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 295: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 9,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 295: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 295: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 9,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 295: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 32,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 295: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 295: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 32,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 295: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 295: Rewards given")
                self:give_rewards({
                    items = {
                        { item_id = 30000476, count = 1 },
                        { item_id = 21000326, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_295