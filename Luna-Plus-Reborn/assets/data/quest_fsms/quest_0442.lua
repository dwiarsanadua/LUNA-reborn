--[[
  Quest 442: [Quest] The request from Ruiz
  Level Required: 53
  NPC Start: 117, NPC Complete: 119
  Prerequisites: [436]
]]

local fsm = require('fsm_engine')

local quest_442 = fsm:new({
    id = 442,
    name = "[Quest] The request from Ruiz",
    level_required = 53,
    npc_start = 117,
    npc_complete = 119,
    prerequisites = {436},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 442: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 117,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 442: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 442: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 117,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 442: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 119,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 442: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 442: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 119,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 442: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 442: Rewards given")
                self:give_rewards({
                })
            end,
            transitions = {},
        },
    },
})

return quest_442