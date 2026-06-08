--[[
  Quest 128: Becoming a Resident
  Level Required: 150
  NPC Start: 2, NPC Complete: 49
  Rewards: EXP=377
]]

local fsm = require('fsm_engine')

local quest_128 = fsm:new({
    id = 128,
    name = "Becoming a Resident",
    level_required = 150,
    npc_start = 2,
    npc_complete = 49,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 128: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 2,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 128: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 128: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 2,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 128: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 49,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 128: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 128: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 49,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 128: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 128: Rewards given")
                self:give_rewards({
                    exp = 377,
                })
            end,
            transitions = {},
        },
    },
})

return quest_128