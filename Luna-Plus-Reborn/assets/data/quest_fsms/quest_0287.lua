--[[
  Quest 287: [Phalanx Job Change] Impenetrable Defense
  Level Required: 75
  NPC Start: 10, NPC Complete: 31
]]

local fsm = require('fsm_engine')

local quest_287 = fsm:new({
    id = 287,
    name = "[Phalanx Job Change] Impenetrable Defense",
    level_required = 75,
    npc_start = 10,
    npc_complete = 31,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 287: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 10,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 287: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 287: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 10,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 287: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 31,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 287: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 287: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 31,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 287: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 287: Rewards given")
                self:give_rewards({
                    items = {
                        { item_id = 30000468, count = 1 },
                        { item_id = 21000326, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_287