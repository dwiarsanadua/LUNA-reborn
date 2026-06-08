--[[
  Quest 437: [Quest] A request for Melina
  Level Required: 53
  NPC Start: 117, NPC Complete: 118
  Prerequisites: [436]
]]

local fsm = require('fsm_engine')

local quest_437 = fsm:new({
    id = 437,
    name = "[Quest] A request for Melina",
    level_required = 53,
    npc_start = 117,
    npc_complete = 118,
    prerequisites = {436},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 437: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 117,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 437: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 437: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 117,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 437: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 118,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 437: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 437: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 118,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 437: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 437: Rewards given")
                self:give_rewards({
                })
            end,
            transitions = {},
        },
    },
})

return quest_437