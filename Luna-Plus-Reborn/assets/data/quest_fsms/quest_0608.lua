--[[
  Quest 608: Quest 608
  Level Required: 120
  NPC Start: 400, NPC Complete: 400
]]

local fsm = require('fsm_engine')

local quest_608 = fsm:new({
    id = 608,
    name = "Quest 608",
    level_required = 120,
    npc_start = 400,
    npc_complete = 400,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 608: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 400,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 608: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 608: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 400,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 608: NPC talk objective met")
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
                self:log("Quest 608: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 400,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 608: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 608: Rewards given")
                self:give_rewards({
                })
            end,
            transitions = {},
        },
    },
})

return quest_608