--[[
  Quest 538: [Necromancer]Job Change
  Level Required: 150
  NPC Start: 120, NPC Complete: 120
]]

local fsm = require('fsm_engine')

local quest_538 = fsm:new({
    id = 538,
    name = "[Necromancer]Job Change",
    level_required = 150,
    npc_start = 120,
    npc_complete = 120,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 538: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 120,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 538: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 538: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 120,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 538: NPC talk objective met")
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
                self:log("Quest 538: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 120,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 538: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 538: Rewards given")
                self:give_rewards({
                })
            end,
            transitions = {},
        },
    },
})

return quest_538