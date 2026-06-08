--[[
  Quest 465: [Change of Job] Way of Grand Master
  Level Required: 105
  NPC Start: 7, NPC Complete: 122
  Prerequisites: [453]
]]

local fsm = require('fsm_engine')

local quest_465 = fsm:new({
    id = 465,
    name = "[Change of Job] Way of Grand Master",
    level_required = 105,
    npc_start = 7,
    npc_complete = 122,
    prerequisites = {453},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 465: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 122,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 465: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 465: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 122,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 465: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 7,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 465: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 465: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 122,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 465: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 465: Rewards given")
                self:give_rewards({
                })
            end,
            transitions = {},
        },
    },
})

return quest_465