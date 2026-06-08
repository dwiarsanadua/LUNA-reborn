--[[
  Quest 297: [Inquirer Job Change] Devoted Discipline
  Level Required: 75
  NPC Start: 34, NPC Complete: 54
]]

local fsm = require('fsm_engine')

local quest_297 = fsm:new({
    id = 297,
    name = "[Inquirer Job Change] Devoted Discipline",
    level_required = 75,
    npc_start = 34,
    npc_complete = 54,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 297: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 34,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 297: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 297: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 34,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 297: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 54,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 297: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 297: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 54,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 297: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 297: Rewards given")
                self:give_rewards({
                    items = {
                        { item_id = 30000478, count = 1 },
                        { item_id = 21000326, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_297