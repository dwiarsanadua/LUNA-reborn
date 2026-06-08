--[[
  Quest 749: [Quest] I'm the Chef!
  Level Required: 3
  NPC Start: 47, NPC Complete: 510
]]

local fsm = require('fsm_engine')

local quest_749 = fsm:new({
    id = 749,
    name = "[Quest] I'm the Chef!",
    level_required = 3,
    npc_start = 47,
    npc_complete = 510,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 749: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 47,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 749: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 749: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 47,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 749: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 510,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 749: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 749: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 510,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 749: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 749: Rewards given")
                self:give_rewards({
                })
            end,
            transitions = {},
        },
    },
})

return quest_749