--[[
  Quest 455: [Change of Job] Way of Crusader
  Level Required: 105
  NPC Start: 10, NPC Complete: 122
  Prerequisites: [453]
]]

local fsm = require('fsm_engine')

local quest_455 = fsm:new({
    id = 455,
    name = "[Change of Job] Way of Crusader",
    level_required = 105,
    npc_start = 10,
    npc_complete = 122,
    prerequisites = {453},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 455: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 122,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 455: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 455: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 122,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 455: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 10,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 455: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 455: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 122,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 455: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 455: Rewards given")
                self:give_rewards({
                })
            end,
            transitions = {},
        },
    },
})

return quest_455