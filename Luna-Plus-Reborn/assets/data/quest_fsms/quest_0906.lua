--[[
  Quest 906: I want to sleep comfortably
  Level Required: 81
  NPC Start: 92, NPC Complete: 92
  Rewards: EXP=2266495
]]

local fsm = require('fsm_engine')

local quest_906 = fsm:new({
    id = 906,
    name = "I want to sleep comfortably",
    level_required = 81,
    npc_start = 92,
    npc_complete = 92,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 906: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 92,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 906: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 906: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 273,
                    count = 100,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 906: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 92,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 906: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 906: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 92,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 906: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 906: Rewards given")
                self:give_rewards({
                    exp = 2266495,
                })
            end,
            transitions = {},
        },
    },
})

return quest_906