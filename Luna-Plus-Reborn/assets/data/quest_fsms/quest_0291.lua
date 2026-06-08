--[[
  Quest 291: [Ranger Job Change] Eagle Eye
  Level Required: 75
  NPC Start: 13, NPC Complete: 20
]]

local fsm = require('fsm_engine')

local quest_291 = fsm:new({
    id = 291,
    name = "[Ranger Job Change] Eagle Eye",
    level_required = 75,
    npc_start = 13,
    npc_complete = 20,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 291: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 20,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 291: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 291: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 20,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 291: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 13,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 291: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 291: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 20,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 291: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 291: Rewards given")
                self:give_rewards({
                    items = {
                        { item_id = 30000472, count = 1 },
                        { item_id = 21000326, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_291