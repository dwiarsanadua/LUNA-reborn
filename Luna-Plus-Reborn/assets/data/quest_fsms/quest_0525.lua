--[[
  Quest 525: [Panzer]Job Change
  Level Required: 150
  NPC Start: 120, NPC Complete: 120
]]

local fsm = require('fsm_engine')

local quest_525 = fsm:new({
    id = 525,
    name = "[Panzer]Job Change",
    level_required = 150,
    npc_start = 120,
    npc_complete = 120,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 525: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 120,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 525: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 525: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 120,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 525: NPC talk objective met")
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
                self:log("Quest 525: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 120,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 525: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 525: Rewards given")
                self:give_rewards({
                })
            end,
            transitions = {},
        },
    },
})

return quest_525