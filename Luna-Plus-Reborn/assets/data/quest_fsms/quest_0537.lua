--[[
  Quest 537: [Grand Master]Job Change
  Level Required: 150
  NPC Start: 120, NPC Complete: 120
]]

local fsm = require('fsm_engine')

local quest_537 = fsm:new({
    id = 537,
    name = "[Grand Master]Job Change",
    level_required = 150,
    npc_start = 120,
    npc_complete = 120,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 537: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 120,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 537: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 537: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 120,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 537: NPC talk objective met")
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
                self:log("Quest 537: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 120,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 537: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 537: Rewards given")
                self:give_rewards({
                })
            end,
            transitions = {},
        },
    },
})

return quest_537