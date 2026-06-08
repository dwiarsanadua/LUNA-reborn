--[[
  Quest 298: [Elemental Master Job Change] Spiritual Support
  Level Required: 75
  NPC Start: 38, NPC Complete: 60
]]

local fsm = require('fsm_engine')

local quest_298 = fsm:new({
    id = 298,
    name = "[Elemental Master Job Change] Spiritual Support",
    level_required = 75,
    npc_start = 38,
    npc_complete = 60,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 298: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 38,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 298: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 298: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 38,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 298: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 60,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 298: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 298: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 60,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 298: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 298: Rewards given")
                self:give_rewards({
                    items = {
                        { item_id = 30000479, count = 1 },
                        { item_id = 30000498, count = 1 },
                        { item_id = 21000326, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_298