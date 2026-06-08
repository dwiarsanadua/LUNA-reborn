--[[
  Quest 225: Silence the Sound Again
  Level Required: 1
  NPC Start: 23, NPC Complete: 27
  Prerequisites: [122]
]]

local fsm = require('fsm_engine')

local quest_225 = fsm:new({
    id = 225,
    name = "Silence the Sound Again",
    level_required = 1,
    npc_start = 23,
    npc_complete = 27,
    prerequisites = {122},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 225: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 23,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 225: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 225: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 23,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 225: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 27,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 225: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 225: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 27,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 225: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 225: Rewards given")
                self:give_rewards({
                })
            end,
            transitions = {},
        },
    },
})

return quest_225