--[[
  Quest 401: Quest 401
  Level Required: 59
  NPC Start: 90, NPC Complete: 90
  Prerequisites: [400]
  Rewards: EXP=470732, Gold=19000
]]

local fsm = require('fsm_engine')

local quest_401 = fsm:new({
    id = 401,
    name = "Quest 401",
    level_required = 59,
    npc_start = 90,
    npc_complete = 90,
    prerequisites = {400},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 401: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 90,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 401: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 401: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 100,
                    count = 70,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 401: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 90,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 401: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 401: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 90,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 401: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 401: Rewards given")
                self:give_rewards({
                    exp = 470732,
                    gold = 19000,
                })
            end,
            transitions = {},
        },
    },
})

return quest_401