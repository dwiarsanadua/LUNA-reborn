--[[
  Quest 467: [Change of Job] Way of Panzer
  Level Required: 105
  NPC Start: 122, NPC Complete: 125
  Prerequisites: [453]
]]

local fsm = require('fsm_engine')

local quest_467 = fsm:new({
    id = 467,
    name = "[Change of Job] Way of Panzer",
    level_required = 105,
    npc_start = 122,
    npc_complete = 125,
    prerequisites = {453},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 467: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 122,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 467: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 467: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 122,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 467: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 125,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 467: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 467: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 125,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 467: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 467: Rewards given")
                self:give_rewards({
                })
            end,
            transitions = {},
        },
    },
})

return quest_467